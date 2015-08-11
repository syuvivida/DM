// example code to run Bulk Graviton->ZZ->ZlepZhad selections on electron-channel

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TString.h>
#include <map>
#include <TH1D.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>

using namespace std;
void xAna_extra(std::string inputFile){
  TString outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*NCUGlobalTuples_}; echo \"histo_${test}\"",inputFile.data()));

  //get TTree from file ...
  TreeReader data(inputFile.data());
  TH1F* h_njet=new TH1F("h_njet","",11,-0.5,10.5);
  TH1F* h_njet30=new TH1F("h_njet30","",11,-0.5,10.5);
  Long64_t nTotal=0;
  Long64_t nPass[20]={0};
  TH1F* h_SD=new TH1F("h_SD","",100,0,200);
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

    genb_l4.Print();
    genbbar_l4.Print();

    // make sure there is an A0
    bool hasA0=false;
    TLorentzVector genn_l4(0,0,0,0);
    TLorentzVector gennbar_l4(0,0,0,0);

    for(int ig=0; ig < nGenPar; ig++){

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
    genn_l4.Print();
    gennbar_l4.Print();

    Int_t nAk4GenJet        = data.GetInt("nAk4GenJet");
    TClonesArray* ak4GenJetP4 = (TClonesArray*) data.GetPtrTObject("ak4GenJetP4");

    Int_t nExtra = 0; 
    Int_t nExtra30 = 0; 
    for(int ij=0; ij < nAk4GenJet; ij++){
      
      TLorentzVector* thisJet = (TLorentzVector*)ak4GenJetP4->At(ij);
       if(thisJet->DeltaR(genb_l4)<0.4)continue;
       if(thisJet->DeltaR(genbbar_l4)<0.4)continue;
       if(thisJet->DeltaR(genn_l4)<0.4)continue;
       if(thisJet->DeltaR(gennbar_l4)<0.4)continue;
       nExtra++;
       
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
  outFile->Close();
}
