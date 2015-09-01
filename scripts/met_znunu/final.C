#include <TEfficiency.h>
#include <TH1.h>
#include <TH2.h>
#include <iostream>
#include <string>
#include <TFile.h>
#include <TMatrixD.h>
#include <TVectorD.h>
#include <TMath.h>
using namespace std;

void final(string znnfile, string zmmfile, string corrfile, string datafile, string outputfile)
{

  TH1F* inputmet;
  TH1F* corrmet1D;
  TH1F* genmet;
  TH1F* compmet;
  TH1F* compmet_data;

  TH1F* outputgenmet;
  TH1F* outputmet2D;
  TH1F* outputmet1D;
  TH1F* outputrev;

  TFile* file_znn   = TFile::Open(znnfile.data());
  TFile* file_zmm   = TFile::Open(zmmfile.data());
  TFile* file_corr  = TFile::Open(corrfile.data());
  TFile* file_data  = TFile::Open(datafile.data());

  inputmet = (TH1F*)file_zmm->FindObjectAny("h_recmet_after_split_data1_total"); //26 bins
  inputmet->SetName("inputmet");

  corrmet1D = (TH1F*)file_corr->FindObjectAny("final_corr"); // 26 mins
  // for comparison
  genmet    = (TH1F*)file_zmm->FindObjectAny("h_genmet_pre_split1_total"); //26 bins
  genmet->SetName("genmet"); 

  compmet   = (TH1F*)file_znn->FindObjectAny("h_recmet_after_split_data1_total"); //26 bins
  compmet->SetName("compmet");

  compmet_data   = (TH1F*)file_data->FindObjectAny("h_recmet_after_data"); //26 bins
  compmet_data->SetName("compmet_data");


  outputmet2D = (TH1F*) inputmet->Clone("outputmet2D");
  outputmet2D->Reset();
  outputmet2D->Sumw2();

  outputmet1D = (TH1F*) inputmet->Clone("outputmet1D");
  outputmet1D->Reset();
  outputmet1D->Sumw2();

  outputgenmet= (TH1F*) inputmet->Clone("outputgenmet");
  outputgenmet->Reset();
  outputgenmet->Sumw2();

  outputrev= (TH1F*) inputmet->Clone("outputrev");
  outputrev->Reset();
  outputrev->Sumw2();


  const int nbins = inputmet->GetNbinsX()+2;

  // for debugging. check GenToRec matrix
  TMatrixD* GenToRec = new TMatrixD(nbins,nbins); // 36 bins
  GenToRec = (TMatrixD*)file_corr->FindObjectAny("ref_GenToRec");

  TVectorD ingenVector(nbins);
  for(int i=0;i<nbins;i++)
    ingenVector(i)= genmet->GetBinContent(i);
  TVectorD recVector = (*GenToRec)*ingenVector;

  for(int i=0;i<nbins;i++)
    outputrev->SetBinContent(i,recVector(i));


  TVectorD recmetVector(nbins);
  for(int i=0;i<nbins;i++)
    recmetVector(i)= inputmet->GetBinContent(i);  
  
  TMatrixD* corrRecToGen = new TMatrixD(nbins,nbins);
  corrRecToGen = (TMatrixD*)file_corr->FindObjectAny("ref_RecToGen");

  TMatrixD* corrGenToRec = new TMatrixD(nbins,nbins);
  corrGenToRec = (TMatrixD*)file_corr->FindObjectAny("nunu_GenToRec");

  // first check genMet
  TVectorD genVector=(*corrRecToGen)*recmetVector;
  for(int i=0;i<nbins;i++)
    outputgenmet->SetBinContent(i,genVector(i));
  
  TVectorD outputVector=(*corrGenToRec)*genVector;
  for(int i=0;i<nbins;i++)
    outputmet2D->SetBinContent(i,outputVector(i));

  const double brll = 3.366;
  const double brnn = 20.00;
  
  outputmet2D->Scale(brnn/brll);

  for(int i=0;i<nbins;i++)
    {
      double value1 = corrmet1D->GetBinContent(i+1) ;
      double err1   = corrmet1D->GetBinError(i+1);

      double value2 = inputmet->GetBinContent(i+1);
      double err2   = inputmet->GetBinError(i+1);

      double value  = value1* value2;
      if(value ==0)continue;
      double err = sqrt ( err1*err1/TMath::Max(1e-7,value1*value1) + 
   			  err2*err2/TMath::Max(1e-7,value2*value2) )*value;

      outputmet1D->SetBinContent( i+1, value);
      outputmet1D->SetBinError( i+1, err);

    }


  // write output
  TFile* outFile = new TFile(outputfile.data(),"recreate");
  inputmet->Write();
  genmet->Write();
  compmet->Write();
  compmet_data->Write();
  outputgenmet->Write();
  outputmet2D->Write();
  outputmet1D->Write();
  outputrev->Write();
  outFile->Close();
}
