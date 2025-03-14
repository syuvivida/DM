#include <TH2.h>
#include <string>
using namespace std;
#include <fstream>

void plot()
{
  TCanvas* c1 = new TCanvas("c1","",700,500);
  unsigned int xLabel[8]={90,100,150,200,250,300,350,400};
  unsigned int yLabel[9]={100, 150, 200, 250, 300, 350, 400, 450, 500};

  TH2F* hxSec = new TH2F("hxSec","Type-I 2HDM+a cross sections @ #sqrt{s}=13.6 TeV [pb]",8,0.5,8.5,9,0.5,9.5);
  hxSec->GetXaxis()->SetLabelSize(0.055);
  hxSec->GetYaxis()->SetLabelSize(0.055);
  hxSec->GetXaxis()->SetTitleSize(0.05);
  hxSec->GetYaxis()->SetTitleSize(0.05);
  hxSec->SetTitleOffset(1,"Y");
  hxSec->SetTitleOffset(0.9,"X");
  hxSec->SetXTitle("M_{a} [GeV]");
  hxSec->SetYTitle("M_{DM} [GeV]");
  TH2F* hCol= (TH2F*)hxSec->Clone("hCol");
  for(int i=0; i< 8; i++)
    hCol->GetXaxis()->SetBinLabel(i+1,std::to_string(xLabel[i]).data());
  for(int i=0; i< 9; i++)
    hCol->GetYaxis()->SetBinLabel(i+1,std::to_string(yLabel[i]).data());
  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat("1.5f");
  ifstream fin;
  fin.open("xSec.dat");
  for(int i=0;i<8;i++)
    {
      for(int j=0; j<9; j++)
	{
	  float x, y, xsec;
	  if(yLabel[j]<=0.5*xLabel[i])continue;
	  if(yLabel[j]>=2*xLabel[i])continue;
	  fin >> x >> y >> xsec;
	  hxSec->SetBinContent(i+1,j+1, xsec);
	  hCol->SetBinContent(i+1,j+1, xsec);
	}
    };
  hxSec->SetMarkerSize(1.5);
  hCol->Draw("colz");
  hxSec->Draw("TEXT SAME");
  c1->Print("2HDMaI_explictbb.pdf");
  c1->Print("2HDMaI_explictbb.gif");
}
