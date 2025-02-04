#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <TFile.h>
#include <TH1.h>
#include <TChain.h>
#include <TLegend.h>
#include <TStyle.h>

using namespace std;

void compareHistos(string inputData){

  ifstream fin;
  vector<string> inputFileNames;
  vector<int> massNames;
  vector<float> xSecs;
  fin.open(inputData.data());
  string tempName;
  int tempMass;
  float tempXSec;
  while(!fin.eof()){
    fin >> tempMass >> tempName >> tempXSec;
    if(!fin.eof())
      {
	massNames.push_back(tempMass);
	inputFileNames.push_back(tempName);
	xSecs.push_back(tempXSec);
      }
  }
  unsigned int nFiles=inputFileNames.size();
  //  TFile* f[nFiles];
  TChain* t[nFiles];
  TH1F* hmet_template=new TH1F("hmet_template","",50,0,200);
  hmet_template->SetXTitle("p_{T}^{miss} [GeV]");
  hmet_template->SetYTitle("Arbitrary Unit");
  
  TH1F* h[nFiles];
  cout << "size = " << nFiles << endl;
  for(unsigned int i=0; i< nFiles; i++){
    cout << inputFileNames[i] << endl;
    //    f[i]=TFile::Open(inputFileNames[i].data());
    //    h[i]=(TH1F*)(f[i]->Get(")
    t[i]=new TChain("lhedump/tree");
    t[i]->Add(inputFileNames[i].data());
    h[i]=(TH1F*)hmet_template->Clone(Form("hmet%d",i));
    h[i]->SetLineWidth(3);
    h[i]->SetLineColor(i+1);
    h[i]->SetLineStyle(i+1);
    t[i]->Draw(Form("met>>hmet%d",i));

  }

  gStyle->SetOptStat(0);
  TLegend* leg = new TLegend(0.45702,0.475789,0.899713,0.88);
  leg->SetHeader("gg#rightarrow hh+#chi#chi, #sqrt{s}=13.6 TeV");
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);
  for(unsigned int i=0; i < nFiles; i++){
    if(i==0)h[i]->Draw();
    else h[i]->Draw("same");
  }
  for(unsigned int i=0; i < nFiles; i++){
    leg->AddEntry(h[i], Form("M_{H2}=%d GeV, #sigma=%.1f fb",massNames[i],xSecs[i]));
    leg->Draw("same");
  }
  
  
}
