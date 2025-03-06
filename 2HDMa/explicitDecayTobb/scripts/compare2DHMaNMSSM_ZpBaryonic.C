#include <TChain.h>
#include <TH1.h>
#include <fstream>
#include <string>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TColor.h>


using namespace std;
int linecolor[9]={4,1,596,6,2,881,801,418,434};


void compare2DHMaNMSSM_ZpBaryonic(std::string histoName, std::string xtitle, const int nbin=100, const float xmin=0, const float xmax=500, const int mode=0){

  ifstream fin;
  vector<string> inputFileNames;
  vector<string> legends;
  fin.open("file2.dat");
  string tempName;
  string tempLegend;

  while(!fin.eof()){
    fin >> tempName >> tempLegend;
    if(!fin.eof())
      {
        inputFileNames.push_back(tempName);
        legends.push_back(tempLegend);
      }
  }
  unsigned int nfiles=inputFileNames.size();

  
  TChain* chain[nfiles];
  TH1F* htemplate=new TH1F("htemplate","",nbin,xmin,xmax);
  htemplate->SetXTitle(xtitle.data());
  TH1F* h[nfiles];
  float ymax=-999;
  for(int i=0;i<nfiles;i++)
    {
      h[i] = (TH1F*)htemplate->Clone(Form("h%d",i));
      chain[i] =new TChain("lhedump/tree");
      chain[i]->Add(inputFileNames[i].data());
      h[i]->SetLineWidth(3);
      if(i%3==0)
	h[i]->SetLineStyle(2);
      else if(i%3==1)
	h[i]->SetLineStyle(1);
      else
	{
	  h[i]->SetLineStyle(3);
	  //	  h[i]->SetFillStyle(3004);
	  //	  h[i]->SetFillColor(linecolor[i]);
	}
      h[i]->SetLineColor(linecolor[i]);
      chain[i]->Draw(Form("%s>>h%d",histoName.data(),i));
      if(h[i]->GetMaximum()>ymax)ymax=h[i]->GetMaximum();
    }

  TCanvas* c1 = new TCanvas("c1");
  gStyle->SetOptStat(0);
  float X1NDC=0.470;
  float Y1NDC=0.472;
  float X2NDC=0.910;
  float Y2NDC=0.882;
  if(mode==1){
    X1NDC=0.120;
    Y1NDC=0.145;
    X2NDC=0.562;
    Y2NDC=0.558;
  }
  
  TLegend* leg = new TLegend(X1NDC,Y1NDC,X2NDC,Y2NDC);
  leg->SetHeader("NMSSM vs. 2HDM+a vs. Z'-Baryonic");
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  for(unsigned int i=0; i < nfiles; i++){
    h[i]->GetYaxis()->SetRangeUser(0,1.1*ymax);
    //    if(i==0)h[i]->Draw();
    //    else h[i]->Draw("same");
    if(i<3 || i>5)continue;
    if(i==3)h[i]->Draw();
    else h[i]->Draw("same"); 
    leg->AddEntry(h[i], Form("%s",legends[i].data()));
    leg->Draw("same");
  }

  c1->Print(Form("2HDMavsNMSSMvsZpBaryonic_%s.pdf",histoName.data()));
  c1->Print(Form("2HDMavsNMSSMvsZpBaryonic_%s.gif",histoName.data()));

}
