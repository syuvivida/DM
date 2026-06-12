#include <TH2.h>
#include <string>
#include <TLatex.h>
#include <TPaveText.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>

using namespace std;

void plot_tanb_mHmA()
{
  TCanvas* c1 = new TCanvas("c1","",700,500);
  gPad->SetRightMargin(0.15);
  
  unsigned int xLabel[]={230,250,300,325,350};
  unsigned int yLabel[]={470,500,550,600,650,700,750,800};

  const unsigned int nA=sizeof(xLabel)/sizeof(xLabel[0]);
  const unsigned int nH=sizeof(yLabel)/sizeof(yLabel[0]);

  cout << "nA = " << nA << "\t nH=" << nH << endl;
  
  TH2F* hxSec = new TH2F("hxSec","Type-I 2HDM+a H#rightarrow AA#rightarrow haha#rightarrow hh+4#chi @#sqrt{s}=13.6 TeV",nA,0.5,nA+.5,nH,0.5,nH+.5);
  hxSec->GetXaxis()->SetLabelSize(0.055);
  hxSec->GetYaxis()->SetLabelSize(0.055);
  hxSec->GetXaxis()->SetTitleSize(0.05);
  hxSec->GetYaxis()->SetTitleSize(0.05);
  hxSec->SetTitleSize(0.05);
  hxSec->SetTitleOffset(1,"Y");
  hxSec->SetTitleOffset(0.9,"X");
  hxSec->SetXTitle("M_{A} [GeV]");
  hxSec->SetYTitle("M_{H} [GeV]");
  TH2F* hCol= (TH2F*)hxSec->Clone("hCol");
  for(int i=0; i< nA; i++)
    hCol->GetXaxis()->SetBinLabel(i+1,std::to_string(xLabel[i]).data());
  for(int i=0; i< nH; i++)
    hCol->GetYaxis()->SetBinLabel(i+1,std::to_string(yLabel[i]).data());
  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat("1.2f");
  //  gStyle->SetPaintTextFormat(".2e");
  ifstream fin;
  fin.open("tanb.dat");
  for(int j=0; j<nH; j++)
    for(int i=0;i<nA;i++)
      {
	{
	  float x, y, xsec;
	  if(yLabel[j]<=2*xLabel[i])continue;
	  fin >> y >> x >> xsec;
	  hxSec->SetBinContent(i+1,j+1, xsec);
	  hCol->SetBinContent(i+1,j+1, xsec);
	}
    };
  hxSec->SetMarkerSize(1.5);
  hCol->Draw("colz");
  hxSec->Draw("TEXT SAME");
  TLatex *lar = new TLatex(0.25, 0.221053, "optimized tan#beta with constraint");
  lar->SetNDC(kTRUE);
  lar->SetTextSize(0.05);
  lar->Draw();
  TLatex *lar2 = new TLatex(0.454155, 0.157895, "M_{a}=100 GeV, M_{#chi}=10 GeV");
  lar2->SetNDC(kTRUE);
  lar2->SetTextSize(0.05);
  lar2->Draw();
  c1->Update();
  TPaveText *pt = (TPaveText*)(c1->GetPrimitive("title"));
  pt->Print();
  pt->SetTextSize(0.05); 
  c1->Modified(); 
  c1->Print("2HDMaI_explictHAAahah_besttanb_constraint.pdf");
  c1->Print("2HDMaI_explictHAAahah_besttanb_constraint.png");
}
