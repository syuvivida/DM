// example code to produce MET from Z->nunu

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
#define DEBUG 0
void xAna_metznn(std::string inputFile){

  //get TTree from file ...
  TString outputFile;
  std::vector<string> infiles;
  TString prefix;

  if(inputFile.find(".root")!= std::string::npos)
    { 
      prefix=gSystem->GetFromPipe(Form("file=%s; test=${file##*ZJetsNuNu/}; test2=${test%%/crab*}; echo \"${test2}\"",inputFile.data()));
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*NCUGlobalTuples_}; echo \"histo_%s_${test}\"",inputFile.data(),prefix.Data()));
      cout << "output file name = " << outputFile.Data() << endl;
      infiles.push_back(inputFile.data());
    }
  else
    {
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*ZJetsNuNu/}; test2=${test%%/crab*}; echo \"histo_${test2}_all.root\"",inputFile.data()));
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
  TH1F* h_pt = new TH1F("h_pt","",100,0,1000);
  h_pt->SetXTitle("#slash{E}_{T} [GeV]");

  TH1F* h_genmet = (TH1F*)h_pt->Clone("h_genmet");
  h_genmet->SetTitle("Generator-level before selections");

  TH1F* h_recmet = (TH1F*)h_pt->Clone("h_recmet");
  h_recmet->SetTitle("Reconstruction-level before selections");

  TH1F* h_trigmet = (TH1F*)h_pt->Clone("h_trigmet");
  h_trigmet->SetTitle("Reconstruction-level after trigger selections");

  TH1F* h_met = (TH1F*)h_pt->Clone("h_met");
  h_met->SetTitle("Reconstruction-level after all selections");

  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // first obtain the Z-momentum at the generator-level

    // 0. check the generator-level information and make sure there is a Z->nu nu
    Int_t nGenPar        = data.GetInt("nGenPar");
    Int_t* genParId      = data.GetPtrInt("genParId");
    Int_t* genParSt      = data.GetPtrInt("genParSt");
    Int_t* genMomParId   = data.GetPtrInt("genMomParId");

    int nuIndex[2]={-1,-1};

    // look for muon events first
    for(int ig=0; ig < nGenPar; ig++){

      if(genParSt[ig]!=1)continue;

      // all flavors of neutrinos
      if(abs(genParId[ig])!=12 && 
	 abs(genParId[ig])!=14 && 
	 abs(genParId[ig])!=16)continue;

      if(genMomParId[ig]!=23 &&
	 genMomParId[ig]!=genParId[ig])continue;

      if(nuIndex[0]==-1)	
	nuIndex[0]=ig;
      else if(nuIndex[1]==-1)
	nuIndex[1]=ig;

      if(nuIndex[0]>-1 && nuIndex[1]>-1)
	break;
    }

    if(nuIndex[0]==-1 || nuIndex[1]==-1)continue;
    nPass[0]++;


    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
    TLorentzVector nu_l4[2];

    for(int i=0; i<2; i++)
      nu_l4[i] = *((TLorentzVector*)genParP4->At(nuIndex[i]));
   

    if(DEBUG)
      for(int i=0; i<2; i++)
	nu_l4[i].Print();
    

    TLorentzVector z_l4=nu_l4[0]+nu_l4[1];
    h_genmet->Fill(z_l4.Pt());

    float met        = data.GetFloat("pfMetRawPt");
    h_recmet->Fill(met);

    //1. pass MET trigger
    std::string* trigName = data.GetPtrString("hlt_trigName");
    vector<bool> &trigResult = *((vector<bool>*) data.GetPtr("hlt_trigResult"));
    const Int_t nsize = data.GetPtrStringSize();

    bool passTrigger=false;
    for(int it=0; it< nsize; it++)
      {
 	std::string thisTrig= trigName[it];
 	bool results = trigResult[it];

	// std::cout << thisTrig << " : " << results << std::endl;
	
 	if( 
	   (thisTrig.find("HLT_PFMET170_NoiseCleaned")!= std::string::npos && results==1) || 
	   (thisTrig.find("HLT_PFMET120_PFMHT120_IDLoose_v")!= std::string::npos && results==1)  
	    )
 	  {
 	    passTrigger=true;
 	    break;
 	  }
	

      }


    if(!passTrigger)continue;
    nPass[1]++;

    h_trigmet->Fill(met);
    //3. has a good vertex
    Int_t nVtx        = data.GetInt("nVtx");
    if(nVtx<1)continue;
    nPass[2]++;

    //4. offline MET cut (not applied yet)

    
    h_met->Fill(met);
  } // end of loop over entries

  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;

  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  h_genmet->Write();
  h_recmet->Write();
  h_trigmet->Write();
  h_met->Write();
  outFile->Close();

}
