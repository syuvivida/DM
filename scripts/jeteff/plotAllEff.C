#include <vector>
#include <string>
#include <TLegend.h>
#include <TLatex.h>
#include <TEfficiency.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TSystem.h>
#include <TFile.h>
#include <TGraphAsymmErrors.h>
#include <TPaveText.h>

using namespace std;

void plotAllEff(string inputFile, vector<string> effnames, string title, 
		vector<vector<string>> legends,
		vector<string> cuts,
		string outputprefix,
		const float ymax=1.35,
		const float x1NDCt = 0.381,
		const float y1NDCt = 0.526,
		const float x2NDCt = 0.899,
		const float y2NDCt = 0.895,
		const float x1NDC  = 0.381,
		const float y1NDC  = 0.526,
		const float x2NDC  = 0.899,
		const float y2NDC  = 0.895)
{



  TFile *inf = new TFile(inputFile.data());
  const unsigned int nfiles = effnames.size();
  TEfficiency* h[nfiles];

  int color[5]={1, kRed, kBlue, kGreen+2, kMagenta};

  for(unsigned int i=0;i<nfiles;i++)
    {
      h[i]  =(TEfficiency*)inf->FindObjectAny(effnames[i].data());
      h[i]->SetFillStyle(3144);
      h[i]->SetMarkerColor(color[i%5]);
      h[i]->SetMarkerStyle(20+i);
      h[i]->SetMarkerSize(0.5);
      h[i]->SetLineColor(color[i%5]);
      h[i]->SetFillColor(color[i%5]);
      h[i]->SetTitle(title.data());

    }


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


  TPaveText *pavetex = new TPaveText(x1NDCt, 
				     y1NDCt, 
				     x2NDCt, 
				     y2NDCt,"NDCBR");
  pavetex->SetBorderSize(0);
  pavetex->SetFillColor(0);
  pavetex->SetFillStyle(0);
  pavetex->SetLineWidth(3);
  pavetex->SetTextAlign(12);
  pavetex->SetTextSize(0.04);
  for(unsigned int i=0; i<cuts.size(); i++)
    pavetex->AddText(cuts[i].data());

  

  TLegend* leg = new TLegend(x1NDC,y1NDC,x2NDC,y2NDC);
  leg->SetTextAlign(12);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);

  for(unsigned int i=0; i<nfiles; i++){

    leg->AddEntry(h[i], legends[i][0].data(),"f");

    for(unsigned int j=1;j<legends[i].size();j++){
      leg->AddEntry((TObject*)0,legends[i][j].data(),""); 
    }
  }

  TCanvas* c1 = new TCanvas("c1","",500,500);

  c1->SetGridx(1);
  c1->SetGridy(1);
  
  h[0]->Draw("ae2"); 
  gPad->Update();

  h[0]->GetPaintedGraph()->SetMaximum(ymax);
  h[0]->GetPaintedGraph()->SetMinimum(0);
  h[0]->Draw("ae2"); 

  for(unsigned int i=1; i<nfiles;i++)
    h[i]->Draw("e2Same");  
  leg->Draw("same");
  lar->Draw("same");
  lar2->Draw("same");
  pavetex->Draw("same");

  gSystem->mkdir("jeteff");

  std::string outputname = "jeteff/" + outputprefix;
  c1->Print(Form("%s.pdf",outputname.data()));
  c1->Print(Form("%s.eps",outputname.data()));
  c1->Print(Form("%s.C",outputname.data()));

}
