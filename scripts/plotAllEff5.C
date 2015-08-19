#include <vector>
#include <string>
#include <TEfficiency.h>
#include <TCanvas.h>
using namespace std;

void plotAllEff5(string inputFile)
{
  bool JETMET=false;
  if(inputFile.find("JETMET")!= std::string::npos)
    JETMET=true;

  TFile* f;
  TFile *inf = new TFile(inputFile.data());
  TEfficiency* fateff  =(TEfficiency*)inf->FindObjectAny("noJetEff");

  fateff->SetLineColor(1);
  fateff->SetFillColor(1);
  fateff->SetFillStyle(3144);
  fateff->SetMarkerColor(1);
  fateff->SetMarkerStyle(8);
  fateff->SetMarkerSize(0.5);

  fateff->SetTitle(";Generator-level Higgs p_{T} [GeV];Efficiency;");

  float x1NDC = 0.403;
  float y1NDC = 0.459;
  float x2NDC = 0.606;
  float y2NDC = 0.684;

  TLegend* leg = new TLegend(x1NDC,y1NDC,x2NDC,y2NDC);
  
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  leg->AddEntry(fateff, "No-matched-jet efficiency","f");

  TCanvas* c1 = new TCanvas("c1","",500,500);

  c1->SetGridx(1);
  c1->SetGridy(1);

  fateff->Draw("ae2");
  leg->Draw("same");

  std::string outputname = JETMET? "jeteff/effno_smallMatchDeltaR": "jeteff/effno_bigMatchDeltaR";
  c1->Print(Form("%s.pdf",outputname.data()));
  c1->Print(Form("%s.eps",outputname.data()));
  c1->Print(Form("%s.C",outputname.data()));

}
