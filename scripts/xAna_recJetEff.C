// example code to run Bulk Graviton->ZZ->ZlepZhad selections on electron-channel

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <TString.h>
#include <map>
#include <TH1D.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TEfficiency.h>

using namespace std;
void xAna_recJetEff(std::string inputFile){

  TString outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*/}; echo \"eff_${test}\"",inputFile.data()));
  cout << "output file name = " << outputFile.Data() << endl;
  //get TTree from file ...
  TreeReader data(inputFile.data());
  
  Long64_t nTotal=0;
  Long64_t nPass[20]={0};
  TH1F* h_pt=new TH1F("h_pt","",125,0,2500);
  h_pt->SetXTitle("Higgs p_{T} [GeV]");

  TH1F* h_deno=(TH1F*)h_pt->Clone("h_deno");
  TH1F* h_numrFAT=(TH1F*)h_pt->Clone("h_numrFAT");
  TH1F* h_numrTHIN=(TH1F*)h_pt->Clone("h_numrTHIN");

  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // 0. check the generator-level information and make sure there is a Z->e+e-
    Int_t nGenPar        = data.GetInt("nGenPar");
    Int_t* genParId      = data.GetPtrInt("genParId");
    // Int_t* genParSt      = data.GetPtrInt("genParSt");
    Int_t* genDa1      = data.GetPtrInt("genDa1");
    Int_t* genDa2      = data.GetPtrInt("genDa2");

    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");


    // 1. make sure there is a H->bb
    bool hasHadron=false;
    int bindex[2]={-1,-1};
    int higgsIndex=-1;

    for(int ig=0; ig < nGenPar; ig++){

      if(genParId[ig]!=25)continue;
      int da1=genDa1[ig];
      int da2=genDa2[ig];

      if(da1<0 || da2<0)continue;
      int da1pdg = genParId[da1];
      int da2pdg = genParId[da2];

      if(abs(da1pdg)!=5)
	continue;
      if(abs(da2pdg)!=5)
	continue;
      
      higgsIndex=ig;
      bindex[0]=da1;
      bindex[1]=da2;

      hasHadron=true;
      if(hasHadron)break;

    }
    
    if(!hasHadron)continue;
    nPass[0]++;


    TLorentzVector* higgs_p4 = (TLorentzVector*)genParP4->At(higgsIndex);    
    h_deno->Fill(higgs_p4->Pt());

    TLorentzVector* b_p4[2];
    for(int i=0;i<2;i++)
      b_p4[i]= (TLorentzVector*)genParP4->At(bindex[i]);    

    // for(int i=0;i<2;i++)
    //   std::cout << "b index " << i <<  " = " << bindex[i] << endl;
     
    Int_t nFATJet         = data.GetInt("FATnJet");
    TClonesArray* fatjetP4 = (TClonesArray*) data.GetPtrTObject("FATjetP4");

    float dRMax=0.8;
    bool findAFATJet=false;

    for(int ij=0; ij<nFATJet; ij++)
      {
    	
     	TLorentzVector* thisJet = (TLorentzVector*)fatjetP4->At(ij);
	if(thisJet->Pt()<30)continue;
	if(fabs(thisJet->Eta())>2.5)continue;
	
	if(thisJet->DeltaR(*b_p4[0]) > dRMax)continue;
	if(thisJet->DeltaR(*b_p4[1]) > dRMax)continue;

	findAFATJet=true;

	if(findAFATJet)break;
	
      }
    
    if(findAFATJet)
      {
	h_numrFAT->Fill(higgs_p4->Pt());
	nPass[1]++;
      }

 
    Int_t nTHINJet         = data.GetInt("THINnJet");
    TClonesArray* thinjetP4 = (TClonesArray*) data.GetPtrTObject("THINjetP4");

    dRMax=0.4;
    bool findATHINJet[2]={false,false};

    for(int ij=0; ij<nTHINJet; ij++)
      {
    	
     	TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(ij);
	if(thisJet->Pt()<30)continue;
	if(fabs(thisJet->Eta())>2.5)continue;
	
	// exclude when the jet is matched to both
	if(thisJet->DeltaR(*b_p4[0]) < dRMax && thisJet->DeltaR(*b_p4[1]) < dRMax)continue;
	else if(thisJet->DeltaR(*b_p4[0]) < dRMax && thisJet->DeltaR(*b_p4[1]) > dRMax)
	  findATHINJet[0]=true;

	else if(thisJet->DeltaR(*b_p4[0]) > dRMax && thisJet->DeltaR(*b_p4[1]) < dRMax)
	  findATHINJet[1]=true;
	    
	if(findATHINJet[0] && findATHINJet[1])break;

	
      }
    
    if(findATHINJet[0] && findATHINJet[1])
      {
	h_numrTHIN->Fill(higgs_p4->Pt());
	nPass[2]++;
      }
    
   

  } // end of loop over entries
  TEfficiency* fEff = new TEfficiency(*h_numrFAT,*h_deno);
  fEff->SetName("fatJetEff");
  fEff->SetTitle("AK8 jet efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* tEff = new TEfficiency(*h_numrTHIN,*h_deno);
  tEff->SetName("thinJetEff");
  tEff->SetTitle("Two AK4 jet efficiency; Higgs p_{T} [GeV]; Efficiency");

  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  h_deno->Write();
  h_numrFAT->Write();
  h_numrTHIN->Write();
  fEff->Write();
  tEff->Write(); 

  outFile->Close();
  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;
    
  
  

}
