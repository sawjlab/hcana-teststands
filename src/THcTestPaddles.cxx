///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcTestPaddles                                                              //
//                                                                           //
// A simple class to handle test stand scintillator paddles                  //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "THcSignalHit.h"
#include "THcHodoHit.h"
#include "THcShower.h"
#include "THcCherenkov.h"
#include "THcHallCSpectrometer.h"

#include "THcHitList.h"
#include "THcRawShowerHit.h"
#include "TClass.h"
#include "math.h"
#include "THaSubDetector.h"

#include "THcTestPaddles.h"
#include "THaEvData.h"
#include "THaDetMap.h"
#include "THcDetectorMap.h"
#include "THaGlobals.h"
#include "THaCutList.h"
#include "THcGlobals.h"
#include "THcParmList.h"
#include "VarDef.h"
#include "VarType.h"
#include "THaTrack.h"
#include "TClonesArray.h"
#include "TMath.h"

#include "THaTrackProj.h"
#include <vector>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;
using std::vector;

//_____________________________________________________________________________
THcTestPaddles::THcTestPaddles( const char* name, const char* description,
				  THaApparatus* apparatus ) :
  THcHodoscope(name,description,apparatus)
{
  // Constructor

  //fTrackProj = new TClonesArray( "THaTrackProj", 5 );
  // Construct the planes
  fNPaddles = 0;
  fStartTime=-1e5;
  fGoodStartTime=kFALSE;
}

//_____________________________________________________________________________
THcTestPaddles::THcTestPaddles( ) :
  THcHodoscope()
{
  // Constructor
}

//_____________________________________________________________________________
void THcTestPaddles::Setup(const char* name, const char* description)
{

  //  static const char* const here = "Setup()";
  //  static const char* const message = 
  //    "Must construct %s detector with valid name! Object construction failed.";

  cout << "In THcTestPaddles::Setup()" << endl;
  // Base class constructor failed?
  if( IsZombie()) return;

  fDebug   = 1;  // Keep this at one while we're working on the code    

}

//_____________________________________________________________________________
THaAnalysisObject::EStatus THcTestPaddles::Init( const TDatime& date )
{
  cout << "In THcTestPaddles::Init()" << endl;
  Setup(GetName(), GetTitle());

  // Should probably put this in ReadDatabase as we will know the
  // maximum number of hits after setting up the detector map
  // But it needs to happen before the sub detectors are initialized
  // so that they can get the pointer to the hitlist.


  InitHitList(fDetMap, "THcRawHodoHit", 100);

  EStatus status;
  // This triggers call of ReadDatabase and DefineVariables
  if( (status = THaNonTrackingDetector::Init( date )) )
    return fStatus=status;

  char EngineDID[]="xSCIN";
  EngineDID[0] = toupper(GetApparatus()->GetName()[0]);
  if( gHcDetectorMap->FillMap(fDetMap, EngineDID) < 0 ) {
    static const char* const here = "Init()";
    Error( Here(here), "Error filling detectormap for %s.", 
	     EngineDID);
      return kInitError;
  }

  return fStatus = kOK;
}
//_____________________________________________________________________________
Int_t THcTestPaddles::ReadDatabase( const TDatime& date )
{

  // Read this detector's parameters from the database file 'fi'.
  // This function is called by THaDetectorBase::Init() once at the
  // beginning of the analysis.
  // 'date' contains the date/time of the run being analyzed.

  //  static const char* const here = "ReadDatabase()";
  char prefix[2];

  prefix[0]=tolower(GetApparatus()->GetName()[0]);
  prefix[1]='\0';

  DBRequest listextra[]={
    //    {"npaddles", &fNPaddles, kInt},
    {"ref_offset", &fRefOffset, kDouble},
    {"ref_nsperchan", &fRefNSPerChan, kDouble},
    {0}
  };

  gHcParms->LoadParmValues((DBRequest*)&listextra,prefix);
  
  fIsInit = true;
  return kOK;
}

//_____________________________________________________________________________
Int_t THcTestPaddles::DefineVariables( EMode mode )
{
  // Initialize global variables and lookup table for decoder
  cout << "THcTestPaddles::DefineVariables called " << GetName() << endl;
  if( mode == kDefine && fIsSetup ) return kOK;
  fIsSetup = ( mode == kDefine );

  // Register variables in global list

  RVarDef vars[] = {
    // Move these into THcHallCSpectrometer using track fTracks
    {"reftdc",            "Reference TDC Average",  "fRefAverage"},
    {"starttime",         "Hodoscope Start Time",   "fStartTime"},
    { 0 }
  };
  return DefineVarsFromList( vars, mode );
  //  return kOK;
}

//_____________________________________________________________________________
THcTestPaddles::~THcTestPaddles()
{
  // Destructor. Remove variables from global list.

  delete [] fFPTime;

  if( fIsSetup )
    RemoveVariables();
  if( fIsInit )
    DeleteArrays();
  if (fTrackProj) {
    fTrackProj->Clear();
    delete fTrackProj; fTrackProj = 0;
  }
}

//_____________________________________________________________________________
void THcTestPaddles::DeleteArrays()
{
  // Delete member arrays. Used by destructor.
  
}

//_____________________________________________________________________________
inline 
void THcTestPaddles::ClearEvent()
{

}

//_____________________________________________________________________________
Int_t THcTestPaddles::Decode( const THaEvData& evdata )
{
  ClearEvent();
  // Get the Hall C style hitlist (fRawHitList) for this event
  Int_t fNhits = DecodeToHitList(evdata);

  // Average all plane "0" tdcs to get the start time
  // For now, ignore all other scintillator hits
  Double_t refSum=0.0;
  Int_t refCount = 0;
  for(Int_t ihit=0;ihit<fNhits;ihit++) {
    THcRawHodoHit* hit = (THcRawHodoHit *) fRawHitList->At(ihit);
    if(hit->GetTDCPos() > 0) {
      refSum += hit->GetTDCPos();
      refCount++;
    } else if(hit->GetTDCNeg() > 0) {
      refSum += hit->GetTDCNeg();
      refCount++;
    }
  }
  if(refCount>0) {
    fRefAverage = refSum/refCount;
    fStartTime = fRefAverage*fRefNSPerChan + fRefOffset;
    fGoodStartTime = kTRUE;
  } else {
    fRefAverage = 0.0;
    fStartTime = 0.0;
    fGoodStartTime = kFALSE;
  }
  return refCount;
}

//_____________________________________________________________________________
Int_t THcTestPaddles::CoarseProcess( TClonesArray&  tracks  )
{

 
  return 0;
}

//_____________________________________________________________________________
Int_t THcTestPaddles::FineProcess( TClonesArray& tracks )
{

  return 0;
  
}
ClassImp(THcTestPaddles)
////////////////////////////////////////////////////////////////////////////////
