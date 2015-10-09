#ifndef ROOT_THcTestPaddles
#define ROOT_THcTestPaddles

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// THcTestPaddles                                                              //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "TClonesArray.h"
#include "THaNonTrackingDetector.h"
#include "THcHodoscope.h"
#include "THcHitList.h"
#include "THcRawHodoHit.h"

#include "THaTrackingDetector.h"
#include "THcHitList.h"
#include "THcRawDCHit.h"
#include "THcSpacePoint.h"
#include "THcDriftChamberPlane.h"
#include "THcDriftChamber.h"
#include "TMath.h"

#include "THaSubDetector.h"
#include "TClonesArray.h"
#include <iostream>
#include <fstream>


class THaScCalib;

class THcTestPaddles : public THcHodoscope {

public:
  THcTestPaddles( const char* name, const char* description = "",
		   THaApparatus* a = NULL );
  virtual ~THcTestPaddles();

  virtual Int_t      Decode( const THaEvData& );
  virtual EStatus    Init( const TDatime& run_time );
  
  virtual Int_t      CoarseProcess( TClonesArray& tracks );
  virtual Int_t      FineProcess( TClonesArray& tracks );
  
  Int_t GetNfptimes() const {return fNfptimes;};

  const TClonesArray* GetTrackHits() const { return fTrackProj; }

  friend class THaScCalib;

  THcTestPaddles();  // for ROOT I/O
protected:

  Int_t fAnalyzePedestals;


  // Per-event data
  //  Bool_t fGoodStartTime;
  Double_t fRefAverage;
  //  Double_t fStartTime; 
  Int_t fNfptimes;

  // Paramters
  Int_t fNPaddles;		// Number of paddles
  Double_t fRefOffset;		// Offset to apply to reference time
  Double_t fRefNSPerChan;       // NS per count for reference time


  TClonesArray*  fTrackProj;  // projection of track onto scintillator plane
                              // and estimated match to TOF paddle

  Double_t*    fFPTime;               // [fNPlanes] Array 

  Double_t* fSumPlaneTime; // [fNPlanes]

  Int_t* fNScinHits;  // [fNPlanes]
  Int_t* fNPlaneTime; // [fNPlanes]

  Bool_t* fGoodPlaneTime;  // [fNPlanes]

  void           ClearEvent();
  void           DeleteArrays();
  virtual Int_t  ReadDatabase( const TDatime& date );
  virtual Int_t  DefineVariables( EMode mode = kDefine );
  enum ESide { kLeft = 0, kRight = 1 };
  
  void Setup(const char* name, const char* description);

  ClassDef(THcTestPaddles,0)   // Hodoscope detector
};

////////////////////////////////////////////////////////////////////////////////

#endif
