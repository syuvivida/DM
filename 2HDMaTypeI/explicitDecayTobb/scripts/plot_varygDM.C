#include <TH2.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
using namespace std;


void plot_varygDM()
{
  TCanvas* c1 = new TCanvas("c1","",700,500);
  float xLabel[7]={0.001,0.0025,0.005,0.01,0.025,0.05,0.1};
  unsigned int yLabel[3]={10,50,100};

  TH2F* hxSec = new TH2F("hxSec","Type-I 2HDM+a cross sections @ #sqrt{s}=13.6 TeV [pb]",7,0.5,7.5,3,0.5,3.5);
  hxSec->GetXaxis()->SetLabelSize(0.055);
  hxSec->GetYaxis()->SetLabelSize(0.055);
  hxSec->GetXaxis()->SetTitleSize(0.05);
  hxSec->GetYaxis()->SetTitleSize(0.05);
  hxSec->SetTitleOffset(1,"Y");
  hxSec->SetTitleOffset(0.9,"X");
  hxSec->SetXTitle("y_{#chi}");
  hxSec->SetYTitle("M_{DM} [GeV]");
  TH2F* hCol= (TH2F*)hxSec->Clone("hCol");
  for(int i=0; i< 7; i++)
    {

      std::stringstream ss;

      ss << std::fixed << std::setprecision(4); // Set to 2 decimal places
      ss << xLabel[i];
      std::string formatted_string = ss.str();
    //    hCol->GetXaxis()->SetBinLabel(i+1,std::to_string(xLabel[i]).data());
      hCol->GetXaxis()->SetBinLabel(i+1,formatted_string.data());
    }
  for(int i=0; i< 3; i++)
    hCol->GetYaxis()->SetBinLabel(i+1,std::to_string(yLabel[i]).data());
  ifstream fin;
  fin.open("xSec.dat");
  for(int i=0;i<7;i++)
    {
      for(int j=0; j<3; j++)
	{
	  float x, y, xsec;
	  fin >> x >> y >> xsec;
	  hxSec->SetBinContent(i+1,j+1, xsec);
	  hCol->SetBinContent(i+1,j+1, xsec);
	}
    };
  gStyle->SetOptStat(0);
  gStyle->SetPaintTextFormat("1.5f");
  hxSec->SetMarkerSize(1.5);
  hCol->Draw("colz");
  hxSec->Draw("TEXT SAME");
  c1->Print("2HDMaI_varygDM_explictbb.pdf");
  c1->Print("2HDMaI_varygDM_explictbb.gif");
}
