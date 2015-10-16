// Histogram all 224 shower counter channels

{
  TFile* f= new TFile("shana.root");
  TTree *T=(TTree*)f->Get("T");
  TCanvas *c=new TCanvas("c","SHMS Shower Counter", 1370, 1200);

  Int_t NROWS=16;
  Int_t NCOLUMNS=14;

  c->Divide(NCOLUMNS,NROWS);
  
  for(Int_t ic=0;ic<NCOLUMNS;ic++) {
    for(Int_t ir=0;ir<NROWS;ir++) {
      Int_t i = ic*NROWS+ir;
      c->cd(i+1);
      T->Draw(Form("S.cal.fly.a[%d]",i));
      TH1F *htemp = (TH1F*)gPad->GetPrimitive("htemp");
      // cout << i << " " << ic << "-" << ir << " " << htemp->GetMean() << " " << htemp->GetRMS() << endl;
    }
  }
}
