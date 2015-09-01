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
  
  TH1F* h_dR=new TH1F("h_dR","",30,0,6);
  h_dR->SetXTitle("#Delta R");
  TH1F* hdR_ISR[2];
  TH1F* hdR_FSR[2];
  TH1F* hdR_Other[2];
  
  std::string title[2]={"#Delta R with Z'",
			"Closest #Delta R with b"};
  for(int i=0; i<2; i++){
    hdR_ISR[i] = (TH1F*)h_dR->Clone(Form("hdR_ISR%d",i));
    hdR_ISR[i]->SetTitle(title[i].data());

    hdR_FSR[i] = (TH1F*)h_dR->Clone(Form("hdR_FSR%d",i));
    hdR_FSR[i]->SetTitle(title[i].data());

    hdR_Other[i] = (TH1F*)h_dR->Clone(Form("hdR_Other%d",i));
    hdR_Other[i]->SetTitle(title[i].data());

  }

  TH1F* h_SD=new TH1F("h_SD","",100,0,200);
  TProfile* hpt=new TProfile("hpt","",100,0,500);
  hpt->SetXTitle("p_{T} [GeV]");
  hpt->SetYTitle("Fraction");

  TProfile* heta=new TProfile("heta","",30,0,3.0);
  heta->SetXTitle("|#eta|");
  heta->SetYTitle("Fraction");

  TProfile* hptfr_ISR[2];
  TProfile* hptfr_FSR[2];
  TProfile* hptfr_Other[2];

  TProfile* hetafr_ISR[2];
  TProfile* hetafr_FSR[2];
  TProfile* hetafr_Other[2];

  title[0]="All extra jets";
  title[1]="Leading extra jets";
 
  for(int i=0; i<2;i++)
    {
      hptfr_ISR[i]=(TProfile*)hpt->Clone(Form("hptfr_ISR%d",i));
      hptfr_FSR[i]=(TProfile*)hpt->Clone(Form("hptfr_FSR%d",i));
      hptfr_Other[i]=(TProfile*)hpt->Clone(Form("hptfr_Other%d",i));

      hetafr_ISR[i]=(TProfile*)heta->Clone(Form("hetafr_ISR%d",i));
      hetafr_FSR[i]=(TProfile*)heta->Clone(Form("hetafr_FSR%d",i));
      hetafr_Other[i]=(TProfile*)heta->Clone(Form("hetafr_Other%d",i));

      hptfr_ISR[i]->SetTitle(title[i].data());
      hptfr_FSR[i]->SetTitle(title[i].data());
      hptfr_Other[i]->SetTitle(title[i].data());

      hetafr_ISR[i]->SetTitle(title[i].data());
      hetafr_FSR[i]->SetTitle(title[i].data());
      hetafr_Other[i]->SetTitle(title[i].data());

    }
  Long64_t nTotal=0;
  Long64_t nPass[20]={0};
  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

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

       float dR = sqrt(pow(thisJet->DeltaPhi(genZprime_l4),2)+
		       pow(thisJet->Rapidity()-genZprime_l4.Rapidity(),2));

       float dR1=thisJet->DeltaR(b_l4);
       float dR2=thisJet->DeltaR(bbar_l4);
       
       float dRmin = (dR1>dR2)?dR2:dR1;
      
       if(isISR)
       	{
	  hdR_ISR[0]->Fill(dR);	
	  hdR_ISR[1]->Fill(dRmin);

       	  hptfr_ISR[0]->Fill(thisJet->Pt(),1.0);
       	  hetafr_ISR[0]->Fill(fabs(thisJet->Eta()),1.0);

       	  hptfr_FSR[0]->Fill(thisJet->Pt(),0.0);
       	  hetafr_FSR[0]->Fill(fabs(thisJet->Eta()),0.0);

       	  hptfr_Other[0]->Fill(thisJet->Pt(),0.0);
       	  hetafr_Other[0]->Fill(fabs(thisJet->Eta()),0.0);

       	  if(nExtra==1)
       	    {
       	      hptfr_ISR[1]->Fill(thisJet->Pt(),1.0);
       	      hetafr_ISR[1]->Fill(fabs(thisJet->Eta()),1.0);

       	      hptfr_FSR[1]->Fill(thisJet->Pt(),0.0);
       	      hetafr_FSR[1]->Fill(fabs(thisJet->Eta()),0.0);

       	      hptfr_Other[1]->Fill(thisJet->Pt(),0.0);
       	      hetafr_Other[1]->Fill(fabs(thisJet->Eta()),0.0);
       	    }
         
       	}
       else if(isFSR)
       	{

       	  hdR_FSR[0]->Fill(dR);	
       	  hdR_FSR[1]->Fill(dRmin);	

       	  hptfr_ISR[0]->Fill(thisJet->Pt(),0.0);
       	  hetafr_ISR[0]->Fill(fabs(thisJet->Eta()),0.0);

       	  hptfr_FSR[0]->Fill(thisJet->Pt(),1.0);
       	  hetafr_FSR[0]->Fill(fabs(thisJet->Eta()),1.0);

       	  hptfr_Other[0]->Fill(thisJet->Pt(),0.0);
       	  hetafr_Other[0]->Fill(fabs(thisJet->Eta()),0.0);

       	  if(nExtra==1)
       	    {

       	      hptfr_ISR[1]->Fill(thisJet->Pt(),0.0);
       	      hetafr_ISR[1]->Fill(fabs(thisJet->Eta()),0.0);

       	      hptfr_FSR[1]->Fill(thisJet->Pt(),1.0);
       	      hetafr_FSR[1]->Fill(fabs(thisJet->Eta()),1.0);

       	      hptfr_Other[1]->Fill(thisJet->Pt(),0.0);
       	      hetafr_Other[1]->Fill(fabs(thisJet->Eta()),0.0);

       	    }
         
       	}
       else
       	{

       	  hdR_Other[0]->Fill(dR);	
       	  hdR_Other[1]->Fill(dRmin);	

       	  hptfr_ISR[0]->Fill(thisJet->Pt(),0.0);
       	  hetafr_ISR[0]->Fill(fabs(thisJet->Eta()),0.0);

       	  hptfr_FSR[0]->Fill(thisJet->Pt(),0.0);
       	  hetafr_FSR[0]->Fill(fabs(thisJet->Eta()),0.0);

       	  hptfr_Other[0]->Fill(thisJet->Pt(),1.0);
       	  hetafr_Other[0]->Fill(fabs(thisJet->Eta()),1.0);

       	  if(nExtra==1)
       	    {

       	      hptfr_ISR[1]->Fill(thisJet->Pt(),0.0);
       	      hetafr_ISR[1]->Fill(fabs(thisJet->Eta()),0.0);

       	      hptfr_FSR[1]->Fill(thisJet->Pt(),0.0);
       	      hetafr_FSR[1]->Fill(fabs(thisJet->Eta()),0.0);

       	      hptfr_Other[1]->Fill(thisJet->Pt(),1.0);
       	      hetafr_Other[1]->Fill(fabs(thisJet->Eta()),1.0);
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

  for(int i=0; i<2; i++){
    hdR_ISR[i]->Write();
    hdR_FSR[i]->Write();
    hdR_Other[i]->Write();

    hptfr_ISR[i]->Write();
    hptfr_FSR[i]->Write();
    hptfr_Other[i]->Write();

    hetafr_ISR[i]->Write();
    hetafr_FSR[i]->Write();
    hetafr_Other[i]->Write();

  }


  outFile->Close();
}
