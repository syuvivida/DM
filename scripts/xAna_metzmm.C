// example code to run Bulk Graviton->ZZ->ZlepZhad selections on muon-channel


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
#include <TSystemDirectory.h>
#include <TList.h>

using namespace std;
void xAna_metzmm(std::string inputFile){

  //get TTree from file ...

  TString outputFile;
  std::vector<string> infiles;
  TString prefix;

  if(inputFile.find(".root")!= std::string::npos)
    { 
      prefix=gSystem->GetFromPipe(Form("file=%s; test=${file##*DYJetsHTBins25nsSamples/}; test2=${test%%/crab*}; echo \"${test2}\"",inputFile.data()));
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*NCUGlobalTuples_}; echo \"histo_%s_${test}\"",inputFile.data(),prefix.Data()));
      cout << "output file name = " << outputFile.Data() << endl;
      infiles.push_back(inputFile.data());
    }
  else
    {
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*DYJetsHTBins25nsSamples/}; test2=${test%%/crab*}; echo \"histo_${test2}_all.root\"",inputFile.data()));
      cout << "output file name = " << outputFile.Data() << endl;      
      TSystemDirectory *base = new TSystemDirectory("root","root");

      base->SetDirectory(inputFile.data());
      TList *listOfFiles = base->GetListOfFiles();
      TIter fileIt(listOfFiles);
      TFile *fileH = new TFile();
      int nfile=0;
      while(fileH = (TFile*)fileIt()) {
	std::string fileN = fileH->GetName();
	if( fileH->IsFolder())  continue;
	if(fileN.find("NCUGlobalTuples") == std::string::npos)continue;
	fileN = inputFile + "/" + fileN;
	cout << fileN.data() << endl;
	nfile++;
	infiles.push_back(fileN);
      }

      std::cout << "Opened " << nfile << " files" << std::endl;

    }
  
  TreeReader data(infiles);



  Long64_t nTotal=0;
  Long64_t nPass[20]={0};

  TH1F* h_pt = new TH1F("h_pt","",50,0,500);
  TH1F* h_metold = (TH1F*)h_pt->Clone("h_metold");
  TH1F* h_met = (TH1F*)h_pt->Clone("h_met");
  TH1F* h_mz = new TH1F("h_mz","",50,50,150);
  TH1F* h_lpt[2];
  for(int i=0; i<2; i++){
    h_lpt[i] = (TH1F*)h_pt->Clone(Form("h_lpt%d",i));
  }

  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // 0. check the generator-level information and make sure there is a Z->mm
    Int_t nGenPar        = data.GetInt("nGenPar");
    Int_t* genParId      = data.GetPtrInt("genParId");
    Int_t* genParSt      = data.GetPtrInt("genParSt");
    Int_t* genMomParId   = data.GetPtrInt("genMomParId");
    Int_t* genDa1      = data.GetPtrInt("genDa1");
    Int_t* genDa2      = data.GetPtrInt("genDa2");


    bool hasLepton=false;

    // look for muon events first
    for(int ig=0; ig < nGenPar; ig++){

      if(genParSt[ig]>30)continue;

      if(abs(genParId[ig])!=13)continue;

      if(genMomParId[ig]!=23 &&
	 genMomParId[ig]!=genParId[ig])continue;

      hasLepton=true;
      if(hasLepton)break;

    }

    if(!hasLepton)continue;


    nPass[0]++;

    //3. has a good vertex
    Int_t nVtx        = data.GetInt("nVtx");
    if(nVtx<1)continue;
    nPass[1]++;

    //4. look for mymuons first
    Int_t nMu          = data.GetInt("nMu");
    Int_t run          = data.GetInt("runId");
    Int_t lumi         = data.GetInt("lumiSection");
    Int_t event        = data.GetInt("eventId");
    vector<bool> &isHighPtMuon = *((vector<bool>*) data.GetPtr("isHighPtMuon"));
    vector<bool> &isCustomTrackerMuon = *((vector<bool>*) data.GetPtr("isCustomTrackerMuon"));
    vector<bool> &isLooseMuon = *((vector<bool>*) data.GetPtr("isLooseMuon"));
    TClonesArray* muP4 = (TClonesArray*) data.GetPtrTObject("muP4");
    Float_t* muMiniIso       = data.GetPtrFloat("muMiniIso");

    //5. select  mymuons
    std::vector<int> myMuons;

    for(int im=0; im< nMu; im++)
      {

    	TLorentzVector* thisMu = (TLorentzVector*)muP4->At(im);

    	if(fabs(thisMu->Eta())>2.1)continue;

    	if(thisMu->Pt() < 10)continue;

    	if(!isLooseMuon[im])continue;
    	
    	if(muMiniIso[im]>0.1)continue;

    	myMuons.push_back(im);
      }


   //6. select a good Z boson
    bool findMPair=false;
    TLorentzVector l4_Z(0,0,0,0);

    for(unsigned int i=0; i< myMuons.size(); i++)
      {
	int im = myMuons[i];
	TLorentzVector* thisMu = (TLorentzVector*)muP4->At(im);

	for(unsigned int j=0; j< i; j++)
	  {
	    int jm= myMuons[j];

	    // if(muCharge[im]*muCharge[jm]>0)continue;

	    TLorentzVector* thatMu = (TLorentzVector*)muP4->At(jm);

	    Float_t mll  = (*thisMu+*thatMu).M();
	    Float_t ptll = (*thisMu+*thatMu).Pt();

	    h_mz->Fill(mll);
	    
	    if(mll<70 || mll>110)continue;
	    if(!findMPair){
	      l4_Z=(*thisMu+*thatMu);

	      float ptmax =  TMath::Max(thisMu->Pt(),thatMu->Pt());
	      float ptmin =  TMath::Min(thisMu->Pt(),thatMu->Pt());

	      h_lpt[0]->Fill(ptmax);
	      h_lpt[1]->Fill(ptmin);
	    }

	    findMPair=true;
	  }	
      }

    if(!findMPair)
      continue;
    nPass[2]++;

    float met        = data.GetFloat("pfMetRawPt");
    float metphi     = data.GetFloat("pfMetRawPhi");

    float metx = met*TMath::Cos(metphi);
    float mety = met*TMath::Sin(metphi);
    
    metx += l4_Z.Px();
    mety += l4_Z.Py();

    float llmet = sqrt(metx*metx + mety*mety);

    h_metold->Fill(met);
    h_met->Fill(llmet);

  } // end of loop over entries

  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;

  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  h_met->Write();
  h_metold->Write();
  h_mz->Write();
  for(int i=0;i<2;i++)
    h_lpt[i]->Write();
    
  outFile->Close();

}
