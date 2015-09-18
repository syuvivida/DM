#include <vector>
#include <string>
#include <TLegend.h>
#include <TLatex.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TSystem.h>
using namespace std;

void plotAllEff(string inputFile, string fatname, string thinname, string title, 
		vector<string> cuts,
		string outputprefix,
		const float ymax=1.35,
		const float x1NDC = 0.280,
		const float y1NDC = 0.517,
		const float x2NDC = 0.797,
		const float y2NDC = 0.887)
{

  bool JETMET=false;
  if(inputFile.find("JETMET")!= std::string::npos)
    JETMET=true;


  TFile* f;
  TFile *inf = new TFile(inputFile.data());
  TEfficiency* fateff  =(TEfficiency*)inf->FindObjectAny(fatname.data());
  TEfficiency* thineff=(TEfficiency*)inf->FindObjectAny(thinname.data());

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
  fateff->SetTitle(title.data());

  TLatex *lar = new TLatex(0.15, 0.85, "CMS");
  lar->SetNDC(kTRUE); 
  lar->SetTextSize(0.050);
  lar->SetTextFont(62);
  lar->SetLineWidth(5);


  TLatex *lar2 = new TLatex(0.15, 0.80, "Simulation");
  lar2->SetNDC(kTRUE); 
  lar2->SetTextSize(0.050);
  lar2->SetTextFont(52);
  lar2->SetLineWidth(5);


  TLegend* leg = new TLegend(x1NDC,y1NDC,x2NDC,y2NDC);
  leg->SetTextAlign(32);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  for(unsigned int i=0; i<cuts.size(); i++)
    leg->AddEntry((TObject*)0, cuts[i].data(),"");

  leg->AddEntry(fateff, "single Anti-Kt R=0.8 jet","f");
  if(JETMET)
    leg->AddEntry((TObject*)0,"#Delta R(H,jet)<0.1",""); 
  else
    leg->AddEntry((TObject*)0,"#Delta R(b,jet)<0.8",""); 
  // leg->AddEntry((TObject*)0, "","");
  leg->AddEntry(thineff, "two Anti-Kt R=0.4 jets","f");
  if(JETMET)
    leg->AddEntry((TObject*)0,"#Delta R(b,jet)<0.1",""); 
  else
    leg->AddEntry((TObject*)0,"#Delta R(b,jet)<0.4",""); 

  TCanvas* c1 = new TCanvas("c1","",500,500);

  c1->SetGridx(1);
  c1->SetGridy(1);
  
  fateff->Draw("ae2"); 
  gPad->Update();

  fateff->GetPaintedGraph()->SetMaximum(ymax);
  fateff->GetPaintedGraph()->SetMinimum(0);
  fateff->Draw("ae2"); 
  thineff->Draw("e2Same");  
  leg->Draw("same");
  lar->Draw("same");
  lar2->Draw("same");

  gSystem->mkdir("jeteff");

  std::string outputname = "jeteff/" + outputprefix;
  if(JETMET)
    outputname += "_JEMET";
  else
    outputname += "_myDefinition";
  c1->Print(Form("%s.pdf",outputname.data()));
  c1->Print(Form("%s.eps",outputname.data()));
  c1->Print(Form("%s.C",outputname.data()));

}
