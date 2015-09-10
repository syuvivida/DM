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

void corr(const TH1F*,const TH1F*, TH1F*);

void final(string znnfile, string zmmfile, string ttfile, string corrfile, string zmmdatafile, string outputfile)
{

  TH1F* testmet_mc;
  TH1F* testmet_data;

  TH1F* inputmet_mc;
  TH1F* genmet;
  TH1F* ttmet_mc;
  TH1F* corrmet1D;
  TH1F* inputmet_data;

  TH1F* outputgenmet;
  TH1F* outputmet2D;
  TH1F* outputmet1D;
  TH1F* outputmetdata;
  TH1F* outputrev;

  TFile* file_znn   = TFile::Open(znnfile.data());
  TFile* file_zmm   = TFile::Open(zmmfile.data());
  TFile* file_tt    = TFile::Open(ttfile.data()); 
  TFile* file_corr  = TFile::Open(corrfile.data());
  TFile* file_zmmdata  = TFile::Open(zmmdatafile.data());

  testmet_mc   = (TH1F*)file_znn->FindObjectAny("h_recmet_after_split_data1_total"); //24 bins
  testmet_mc->SetName("testmet_mc");

  testmet_data   = (TH1F*)file_znn->FindObjectAny("h_recmet_after_data_total"); //24 bins, no splitting
  testmet_data->SetName("testmet_data");

  inputmet_mc = (TH1F*)file_zmm->FindObjectAny("h_recmet_after_split_data1_total"); //24 bins
  inputmet_mc->SetName("inputmet_mc");

  // for debugging
  genmet    = (TH1F*)file_zmm->FindObjectAny("h_genmet_pre_split1_total"); //24 bins
  genmet->SetName("genmet"); 

  ttmet_mc   = (TH1F*)file_tt->FindObjectAny("h_recmet_after_data_total"); //24 bins
  ttmet_mc->SetName("ttmet_mc");

  corrmet1D = (TH1F*)file_corr->FindObjectAny("final_corr"); // 24 mins

 
  inputmet_data   = (TH1F*)file_zmmdata->FindObjectAny("h_recmet_after_data_total"); //24 bins
  inputmet_data->SetName("inputmet_data");
  inputmet_data->Sumw2();
  inputmet_data->Add(ttmet_mc,-1);


  outputmet2D = (TH1F*) inputmet_mc->Clone("outputmet2D");
  outputmet2D->Reset();
  outputmet2D->Sumw2();

  outputmet1D = (TH1F*) inputmet_mc->Clone("outputmet1D");
  outputmet1D->Reset();
  outputmet1D->Sumw2();

  outputgenmet= (TH1F*) inputmet_mc->Clone("outputgenmet");
  outputgenmet->Reset();
  outputgenmet->Sumw2();

  outputrev= (TH1F*) inputmet_mc->Clone("outputrev");
  outputrev->Reset();
  outputrev->Sumw2();

  outputmetdata = (TH1F*) inputmet_mc->Clone("outputmetdata");
  outputmetdata->Reset();
  outputmetdata->Sumw2();


  const int nbins = inputmet_mc->GetNbinsX()+2;

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
    recmetVector(i)= inputmet_mc->GetBinContent(i);  
  
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

  corr(corrmet1D,inputmet_mc,outputmet1D);
  corr(corrmet1D,inputmet_data,outputmetdata);


  // write output
  TFile* outFile = new TFile(outputfile.data(),"recreate");
  inputmet_mc->Write();
  genmet->Write();
  testmet_mc->Write();
  testmet_data->Write();
  ttmet_mc->Write();
  inputmet_data->Write();
  outputgenmet->Write();
  outputmet2D->Write();
  outputmet1D->Write();
  outputrev->Write();
  outputmetdata->Write();
  outFile->Close();
}



void corr(const TH1F* corrmet, const TH1F* input, TH1F* output)
{

  const int nbins = corrmet->GetNbinsX();
  for(int i=0;i<nbins;i++)
    {
      double value1 = corrmet->GetBinContent(i+1) ;
      double err1   = corrmet->GetBinError(i+1);

      double value2 = input->GetBinContent(i+1);
      double err2   = input->GetBinError(i+1);

      double value  = value1* value2;
      if(value ==0)continue;
      double err = sqrt ( err1*err1/TMath::Max(1e-7,value1*value1) + 
   			  err2*err2/TMath::Max(1e-7,value2*value2) )*value;

      output->SetBinContent( i+1, value);
      output->SetBinError( i+1, err);

    }

  return;

}

