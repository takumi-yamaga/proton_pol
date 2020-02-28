// Analysis.C

#include"DrawTools.h"

TFile* input_file = new TFile("test.root");
TFile* output_file = nullptr;

TString pdf_name = "fig/analysis.pdf";

void CreateHistograms();
void DrawHistograms();

void Analysis(){
  gROOT->SetBatch(true);


  // initialization ---------------------------------------
  TTree* tree = (TTree*)input_file->Get("EventTree");     
  Float_t momentum_x, momentum_y, momentum_z;
  tree->SetBranchAddress("dcout_momentum_x",&momentum_x);
  tree->SetBranchAddress("dcout_momentum_y",&momentum_y);
  tree->SetBranchAddress("dcout_momentum_z",&momentum_z);

  output_file = new TFile("output.root","RECREATE");
  CreateHistograms();
  // ------------------------------------------------------


  // analysis ---------------------------------------------
  Long64_t total_entries = tree->GetEntries();
  for(Long64_t i_entry=0; i_entry<total_entries; ++i_entry){
    if((i_entry+1)%(Int_t)pow(10, (Int_t)log10(i_entry+1)) ==0 ){
      std::cout << ">> " << i_entry+1 << std::endl;
    }
    tree->GetEntry(i_entry);

    Float_t momentum = sqrt(momentum_x*momentum_x + momentum_y*momentum_y + momentum_z*momentum_z);
    Float_t theta = atan2(sqrt(momentum_x*momentum_x + momentum_y*momentum_y),momentum_z) * (180./TMath::Pi());
    Float_t phi = atan2(momentum_x,momentum_y) * (180./TMath::Pi());

    ((TH1F*)output_file->Get("momentum"))->Fill(momentum);
    ((TH1F*)output_file->Get("theta"))->Fill(theta);
    ((TH1F*)output_file->Get("phi"))->Fill(phi);
    ((TH2F*)output_file->Get("momentum_vs_theta"))->Fill(momentum,theta);

    if(20.<theta&&theta<60.){
      ((TH1F*)output_file->Get("momentum_selected"))->Fill(momentum);
      ((TH1F*)output_file->Get("theta_selected"))->Fill(theta);
      ((TH1F*)output_file->Get("phi_selected"))->Fill(phi);
    }

  }
  // ------------------------------------------------------

  DrawHistograms();

  output_file->Write();
  output_file->Close();
}

void CreateHistograms(){
  output_file->cd();
  
  // 1D histograms
  new TH1F("momentum","momentum;#font[12]{p} (MeV/#font[12]{c});counts",200,0.,200.);
  new TH1F("theta","theta;#theta (deg.);counts",180,0.,180.);
  new TH1F("phi","phi;#phi (deg.);counts",180,-180.,180.);
  new TH1F("momentum_selected","momentum;#font[12]{p} (MeV/#font[12]{c});counts",200,0.,200.);
  new TH1F("theta_selected","theta;#theta (deg.);counts",180,0.,180.);
  new TH1F("phi_selected","phi;#phi (deg.);counts",180,-180.,180.);

  // 2D histograms
  new TH2F("momentum_vs_theta","momentum vs. theta;#font[12]{p} (MeV/#font[12]{c});#theta (deg.)"
      ,200,0.,200.,180,0.,180.);
}

void DrawHistograms(){
  TCanvas* canvas = new TCanvas("canvas","canvas",500,500);
  canvas->cd();

  // open pdf file
  canvas->Print(pdf_name + "[");

  TH1F* hist_1d = nullptr;
  TH2F* hist_2d = nullptr;

  // --------------
  gPad->SetLogy();
  // --------------

  // momentum
  hist_1d = (TH1F*)output_file->Get("momentum");
  Draw(hist_1d);
  canvas->Print(pdf_name);

  // theta
  hist_1d = (TH1F*)output_file->Get("theta");
  Draw(hist_1d);
  canvas->Print(pdf_name);

  // --------------
  gPad->SetLogy(0);
  // --------------
  
  // phi
  hist_1d = (TH1F*)output_file->Get("phi_selected");
  Draw(hist_1d);
  hist_1d->SetMinimum(0);
  canvas->Print(pdf_name);

  // --------------
  gPad->SetLogy(0);
  gPad->SetLogz();
  // --------------

  // momentum vs. theta
  hist_2d = (TH2F*)output_file->Get("momentum_vs_theta");
  Draw(hist_2d);
  canvas->Print(pdf_name);

  // close pdf file
  canvas->Print(pdf_name + "]");
}
