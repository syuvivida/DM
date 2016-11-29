#include <TROOT.h>
#include <TFile.h>
#include <TH1.h>
#include <TTree.h>
#include <TProfile.h>
#include <TKey.h>
#include <TStyle.h>
#include <TMath.h>
#include <string>
#include <iostream>
#include <TSystem.h>
#include <TCanvas.h>
#include <TLegend.h>

using namespace std;


void dumpCompareMultiplePDF(std::string inputText_,std::string header,bool normalize=true,bool logy=false)
{

  std::string dirName = "plots";
  gSystem->mkdir(dirName.data());

  // TLegend* leg = new TLegend(0.6,0.435,0.83,0.857);
  TLegend* leg = new TLegend(0.45,0.435,0.83,0.857);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);

  std::string endfix=logy? "_logy.pdf":".pdf";

  TString outputPDF=gSystem->GetFromPipe(Form("file=%s; test=${file%%*.txt}; echo \"%s/${test}%s\"",inputText_.data(),dirName.data(),endfix.data()));

  cout << outputPDF << endl;

  gStyle->SetOptStat(0);

  
  FILE *fTable = fopen(inputText_.data(),"r");
   
  int flag=1;   

  std::vector<std::string> dataFile;
  std::vector<std::string> legendName;

  char filename[300];

  while (flag!=-1){
    // first reading input file
    flag=fscanf(fTable,"%s",filename);
    std::string tempFile = filename;

    flag=fscanf(fTable,"%s",filename);
    std::string tempLegend = filename;

    if (flag!=-1) {
      dataFile.push_back(tempFile);
      cout << "Reading " << tempFile.data() << endl;
      legendName.push_back(tempLegend);
      cout << "Legend is " << tempLegend.data() << endl;
    }
  }	 

  int nfile=dataFile.size();
  
  cout << "There are " << nfile << " files to compare" << endl;

  TFile* f[20];
  TH1F* h[20];

  const int NMAXFILE=10;

  if(nfile > NMAXFILE){
    cout << "Too many files!!" << endl;
    return;
  }


  TCanvas* c1 = new TCanvas("c1","",500,500);
  for(int i=0; i < nfile; i++){
    f[i] = TFile::Open(dataFile[i].data());
  }

  f[0]->cd();

  TDirectory *current_sourcedir = gDirectory;
  int nPage=0;
  // loop over all keys in this directory
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key;
  while ((key = (TKey*)nextkey()) ) {

    TObject *obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
      // descendant of TH1 -> scale it

      cout << "outputing histogram " << obj->GetName() << endl;
      float max=-999;
      for(int i=0; i < nfile; i++){
	h[i] = (TH1F*)(f[i]->FindObjectAny(obj->GetName()));
	cout << "reading " << h[i]->GetName() << endl;
	if(normalize)
	  h[i]->SetYTitle("Normalized Distributions");  
	h[i]->SetTitleOffset(1.4,"Y");  
	h[i]->GetYaxis()->SetDecimals();
	h[i]->GetYaxis()->SetNdivisions(5);
	h[i]->GetXaxis()->SetNdivisions(5);
	h[i]->SetLineColor(1+i);
	h[i]->SetLineWidth(3);
	h[i]->SetMarkerColor(1+i);
	
	gStyle->SetOptStat(0);
	h[i]->Sumw2();
	if(normalize)
	  {
	    h[i]->Scale(1.0/h[i]->Integral());
	  }
	
        float max1   = h[i]->GetBinError(h[i]->GetMaximumBin()) + h[i]->GetMaximum();

	if(max1>max)max = max1;
      }

      for(int i=0; i < nfile; i++){
	h[i]->SetMaximum(1.2*max);
      }

      h[0]->Draw("hist");
      for(int i=0; i<nfile; i++)h[i]->Draw("histsame");

      leg->Clear();
      leg->SetHeader(header.data());
      for(int i=0; i< nfile; i++)
	leg->AddEntry(h[i], legendName[i].data(),"l");
      leg->Draw("same");
      
      if(nPage==0)
	c1->Print(Form("%s(",outputPDF.Data()),"pdf");
      else 
	c1->Print(Form("%s",outputPDF.Data()),"pdf");
      nPage++;
    } // if the object is a histogram


  } // loop over keys
  if(logy)
    c1->SetLogy(1);
  c1->Print(Form("%s)",outputPDF.Data()),"pdf");
    


}
