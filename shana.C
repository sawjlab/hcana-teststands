{

  //
  //  Steering script to test drift chambers on the bench
  //

  gSystem->Load("src/libShowerTest.so");
  
  Int_t RunNumber=25;
  char* RunFileNamePattern="raw/shms_cal_%05d.dat";
  
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);

  // Constants not in ENGINE PARAM files that we want to be
  // configurable
  gHcParms->Load("PARAM/shana.param");

  // Load the Hall C style detector map
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load("MAPS/shana.map");

  // Set up the equipment to be analyzed.

  THaApparatus* SHMS = new THcHallCSpectrometer("S","SHMS");
  gHaApps->Add( SHMS );

  SHMS->AddDetector( new THcShower("cal", "Calorimeter" ));
  //  SHMS->AddDetector( new THcTestPaddles("hod", "Scintillator Trigger"));

  THcAnalyzer* analyzer = new THcAnalyzer;

  THaEvent* event = new THaEvent;
  
  // Define the run(s) that we want to analyze.
  // We just set up one, but this could be many.
  char RunFileName[100];
  sprintf(RunFileName,RunFileNamePattern,RunNumber);
  THaRun* run = new THaRun(RunFileName);
  run->SetNscan(1);
  run->SetDataRequired(0x7);
  run->Print();

  run->SetEventRange(1,10000);//  Physics Event number, does not
                                // include scaler or control events

  // Define the analysis parameters
  analyzer->SetEvent( event );
  analyzer->SetOutFile( "shana.root" );
  analyzer->SetOdefFile("shana.def");
  analyzer->SetCutFile("shana_cuts.def");        // optional
  analyzer->SetCountMode(2);// Counter event number same as gen_event_ID_number
  
  // File to record cuts accounting information
  //  analyzer->SetSummaryFile("summary_example.log"); // optional
  
  analyzer->Process(run);     // start the actual analysis
  //analyzer->PrintReport("shana.template","shana.out");
}
