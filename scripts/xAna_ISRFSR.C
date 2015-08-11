// example code to run Bulk Graviton->ZZ->ZlepZhad selections on electron-channel

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TString.h>
#include <map>
#include <TH1D.h>
#include <TProfile.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>

using namespace std;
void xAna_ISRFSR(std::string inputFile){
  TString outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*NCUGlobalTuples_}; echo \"histo_${test}\"",inputFile.data()));

  //get TTree from file ...
  TreeReader data(inputFile.data());
  TH1F* h_njet=new TH1F("h_njet","",11,-0.5,10.5);
  TH1F* h_njet30=new TH1F("h_njet30","",11,-0.5,10.5);
  TH1F* hdR_ISR=new TH1F("hdR_ISR","",30,0,6);
  TH1F* hdR_FSR=new TH1F("hdR_FSR","",30,0,6);
  TH1F* h_SD=new TH1F("h_SD","",100,0,200);


  TProfile* hptfr_ISR1=new TProfile("hptfr_ISR1","",100,0,500);
  TProfile* hptfr_FSR1=new TProfile("hptfr_FSR1","",100,0,500);

  TProfile* hptfr_ISR=new TProfile("hptfr_ISR","",100,0,500);
  TProfile* hptfr_FSR=new TProfile("hptfr_FSR","",100,0,500);

  TProfile* hetafr_ISR1=new TProfile("hetafr_ISR1","",30,0,3.0);
  TProfile* hetafr_FSR1=new TProfile("hetafr_FSR1","",30,0,3.0);

  TProfile* hetafr_ISR=new TProfile("hetafr_ISR","",30,0,3.0);
  TProfile* hetafr_FSR=new TProfile("hetafr_FSR","",30,0,3.0);

  Long64_t nTotal=0;
  Long64_t nPass[20]={0};
  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
  // for(Long64_t jEntry=0; jEntry<10 ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // 0. check the generator-level information and make sure there is a Z->e+e-
    Int_t nGenPar        = data.GetInt("nGenPar");
    Int_t* genParId      = data.GetPtrInt("genParId");
    Int_t* genParSt      = data.GetPtrInt("genParSt");
    Int_t* genMomParId   = data.GetPtrInt("genMomParId");
    Int_t* genDa1      = data.GetPtrInt("genDa1");
    Int_t* genDa2      = data.GetPtrInt("genDa2");

    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");


    // 1. make sure there is a h->bb
    bool hasHadron=false;
    TLorentzVector genb_l4(0,0,0,0);
    TLorentzVector genbbar_l4(0,0,0,0);

    for(int ig=0; ig < nGenPar; ig++){

      if(genParId[ig]!=25)continue;
      int da1=genDa1[ig];
      int da2=genDa2[ig];

      if(da1<0 || da2<0)continue;
      int da1pdg = genParId[da1];
      int da2pdg = genParId[da2];

      if(abs(da1pdg)!=5 || abs(da2pdg)!=5)
	continue;
      
      if(da1pdg==5 && da2pdg==-5)
	{
	  genb_l4 = *((TLorentzVector*)genParP4->At(da1));
	  genbbar_l4 = *((TLorentzVector*)genParP4->At(da2));
	}

      else if(da1pdg== -5 && da2pdg== 5)
	{
	  genb_l4 = *((TLorentzVector*)genParP4->At(da2));
	  genbbar_l4 = *((TLorentzVector*)genParP4->At(da1));
	}

      hasHadron=true;
      if(hasHadron)break;

    }
    
    if(!hasHadron)continue;
    nPass[1]++;


    // make sure there is an A0
    bool hasA0=false;
    TLorentzVector genn_l4(0,0,0,0);
    TLorentzVector gennbar_l4(0,0,0,0);
    TLorentzVector genZprime_l4(0,0,0,0);

    for(int ig=0; ig < nGenPar; ig++){
      if(genParSt[ig]>30)continue;

      if(genParId[ig]==32)
	genZprime_l4= *((TLorentzVector*)genParP4->At(ig));
      
      if(genParId[ig]!=28)continue;
      int da1=genDa1[ig];
      int da2=genDa2[ig];

      if(da1<0 || da2<0)continue;
      int da1pdg = genParId[da1];
      int da2pdg = genParId[da2];

      if(abs(da1pdg)!=18 || abs(da2pdg)!=18)
	continue;
      
      if(da1pdg==18 && da2pdg==-18)
	{
	  genn_l4 = *((TLorentzVector*)genParP4->At(da1));
	  gennbar_l4 = *((TLorentzVector*)genParP4->At(da2));
	}

      else if(da1pdg== -18 && da2pdg== 18)
	{
	  genn_l4 = *((TLorentzVector*)genParP4->At(da2));
	  gennbar_l4 = *((TLorentzVector*)genParP4->At(da1));
	}

      hasA0=true;
      if(hasA0)break;

    }
    
    if(!hasA0)continue;
    nPass[2]++;

    Int_t nAk4GenJet        = data.GetInt("nAk4GenJet");
    TClonesArray* ak4GenJetP4 = (TClonesArray*) data.GetPtrTObject("ak4GenJetP4");

    Int_t nExtra = 0; 
    Int_t nExtra30 = 0; 

    
    TLorentzVector b_l4(0,0,0,0);
    TLorentzVector bbar_l4(0,0,0,0);

    for(int ij=0; ij < nAk4GenJet; ij++){
      
      TLorentzVector* thisJet = (TLorentzVector*)ak4GenJetP4->At(ij);
      if(thisJet->DeltaR(genb_l4)<0.4 && b_l4.E()<1e-6)
	b_l4 = *thisJet;

      if(thisJet->DeltaR(genbbar_l4)<0.4 && bbar_l4.E()<1e-6)
	bbar_l4 = *thisJet;

      if(b_l4.Pt()>0.5 && bbar_l4.Pt()>0.5)break;

    }
    for(int ij=0; ij < nAk4GenJet; ij++){
      
      TLorentzVector* thisJet = (TLorentzVector*)ak4GenJetP4->At(ij);
      if(thisJet->DeltaR(genb_l4)<0.4)continue;
      if(thisJet->DeltaR(genbbar_l4)<0.4)continue;
      if(thisJet->DeltaR(genn_l4)<0.4)continue;
      if(thisJet->DeltaR(gennbar_l4)<0.4)continue;
      nExtra++;

      double ptmax=-9999;
      int matchedIndex=-1;
      for(int ig=0; ig < nGenPar; ig++){

	int PID=abs(genParId[ig]);
	if(PID!=21 && !(PID>=1 && PID<=4) )continue;
      	int st=genParSt[ig];
	if(st<41 || st>59)continue;
	// cout << "PID = " << PID << " st = " << st << endl;
	TLorentzVector* thisGen = (TLorentzVector*)genParP4->At(ig);
	if(thisGen->Pt()<10.0)continue;
	if(thisJet->DeltaR(*(thisGen))>0.4)continue;
       
	if(thisGen->Pt()>ptmax)
	  {
	    ptmax=thisGen->Pt();
	    matchedIndex=ig;
	  }
      }

      // cout << "matchedIndex= " << matchedIndex << endl;
       bool isISR=false;
       bool isFSR=false;

       if(matchedIndex>=0)
       	{
       	  int st = genParSt[matchedIndex];
       	  if(st>40 && st<50)isISR=true;
       	  else isFSR=true;
       	}
      
       if(isISR)
       	{
	  float dR = sqrt(pow(thisJet->DeltaPhi(genZprime_l4),2)+
			  pow(thisJet->Rapidity()-genZprime_l4.Rapidity(),2));
	  hdR_ISR->Fill(dR);	

       	  hptfr_ISR->Fill(thisJet->Pt(),1.0);
       	  hetafr_ISR->Fill(fabs(thisJet->Eta()),1.0);

       	  hptfr_FSR->Fill(thisJet->Pt(),0.0);
       	  hetafr_FSR->Fill(fabs(thisJet->Eta()),0.0);

       	  if(nExtra==1)
       	    {
       	      hptfr_ISR1->Fill(thisJet->Pt(),1.0);
       	      hetafr_ISR1->Fill(fabs(thisJet->Eta()),1.0);

       	      hptfr_FSR1->Fill(thisJet->Pt(),0.0);
       	      hetafr_FSR1->Fill(fabs(thisJet->Eta()),0.0);
       	    }
         
       	}
       else if(isFSR)
       	{
       	  float dR1=thisJet->DeltaR(b_l4);
       	  float dR2=thisJet->DeltaR(bbar_l4);
        
       	  float dRmin = (dR1>dR2)?dR2:dR1;

       	  hdR_FSR->Fill(dRmin);	

       	  hptfr_ISR->Fill(thisJet->Pt(),0.0);
       	  hetafr_ISR->Fill(fabs(thisJet->Eta()),0.0);

       	  hptfr_FSR->Fill(thisJet->Pt(),1.0);
       	  hetafr_FSR->Fill(fabs(thisJet->Eta()),1.0);
       	  if(nExtra==1)
       	    {

       	      hptfr_ISR1->Fill(thisJet->Pt(),0.0);
       	      hetafr_ISR1->Fill(fabs(thisJet->Eta()),0.0);

       	      hptfr_FSR1->Fill(thisJet->Pt(),1.0);
       	      hetafr_FSR1->Fill(fabs(thisJet->Eta()),1.0);
       	    }
         
       	}
       else
       	{

       	  hptfr_ISR->Fill(thisJet->Pt(),0.0);
       	  hetafr_ISR->Fill(fabs(thisJet->Eta()),0.0);

       	  hptfr_FSR->Fill(thisJet->Pt(),0.0);
       	  hetafr_FSR->Fill(fabs(thisJet->Eta()),0.0);
       	  if(nExtra==1)
       	    {

       	      hptfr_ISR1->Fill(thisJet->Pt(),0.0);
       	      hetafr_ISR1->Fill(fabs(thisJet->Eta()),0.0);

       	      hptfr_FSR1->Fill(thisJet->Pt(),0.0);
       	      hetafr_FSR1->Fill(fabs(thisJet->Eta()),0.0);
       	    }


       	}
	     
      if(thisJet->Pt()<30 || fabs(thisJet->Eta())>2.5)continue;
      nExtra30++;
    }

    h_njet->Fill(nExtra);
    h_njet30->Fill(nExtra30);

  } // end of loop over entries

  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;
  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  h_njet->Write();
  h_njet30->Write();

  hdR_ISR->Write();
  hdR_FSR->Write();

  hptfr_ISR1->Write();
  hptfr_FSR1->Write();

  hptfr_ISR->Write();
  hptfr_FSR->Write();

  hetafr_ISR1->Write();
  hetafr_FSR1->Write();

  hetafr_ISR->Write();
  hetafr_FSR->Write();

  outFile->Close();
}
