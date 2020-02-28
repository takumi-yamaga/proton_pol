// DrawTools.h


// ========================================================
// ========================================================
// ========================================================


static const Double_t kLeftMargin   = 0.10;
static const Double_t kBottomMargin = 0.10;
static const Double_t kRightMargin  = 0.02;
static const Double_t kTopMargin    = 0.02;

static const Double_t kLeftMargin_2D   = 0.10;
static const Double_t kBottomMargin_2D = 0.10;
static const Double_t kRightMargin_2D  = 0.02;
static const Double_t kTopMargin_2D    = 0.02;


// ========================================================
// ========================================================
// ========================================================


void Draw(TH1F*,TString);
void Draw(TH2F*,TString);


// ========================================================
// ========================================================
// ========================================================

void Draw(TH1F* hist, TString option="hist"){

  // canvas setting
  gPad->SetTicks();
  gPad->SetGrid();
  gPad->SetLeftMargin(kLeftMargin);
  gPad->SetBottomMargin(kBottomMargin);
  gPad->SetRightMargin(kRightMargin);
  gPad->SetTopMargin(kTopMargin);

  // histogram setting
  gStyle->SetOptTitle(false);
  gStyle->SetOptStat(111110);
  gStyle->SetEndErrorSize(0);
  hist->SetLineColor(kBlack);
  hist->SetLineWidth(2);
  hist->SetMarkerColor(kBlack);

  // axis setting
  hist->GetXaxis()->SetNdivisions(505);
  hist->GetYaxis()->SetNdivisions(505);
  hist->GetXaxis()->SetTitleFont(132);
  hist->GetYaxis()->SetTitleFont(132);
  hist->GetXaxis()->SetTitleSize(0.04);
  hist->GetYaxis()->SetTitleSize(0.04);
  hist->GetXaxis()->SetLabelFont(132);
  hist->GetYaxis()->SetLabelFont(132);
  hist->GetXaxis()->SetLabelSize(0.03);
  hist->GetYaxis()->SetLabelSize(0.03);
  hist->GetXaxis()->CenterTitle();
  hist->GetYaxis()->CenterTitle();

  // drawing
  hist->Draw(option);
  gPad->Update();

  // statistics bos setting (MUST be after "Draw()")
  TPaveStats* stat = (TPaveStats*)hist->FindObject("stats");
  stat->SetX1NDC(1.-kRightMargin-0.2);
  stat->SetX2NDC(1.-kRightMargin);
  stat->SetY1NDC(1.-kTopMargin-0.2);
  stat->SetY2NDC(1.-kTopMargin);
  stat->SetFillColorAlpha(kGray,0.3);
  stat->SetFillStyle(1001);
  stat->SetTextFont(132);

  // redraw axis
  gPad->RedrawAxis();
}


// ========================================================
// ========================================================
//

void Draw(TH2F* hist, TString option="colz"){

  // canvas setting
  gPad->SetTicks();
  gPad->SetGrid();
  gPad->SetLeftMargin(kLeftMargin_2D);
  gPad->SetBottomMargin(kBottomMargin_2D);
  gPad->SetRightMargin(kRightMargin_2D);
  gPad->SetTopMargin(kTopMargin_2D);

  // histogram setting
  gStyle->SetOptTitle(false);
  gStyle->SetOptStat(111110);
  gStyle->SetEndErrorSize(0);
  hist->SetLineColor(kBlack);
  hist->SetLineWidth(2);
  hist->SetMarkerColor(kBlack);

  // axis setting
  hist->GetXaxis()->SetNdivisions(505);
  hist->GetYaxis()->SetNdivisions(505);
  hist->GetXaxis()->SetTitleFont(132);
  hist->GetYaxis()->SetTitleFont(132);
  hist->GetXaxis()->SetTitleSize(0.04);
  hist->GetYaxis()->SetTitleSize(0.04);
  hist->GetXaxis()->SetLabelFont(132);
  hist->GetYaxis()->SetLabelFont(132);
  hist->GetXaxis()->SetLabelSize(0.03);
  hist->GetYaxis()->SetLabelSize(0.03);
  hist->GetXaxis()->CenterTitle();
  hist->GetYaxis()->CenterTitle();

  // drawing
  hist->Draw(option);
  gPad->Update();

  // statistics bos setting (MUST be after "Draw()")
  TPaveStats* stat = (TPaveStats*)hist->FindObject("stats");
  stat->SetX1NDC(1.-kRightMargin-0.2);
  stat->SetX2NDC(1.-kRightMargin);
  stat->SetY1NDC(1.-kTopMargin-0.2);
  stat->SetY2NDC(1.-kTopMargin);
  stat->SetFillColorAlpha(kGray,0.3);
  stat->SetFillStyle(1001);
  stat->SetTextFont(132);

  // redraw axis
  gPad->RedrawAxis();
}

