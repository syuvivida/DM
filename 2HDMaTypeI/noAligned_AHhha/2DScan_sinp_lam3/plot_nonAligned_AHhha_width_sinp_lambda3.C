#include <TH2.h>
#include <string>
#include <TLatex.h>
#include <TPaveText.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

void plot_nonAligned_AHhha_width_sinp_lambda3()
{
  TCanvas* c1 = new TCanvas("c1","",700,500);
  gPad->SetRightMargin(0.15);
  
  unsigned int xLabel[]={2,3,4,6,8};
  float yLabel[]={0.0, 0.1,0.2,0.3,0.5,0.71};

  const unsigned int nA=sizeof(xLabel)/sizeof(xLabel[0]);
  const unsigned int nH=sizeof(yLabel)/sizeof(yLabel[0]);

  cout << "nA = " << nA << "\t nH=" << nH << endl;
  
  TH2F* hxSec = new TH2F("hxSec","Type-I 2HDM+a: A#rightarrow Ha#rightarrow hh+2#chi @ #sqrt{s}=13.6 TeV, #Gamma_{H}/M_{H}",nA,0.5,nA+.5,nH,0.5,nH+.5);
  hxSec->GetXaxis()->SetLabelSize(0.055);
  hxSec->GetYaxis()->SetLabelSize(0.055);
  hxSec->GetXaxis()->SetTitleSize(0.05);
  hxSec->GetYaxis()->SetTitleSize(0.05);
  hxSec->SetTitleSize(0.05);
  hxSec->SetTitleOffset(1,"Y");
  hxSec->SetTitleOffset(0.9,"X");
  hxSec->SetXTitle("#lambda_{3}");
  hxSec->SetYTitle("sin#theta");
  TH2F* hCol= (TH2F*)hxSec->Clone("hCol");
  for(int i=0; i< nA; i++)
    hCol->GetXaxis()->SetBinLabel(i+1,std::to_string(xLabel[i]).data());
  for(int i=0; i< nH; i++)
    {
      std::ostringstream out;
      out << std::fixed << std::setprecision(2) << yLabel[i];
      std::string s = out.str();
      hCol->GetYaxis()->SetBinLabel(i+1,s.data());
    }
  gStyle->SetOptStat(0);
  //gStyle->SetPaintTextFormat("1.4f");
  gStyle->SetPaintTextFormat(".2e");
  ifstream fin;
  fin.open("width.dat");
  for(int j=1; j<nH; j++)
    for(int i=0;i<nA;i++)
      {
	{
	  float x, y, xsec;
	  fin >> y >> x >> xsec;
	  hxSec->SetBinContent(i+1,j+1, xsec);
	  hCol->SetBinContent(i+1,j+1, xsec);
	}
    };
  hxSec->SetMarkerSize(1.5);
  hCol->Draw("colz");
  hxSec->Draw("TEXT SAME");
  TLatex *lar = new TLatex(0.12, 0.17, "cos(#beta-#alpha)=0.1, tan#beta=2, M_{A}=410 GeV, M_{H}=260 GeV, M_{a}=100 GeV, M_{#chi}=10 GeV");
  lar->SetNDC(kTRUE);
  lar->SetTextSize(0.035);
  lar->Draw();
  c1->Update();
  TPaveText *pt = (TPaveText*)(c1->GetPrimitive("title"));
  pt->Print();
  pt->SetTextSize(0.05); 
  c1->Modified(); 
  c1->Print("2HDMaI_width_noAligned_tanbeta5_mA410_mH260.pdf");
  c1->Print("2HDMaI_width_noAligned_tanbeta5_mA410_mH260.gif");
}
