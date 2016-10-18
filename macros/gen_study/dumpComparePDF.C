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

  

void dumpComparePDF(std::string inputFile1_,std::string inputFile2_,bool normalize=true)
{

  TString endfix1=gSystem->GetFromPipe(Form("file=%s; test=${file##*/}; echo \"${test%%.root}\"",inputFile1_.data()));

  cout << endfix1 << endl;

  TString endfix2=gSystem->GetFromPipe(Form("file=%s; test=${file##*/}; echo \"${test%%.root}\"",inputFile2_.data()));


  cout << endfix2 << endl;

  TLegend* leg = new TLegend(0.3333,0.7027,0.8333,0.9023);
  leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.04);
  leg->SetBorderSize(0);

  std::string dirName = "fig";
  gSystem->mkdir(dirName.data());

  std::string outputFileName=Form("%s/%s_%s.pdf",dirName.data(),
				  endfix1.Data(),
				  endfix2.Data());

  TCanvas* c1 = new TCanvas("c1","",500,500);

  TFile *_file1 = TFile::Open(inputFile1_.data());
  TFile *_file2 = TFile::Open(inputFile2_.data());
  _file1->cd();

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
      TH1 *h1 = (TH1*)_file1->FindObjectAny(obj->GetName());
      h1->SetMarkerStyle(8);
      h1->SetMarkerSize(1);
      h1->SetLineWidth(3);
      h1->SetLineColor(4);

      TH1 *h2 = (TH1*)_file2->FindObjectAny(obj->GetName());
      h2->SetMarkerStyle(25);
      h2->SetMarkerSize(1);
      h2->SetLineWidth(3);
      h2->SetLineColor(2);
			 			 
      gStyle->SetOptStat(0);
      h1->Sumw2();
      h2->Sumw2();
      if(normalize)
	{
	  h1->Scale(1.0/h1->Integral());
	  h2->Scale(1.0/h2->Integral());
	}

      float max = h1->GetMaximum()>h2->GetMaximum()?
	h1->GetMaximum():h2->GetMaximum();

      h1->SetMaximum(1.1*max);
      h2->SetMaximum(1.1*max);

      double chi2=0.;
      int nbins=0;
      chi2NbinsCompare(h1,h2,chi2,nbins,1,h1->GetNbinsX());

      h1->Draw("hist");
      h2->Draw("histsame");

      leg->Clear();
      leg->SetHeader("");
      leg->AddEntry((TObject*)0, Form("#chi^{2}/NDF=%.1f/%d",chi2,nbins), "");
      leg->AddEntry((TObject*)0, Form("#chi^{2} Prob=%.2f",TMath::Prob(chi2,nbins)), "");
      leg->AddEntry((TObject*)0, "", "");
      leg->AddEntry(h1,endfix1.Data(),"l");
      leg->AddEntry(h2,endfix2.Data(),"l");
      leg->Draw("same");

      if(nPage==0)
	c1->Print(Form("%s(",outputFileName.data()),"pdf");
      else 
	c1->Print(Form("%s",outputFileName.data()),"pdf");
      nPage++;
    } // if the object is a histogram


  } // loop over keys

  c1->Print(Form("%s)",outputFileName.data()),"pdf");
    


}
