#include <TH2.h>
#include <string>
#include <TLatex.h>
#include <TPaveText.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>

using namespace std;

void plot_nonAligned_AHhha_xSec2D_mHmA()
{
  TCanvas* c1 = new TCanvas("c1","",700,500);
  gPad->SetRightMargin(0.15);

  unsigned int xLabel[]={260, 300, 350, 400, 450, 500, 550};   
  unsigned int yLabel[]={410, 460, 510, 560, 610, 660, 700, 750, 800};

  const unsigned int nH=sizeof(xLabel)/sizeof(xLabel[0]);
  const unsigned int nA=sizeof(yLabel)/sizeof(yLabel[0]);

  cout << "nA = " << nA << "\t nH=" << nH << endl;
  
  TH2F* hxSec = new TH2F("hxSec","Type-I 2HDM+a A#rightarrow aH#rightarrow hh+2#chi @ #sqrt{s}=13.6 TeV [pb]",nH,0.5,nH+.5,nA,0.5,nA+.5);
  hxSec->GetXaxis()->SetLabelSize(0.055);
  hxSec->GetYaxis()->SetLabelSize(0.055);
  hxSec->GetXaxis()->SetTitleSize(0.05);
  hxSec->GetYaxis()->SetTitleSize(0.05);
  hxSec->SetTitleSize(0.05);
  hxSec->SetTitleOffset(1,"Y");
  hxSec->SetTitleOffset(0.9,"X");
  hxSec->SetXTitle("M_{H} [GeV]");
  hxSec->SetYTitle("M_{A} [GeV]");
  TH2F* hCol= (TH2F*)hxSec->Clone("hCol");
  for(int i=0; i< nH; i++)
    hCol->GetXaxis()->SetBinLabel(i+1,std::to_string(xLabel[i]).data());
  for(int i=0; i< nA; i++)
    hCol->GetYaxis()->SetBinLabel(i+1,std::to_string(yLabel[i]).data());
  gStyle->SetOptStat(0);
  //gStyle->SetPaintTextFormat("1.4f");
  gStyle->SetPaintTextFormat(".2e");
  ifstream fin;
  fin.open("xSec.dat");
  for(int j=0; j<nA; j++)
    for(int i=0;i<nH;i++)
      {
	{
	  float x, y, xsec;
	  if(yLabel[j]<(xLabel[i]+100))continue;
	  fin >> y >> x >> xsec;
	  hxSec->SetBinContent(i+1,j+1, xsec);
	  hCol->SetBinContent(i+1,j+1, xsec);
	}
    };
  hxSec->SetMarkerSize(1.5);
  hCol->Draw("colz");
  hxSec->Draw("TEXT SAME");
  TLatex* lar0= new TLatex(0.601719,0.28,"cos(#alpha-#beta)=0.1");
  lar0->SetNDC(kTRUE);
  lar0->SetTextSize(0.05);
  lar0->Draw();
  
  TLatex *lar = new TLatex(0.475645, 0.214737, "tan#beta=2, #lambda_{3}=2, sin#theta=0.1");
  lar->SetNDC(kTRUE);
  lar->SetTextSize(0.05);
  lar->Draw();
  TLatex *lar2 = new TLatex(0.454155, 0.151579, "M_{a}=100 GeV, M_{#chi}=10 GeV");
  lar2->SetNDC(kTRUE);
  lar2->SetTextSize(0.05);
  lar2->Draw();
  c1->Update();
  TPaveText *pt = (TPaveText*)(c1->GetPrimitive("title"));
  pt->Print();
  pt->SetTextSize(0.05); 
  c1->Modified(); 
  c1->Print("2HDMaI_nonAligned_AHhha_cosbma0p1_tanbeta2_lambda3_2_sint0p1.pdf");
  c1->Print("2HDMaI_nonAligned_AHhha_cosbma0p1_tanbeta2_lambda3_2_sint0p1.gif");
}

