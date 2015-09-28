{

  //
  //  Steering script to test drift chambers on the bench
  //

  gSystem->Load("src/libTestStand.so");
  
  Int_t RunNumber=25;
  char* RunFileNamePattern="raw/shms1190_%05d.dat";
  
  gHcParms->Define("gen_run_number", "Run Number", RunNumber);

  // Constants not in ENGINE PARAM files that we want to be
  // configurable
  gHcParms->Load("PARAM/dcana.param");

  // Load the Hall C style detector map
  gHcDetectorMap=new THcDetectorMap();
  gHcDetectorMap->Load("MAPS/shms_dc_test_15.map");

  // Set up the equipment to be analyzed.

  THaApparatus* SHMS = new THcHallCSpectrometer("S","SHMS");
  gHaApps->Add( SHMS );

  SHMS->AddDetector( new THcDC("dc", "Drift Chambers" ));
  SHMS->AddDetector( new THcTestPaddles("hod", "Scintillator Trigger"));

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
  analyzer->SetOutFile( "dcana.root" );
  analyzer->SetOdefFile("dcana.def");
  analyzer->SetCutFile("dcana_cuts.def");        // optional
  analyzer->SetCountMode(2);// Counter event number same as gen_event_ID_number
  
  // File to record cuts accounting information
  //  analyzer->SetSummaryFile("summary_example.log"); // optional
  
  analyzer->Process(run);     // start the actual analysis
  //analyzer->PrintReport("dcana.template","dcana.out");
}
