#include <TH2.h>
#include <string>
using namespace std;
#include <fstream>

void plot()
{
  TCanvas* c1 = new TCanvas("c1","",700,500);
  unsigned int xLabel[12]={90, 100, 150, 200, 300, 400, 500, 600, 700, 800, 900, 1000};
  unsigned int yLabel[13]={1, 10, 50, 100, 200, 300, 400, 500, 600, 700, 800, 900,1000};

  TH2F* hxSec = new TH2F("hxSec","Z'-Baryonic cross sections @ #sqrt{s}=13.6 TeV [pb]",12,0.5,12.5,13,0.5,13.5);
  hxSec->GetXaxis()->SetLabelSize(0.055);
  hxSec->GetYaxis()->SetLabelSize(0.055);
  hxSec->GetXaxis()->SetTitleSize(0.05);
  hxSec->GetYaxis()->SetTitleSize(0.05);
  hxSec->SetTitleOffset(1,"Y");
  hxSec->SetTitleOffset(0.9,"X");
  hxSec->SetXTitle("M_{Z'} [GeV]");
  hxSec->SetYTitle("M_{DM} [GeV]");
  TH2F* hCol= (TH2F*)hxSec->Clone("hCol");
  for(int i=0; i< 12; i++)
    hCol->GetXaxis()->SetBinLabel(i+1,std::to_string(xLabel[i]).data());
  for(int i=0; i< 13; i++)
    hCol->GetYaxis()->SetBinLabel(i+1,std::to_string(yLabel[i]).data());
  gStyle->SetOptStat(0);
  ifstream fin;
  fin.open("xSec_ZpBaryonic_explicit.dat");
  for(int i=0;i<12;i++)
    {
      for(int j=0; j<13; j++)
	{
	  float x, y, xsec;
	  if(yLabel[j]>2*xLabel[i])continue;
	  fin >> x >> y >> xsec;
	  hxSec->SetBinContent(i+1,j+1, xsec);
	  hCol->SetBinContent(i+1,j+1, xsec);
	}
    };
  hCol->Draw("colz");
  hxSec->Draw("TEXT SAME");
  c1->Print("ZpBaryonic_explictbb.pdf");
  c1->Print("ZpBaryonic_explictbb.gif");
}
