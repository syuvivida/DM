#include <TH2.h>
#include <string>
using namespace std;
#include <fstream>

void plot()
{
  TCanvas* c1 = new TCanvas("c1","",700,500);
  unsigned int xLabel[4]={230,250,300,350};
  unsigned int yLabel[8]={470,500,550,600,650,700,750,800};

  TH2F* hxSec = new TH2F("hxSec","Type-I 2HDM+a H#rightarrow AA#rightarrow haha#rightarrow hh+4#chi @ #sqrt{s}=13.6 TeV [pb]",4,0.5,4.5,8,0.5,8.5);
  hxSec->GetXaxis()->SetLabelSize(0.055);
  hxSec->GetYaxis()->SetLabelSize(0.055);
  hxSec->GetXaxis()->SetTitleSize(0.05);
  hxSec->GetYaxis()->SetTitleSize(0.05);
  hxSec->SetTitleOffset(1,"Y");
  hxSec->SetTitleOffset(0.9,"X");
  hxSec->SetXTitle("M_{A} [GeV]");
  hxSec->SetYTitle("M_{H} [GeV]");
  TH2F* hCol= (TH2F*)hxSec->Clone("hCol");
  for(int i=0; i< 4; i++)
    hCol->GetXaxis()->SetBinLabel(i+1,std::to_string(xLabel[i]).data());
  for(int i=0; i< 8; i++)
    hCol->GetYaxis()->SetBinLabel(i+1,std::to_string(yLabel[i]).data());
  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat("1.4f");
  ifstream fin;
  fin.open("xSec.dat");
  for(int j=0; j<8; j++)
    for(int i=0;i<4;i++)
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
  c1->Print("2HDMaI_explictHAAahah.pdf");
  c1->Print("2HDMaI_explictHAAahah.gif");
}
