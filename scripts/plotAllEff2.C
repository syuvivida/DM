#include <vector>
#include <string>
#include <TEfficiency.h>
#include <TCanvas.h>
using namespace std;

void plotAllEff2()
{

  TFile* f;
  TFile *inf = new TFile("eff_dr0.1.root");
  TEfficiency* fateff  =(TEfficiency*)inf->FindObjectAny("fatJetEff");
  TEfficiency* thineff=(TEfficiency*)inf->FindObjectAny("thinJetEff");

  fateff->SetLineColor(1);
  fateff->SetFillColor(1);
  fateff->SetFillStyle(3144);
  fateff->SetMarkerColor(1);
  fateff->SetMarkerStyle(8);
  fateff->SetMarkerSize(0.5);

  thineff->SetLineColor(2);
  thineff->SetFillColor(2);
  thineff->SetFillStyle(3144);
  thineff->SetMarkerColor(2);
  thineff->SetMarkerStyle(8);
  thineff->SetMarkerSize(0.5);
  fateff->SetTitle(";Generator-level Higgs p_{T} [GeV];Efficiency;");

  fateff->Draw("e2");
  thineff->Draw("e2Same");

  float x1NDC = 0.503;
  float y1NDC = 0.359;
  float x2NDC = 0.706;
  float y2NDC = 0.684;

  TLegend* leg = new TLegend(x1NDC,y1NDC,x2NDC,y2NDC);
  
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  leg->AddEntry(fateff, "merged jet efficiency","f");
  leg->AddEntry((TObject*)0, "single Anti-Kt R=0.8 jet","");
  leg->AddEntry((TObject*)0, "pt > 30 GeV, |#eta|<2.5","");
  // leg->AddEntry((TObject*)0, "#Delta R(b/#bar{b},jet)<0.8","");
  leg->AddEntry((TObject*)0, "#Delta R(H,jet)<0.1","");
  leg->AddEntry((TObject*)0, "","");
  leg->AddEntry(thineff, "resolved jet efficiency","f");
  leg->AddEntry((TObject*)0, "two Anti-Kt R=0.4 jets","");
  leg->AddEntry((TObject*)0, "pt > 30 GeV, |#eta|<2.5","");
  // leg->AddEntry((TObject*)0, "#Delta R(b,jet)<0.4","");
  leg->AddEntry((TObject*)0, "#Delta R(b,jet)<0.1","");
  leg->Draw("same");

  // c1->SetGridx(1);
  // c1->SetGridy(1);

  // c1->Print("effall.pdf");
  // c1->Print("effall.eps");
  // c1->Print("effall.C");

}
