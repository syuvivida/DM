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

void final(string znnfile, string zmmfile, string corrfile, string outputfile)
{

  TH1F* inputmet;
  TH1F* corrmet1D;
  TH1F* genmet;
  TH1F* compmet;

  TH1F* outputgenmet;
  TH1F* outputmet2D;
  TH1F* outputmet1D;
  TH1F* outputrev;

  TFile *inf = TFile::Open(znnfile.data());
  TFile *inf2 = TFile::Open(zmmfile.data());
  TFile *inf3 = TFile::Open(corrfile.data());

  inputmet = (TH1F*)inf2->FindObjectAny("h_recmet_after_split1_total");
  inputmet->SetName("inputmet");

  corrmet1D = (TH1F*)inf3->FindObjectAny("final_corr");
  // for comparison
  genmet    = (TH1F*)inf2->FindObjectAny("h_genmet_pre_split1_total");
  genmet->SetName("genmet");
  compmet   = (TH1F*)inf->FindObjectAny("h_recmet_after_split1_total");
  compmet->SetName("compmet");

  outputmet2D = (TH1F*) inputmet->Clone("outputmet2D");
  outputmet2D->Reset();

  outputmet1D = (TH1F*) inputmet->Clone("outputmet1D");
  outputmet1D->Reset();

  outputgenmet= (TH1F*) inputmet->Clone("outputgenmet");
  outputgenmet->Reset();

  outputrev= (TH1F*) inputmet->Clone("outputrev");
  outputrev->Reset();


  const int nbins = inputmet->GetNbinsX();

  // for debugging. check GenToRec matrix
  TMatrixD* GenToRec = new TMatrixD(nbins,nbins);
  GenToRec = (TMatrixD*)inf3->FindObjectAny("ref_GenToRec");

  TVectorD ingenVector(nbins);
  for(int i=0;i<nbins;i++)
    ingenVector(i)= genmet->GetBinContent(i+1);
  TVectorD recVector = (*GenToRec)*ingenVector;

  for(int i=0;i<nbins;i++)
    outputrev->SetBinContent(i+1,recVector(i));


  TVectorD recmetVector(nbins);
  for(int i=0;i<nbins;i++)
    recmetVector(i)= inputmet->GetBinContent(i+1);  
  
  TMatrixD* corrRecToGen = new TMatrixD(nbins,nbins);
  corrRecToGen = (TMatrixD*)inf3->FindObjectAny("ref_RecToGen");

  TMatrixD* corrGenToRec = new TMatrixD(nbins,nbins);
  corrGenToRec = (TMatrixD*)inf3->FindObjectAny("nunu_GenToRec");

  // first check genMet
  TVectorD genVector=(*corrRecToGen)*recmetVector;
  for(int i=0;i<nbins;i++)
    outputgenmet->SetBinContent(i+1,genVector(i));
  
  TVectorD outputVector=(*corrGenToRec)*genVector;
  for(int i=0;i<nbins;i++)
    outputmet2D->SetBinContent(i+1,outputVector(i));

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
      double err = sqrt ( err1*err1/TMath::Max(1e-7,value1*value1) + 
   			  err2*err2/TMath::Max(1e-7,value2*value2) )*value;

      outputmet1D->SetBinContent( i+1, value);
      outputmet1D->SetBinError( i+1, err);

    }

  outputmet1D->Scale(brnn/brll);

  // write output
  TFile* outFile = new TFile(outputfile.data(),"recreate");
  inputmet->Write();
  genmet->Write();
  compmet->Write();
  outputgenmet->Write();
  outputmet2D->Write();
  outputmet1D->Write();
  outputrev->Write();
  outFile->Close();
}
