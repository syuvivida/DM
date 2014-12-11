#include <iostream>
#include <fstream>
#include <TFile.h>
#include <TTree.h>
#include <TH1.h>
#include <vector>
#include <string>
#include <TCanvas.h>
#include <TLegend.h>
#include <TStyle.h>
#include <string>
#include <TString.h>
#include <TSystem.h>
using namespace std;

void plotAll(std::string histoName,std::string txtFile)
{
  std::vector<string> fileNames;
  std::vector<double> scales;
  std::vector<string> titles;
  const double lumi = 20000.0;
  FILE *fTable = fopen(txtFile.data(),"r");
  int flag=1;
  int nfile=0;
  char tmp[300];
  while (flag!=-1){
    // first reading input file
    flag=fscanf(fTable,"%s",tmp);
    std::string tempFile = tmp;

    TString endfix = gSystem->GetFromPipe(Form("file=%s; test3=${file##*DMHisto_delpanj_v4_}; test2=${test3%%.root*}; test1=${test2%%_8TeV*};echo \"${test1%%_TuneZ2star*}\"",tempFile.data()));
    // read in x-section
    flag=fscanf(fTable,"%s",tmp);
    double cross=atof(tmp);
    // read in number of events
    flag=fscanf(fTable,"%s",tmp);
    double nevt=atof(tmp);
    double scale = lumi*cross/nevt;
    cout << "scale = " << scale << endl;
    if (flag!=-1) {
      fileNames.push_back(tempFile);
      scales.push_back(scale);
      titles.push_back(Form("%s",endfix.Data()));
      nfile++;
    }
  }
  
  cout << "number of files = " << nfile<< endl;
  const int nFiles=nfile;
  TH1F* h[nFiles];
  TH1F* scaleh[nFiles];
  float maxLumi=-9999;
  float maxNorm=-9999;

  int COLOR[6]={1, kRed, kBlue, kGreen+3, kOrange,kMagenta+1};
  for(int i=0; i<nFiles; i++)
    { 

      TFile *inf = new TFile(fileNames[i].data());
      h[i] = (TH1F*)inf->FindObjectAny(histoName.data());	
      h[i]->SetName(Form("h%d",i));
      h[i]->Sumw2();
      h[i]->Scale(scales[i]);
      h[i]->SetLineWidth(3);  
      h[i]->SetMarkerColor(COLOR[i%6]);
      h[i]->SetMarkerSize(1);
      h[i]->SetLineColor(COLOR[i%6]);

      if(h[i]->GetMaximum()>maxLumi)
	maxLumi = h[i]->GetMaximum();
      h[i]->SetMaximum(1.5*maxLumi);

      scaleh[i]= (TH1F*)inf->FindObjectAny(histoName.data());	
      scaleh[i]->SetName(Form("scaleh%d",i));
      scaleh[i]->Sumw2();     
      scaleh[i]->Scale(1.0/scaleh[i]->Integral());
      scaleh[i]->SetLineWidth(3);  
      scaleh[i]->SetMarkerColor(COLOR[i%6]);
      scaleh[i]->SetMarkerSize(1);
      scaleh[i]->SetLineColor(COLOR[i%6]);
      
      if(scaleh[i]->GetMaximum()>maxNorm)
	maxNorm = scaleh[i]->GetMaximum();

      scaleh[i]->SetMaximum(1.5*maxNorm);

    }

  TLegend* leg = new TLegend(0.360887,0.670886,0.518145,0.892405) ;
 
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->SetTextSize(0.04);
 

  TCanvas* c1 = new TCanvas("c1","",500,500);
  gStyle->SetOptStat(0);
  // first normalize according to luminosity
  h[0]->Draw("hist");
  for(int i=nFiles-1; i>=0; i--)
    {
      h[i]->Draw("histsame");
      leg->AddEntry(h[i],titles[i].data());
    }
  leg->Draw("same");

  gSystem->mkdir("DMfigures");

  std::string temp2="DMfigures/lumiNorm_" + histoName;
  c1->Print(Form("%s.eps",temp2.data()));
  c1->Print(Form("%s.gif",temp2.data()));
  c1->Print(Form("%s.pdf",temp2.data()));

  scaleh[0]->Draw("hist");
  for(int i=nFiles-1; i>=0; i--)
    {
      scaleh[i]->Draw("histsame");
    }
  
  leg->Draw("same");
  temp2="DMfigures/unitNorm_" + histoName;
  c1->Print(Form("%s.eps",temp2.data()));
  c1->Print(Form("%s.gif",temp2.data()));
  c1->Print(Form("%s.pdf",temp2.data()));


}
