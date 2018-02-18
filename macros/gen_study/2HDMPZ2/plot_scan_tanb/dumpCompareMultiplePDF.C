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
#include <TPaveText.h>
#include "DMWGStyle2017.cxx"

using namespace std;

void chi2NbinsCompare( const TH1* h1, const TH1 *h2, double& chi2, int& nbins , int binLo=-1, int binHi=-1){

  printf( "Chi2 Calculation: \n");

  chi2 = 0;
  nbins = 0;

  int nBins = 0;
  double evtThresh = 1e-6;

  double h1Err, h1Value, h2Err, h2Value, lowEdge, highEdge, binChi2;

  double  binWidth    = h1 -> GetXaxis() -> GetBinWidth(1);

  if(binLo < 0 || binHi < 0)
    {
      binLo = 1;
      binHi = h1->GetNbinsX();
    }

  for ( int i = binLo ; i <= binHi; i++){
 
    h1Value = h1 -> GetBinContent(i);
    h1Err   = h1 -> GetBinError(i);

    h2Value = h2 -> GetBinContent(i);
    h2Err   = h2 -> GetBinError(i);

    if ( h1Value < evtThresh && h2Value  < evtThresh) continue;
    if ( h1Err < 1e-6 && h2Err  < 1e-6) continue;
    lowEdge   = h1 -> GetXaxis() -> GetBinLowEdge( i );
    highEdge  = h1 -> GetXaxis() -> GetBinUpEdge( i );

    binChi2 = (h1Value - h2Value) / sqrt(h1Err*h1Err+h2Err*h2Err);
    binChi2 *= binChi2;
    
    chi2 += binChi2;

    // printf( "%d) [%d, %d] [%f, %f] h1: %f h2: %f h1Err: %f h2Err: %f chi2: %f total: %f \n ",  
    // 	    nBins, i, i, lowEdge, highEdge,h1Value, h2Value, h1Err, h2Err, binChi2, chi2); 
    
    nBins++;

  }
  int NDF = nBins;

  printf("Fit chi2/NDF = %f/%d, prob: %f\n", chi2, NDF, TMath::Prob(chi2,NDF)*100);
  nbins = NDF;
}

  

void dumpCompareMultiplePDF(std::string inputText_,int mh3, int mh4,bool normalize=true,bool logy=false)
{
  SetDMWGStyle();
  std::string dirName = "plots";
  gSystem->mkdir(dirName.data());

  TLegend* leg = new TLegend(0.6,0.5,0.9,0.9);
  // leg->SetFillColor(0);
  // leg->SetFillStyle(0);
  // leg->SetTextSize(0.04);
  // leg->SetBorderSize(0);

  std::string endfix=logy? "_logy.pdf":".pdf";

  TString outputPDF=gSystem->GetFromPipe(Form("file=%s; test=${file%%*.txt}; echo \"%s/${test}%s\"",inputText_.data(),dirName.data(),endfix.data()));

  cout << outputPDF << endl;

  TString pagePDF=gSystem->GetFromPipe(Form("file=%s; test=${file%%*.txt}; echo \"%s/${test}\"",inputText_.data(),dirName.data()));
  cout << pagePDF << endl;


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

  const int NMAXFILE=12;

  if(nfile > NMAXFILE){
    cout << "Too many files!!" << endl;
    return;
  }


  //  TCanvas* c1 = new TCanvas("c1","",500,500);
  TCanvas* c1 = new TCanvas("c1","");
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
	  h[i]->SetYTitle("Events (normalised to unity)");  
	h[i]->SetTitle("");
	h[i]->SetXTitle("#it{E}_{#it{T}}^{miss} [GeV]");
	//	h[i]->SetTitleOffset(1.4,"Y");  
	h[i]->GetYaxis()->SetDecimals();
	// h[i]->GetYaxis()->SetNdivisions(5);
	// h[i]->GetXaxis()->SetNdivisions(5);
	h[i]->SetLineColor((1+i)==5? kOrange: (1+i));
	h[i]->SetLineWidth(3);
	h[i]->SetMarkerColor((1+i)==5? kOrange: (1+i));
	h[i]->SetMarkerStyle(20+i);
	
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
	h[i]->SetMaximum(2*max);
      }

      //      h[0]->Draw("hist");
      h[0]->Draw("e");
      //      for(int i=0; i<nfile; i++)h[i]->Draw("histsame");
      for(int i=0; i<nfile; i++)h[i]->Draw("esame");

      leg->Clear();
      if(nfile==2)
	{
	  double chi2=0.;
	  int nbins=0;
	  chi2NbinsCompare(h[0],h[1],chi2,nbins,1,h[0]->GetNbinsX());
	  leg->AddEntry((TObject*)0, Form("#chi^{2}/NDF=%.1f/%d",chi2,nbins), "");
	  leg->AddEntry((TObject*)0, Form("#chi^{2} Prob=%.2f",TMath::Prob(chi2,nbins)), "");
	  leg->AddEntry((TObject*)0, "", "");
	}
      for(int i=0; i< nfile; i++)
	leg->AddEntry(h[i], legendName[i].data(),"pe");
      leg->Draw("same");
      

      TPaveText *pavetex = new TPaveText(0.15,0.72,0.55,0.9,"NDCBR");
      pavetex->SetBorderSize(0);
      pavetex->SetFillColor(0);
      pavetex->SetFillStyle(0);
      pavetex->SetLineWidth(3);
      pavetex->SetTextAlign(12);
      // pavetex->SetTextSize(0.055);
      pavetex->AddText(Form("#it{M_{H}} = #it{M_{H^{#pm}}} = #it{M_{A}} = %d GeV",mh3)); 
      pavetex->AddText(Form("#it{M_{a}} = %d GeV, sin(#it{#theta}) = 0.35",mh4));
      pavetex->AddText("#it{#lambda_{3}} = #it{#lambda_{P1}} = #it{#lambda_{P2}} = 3, #it{M_{#chi}} = 10 GeV");
      pavetex->Draw();
      c1->Print(Form("%s_%d.pdf",pagePDF.Data(),nPage));
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
