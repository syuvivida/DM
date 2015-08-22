// example code to run Bulk Graviton->ZZ->ZlepZhad selections on muon-channel


#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TString.h>
#include <map>
#include <TH1.h>
#include <TH2.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TSystemDirectory.h>
#include <TList.h>

#define DEBUG 0

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
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*DYJetsHTBins25nsSamples/}; test2=${test%%/crab*}; echo \"histo_${test2}_all.root\"",
					   inputFile.data()));
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

  TH1F* h_dR=new TH1F("h_dR","",60,0,6);

  TH2F* h_genrec= new TH2F("h_genrec","",30,100,1000,30,100,1000); 
  TH2F* h_genrec_deno= (TH2F*)h_genrec->Clone("h_genrec_deno");
  TH2F* h_genrec_numr= (TH2F*)h_genrec->Clone("h_genrec_numr");

  TH1F* h_pt0 = new TH1F("h_pt0","",100,0,1000);
  h_pt0->SetXTitle("#slash{E}_{T} [GeV]");

  TH1F* h_pt = new TH1F("h_pt","",30,100,1000);
  h_pt->SetXTitle("#slash{E}_{T} [GeV]");

  TH1F* h_genmet = (TH1F*)h_pt0->Clone("h_genmet");
  h_genmet->SetTitle("Generator-level before selections");

  TH1F* h_genmet_pre = (TH1F*)h_pt->Clone("h_genmet_pre");
  h_genmet_pre->SetTitle("Generator-level after pre-selections");

  TH1F* h_trigmet = (TH1F*)h_pt->Clone("h_trigmet");
  h_trigmet->SetTitle("Reconstruction-level before trigger selections");

  TH1F* h_metold = (TH1F*)h_pt0->Clone("h_metold");
  h_metold->SetTitle("Reconstruction-level without muon contribution");

  TH1F* h_recmet_after     = (TH1F*)h_pt->Clone("h_recmet_after");
  h_recmet_after->SetTitle("Reconstruction-level Z momentum + residue missing momentum");

  TH1F* h_genmet_after = (TH1F*)h_pt->Clone("h_genmet_after");
  h_genmet_after->SetTitle("Generator-level after selections");

  TH1F* h_mz = new TH1F("h_mz","",50,50,150);
  TH1F* h_lpt[2];

  TH2F* h_genrec_deno_split[2];
  TH2F* h_genrec_numr_split[2];
  TH1F* h_genmet_split[2];
  TH1F* h_genmet_pre_split[2];
  TH1F* h_trigmet_split[2];
  TH1F* h_metold_split[2];
  TH1F* h_recmet_after_split[2];
  TH1F* h_genmet_after_split[2];
  TH1F* h_mz_split[2];

  for(int i=0; i<2; i++){
    h_lpt[i] = (TH1F*)h_pt0->Clone(Form("h_lpt%d",i));

    h_genrec_deno_split[i] = (TH2F*)h_genrec->Clone(Form("h_genrec_deno_split%d",i));
    h_genrec_numr_split[i] = (TH2F*)h_genrec->Clone(Form("h_genrec_numr_split%d",i));
    h_genmet_split[i] = (TH1F*)h_genmet->Clone(Form("h_genmet_split%d",i));
    h_genmet_pre_split[i] = (TH1F*)h_genmet_pre->Clone(Form("h_genmet_pre_split%d",i));
    h_trigmet_split[i] = (TH1F*)h_trigmet->Clone(Form("h_trigmet_split%d",i));
    h_metold_split[i] = (TH1F*)h_metold->Clone(Form("h_metold_split%d",i));
    h_recmet_after_split[i] = (TH1F*)h_recmet_after->Clone(Form("h_recmet_after_split%d",i));
    h_genmet_after_split[i] = (TH1F*)h_genmet_after->Clone(Form("h_genmet_after_split%d",i));
    h_mz_split[i] = (TH1F*)h_mz->Clone(Form("h_mz_split%d",i));

  }



  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
  
    int split=jEntry%2;
    
    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // 0. check the generator-level information and make sure there is a Z->mm
    Int_t nGenPar        = data.GetInt("nGenPar");
    Int_t* genParId      = data.GetPtrInt("genParId");
    Int_t* genParSt      = data.GetPtrInt("genParSt");
    Int_t* genMomParId   = data.GetPtrInt("genMomParId");

    
    int muIndex[2]={-1,-1};

    // look for muon events first
    for(int ig=0; ig < nGenPar; ig++){

      if(genParSt[ig]!=1)continue;

      if(abs(genParId[ig])!=13)continue;

      if(genMomParId[ig]!=23 &&
	 genMomParId[ig]!=genParId[ig])continue;

      if(muIndex[0]==-1)	
	muIndex[0]=ig;
      else if(muIndex[1]==-1)
	muIndex[1]=ig;

      if(muIndex[0]>-1 && muIndex[1]>-1)
	break;

    }

    if(muIndex[0]==-1 || muIndex[1]==-1)continue;
    nPass[0]++;

    
    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
    TLorentzVector mu_l4[2];

    for(int i=0; i<2; i++)
      mu_l4[i] = *((TLorentzVector*)genParP4->At(muIndex[i]));
   

    if(DEBUG)
      for(int i=0; i<2; i++)
	mu_l4[i].Print();
 

    TLorentzVector z_l4=mu_l4[0]+mu_l4[1];
    float genmet=z_l4.Pt();
    h_genmet->Fill(genmet);
    h_genmet_split[split]->Fill(genmet);

    if(genmet<100)continue;
    nPass[1]++;
    h_genmet_pre->Fill(genmet);
    h_genmet_pre_split[split]->Fill(genmet);

 

    // look for reco muons that are matched to gen-muons
    Int_t nMu          = data.GetInt("nMu");
    TClonesArray* muP4 = (TClonesArray*) data.GetPtrTObject("muP4");

    int recmuIndex[2]={-1,-1};

    for(int ip=0; ip<2; ip++){

      float ptmax=-99999;
      for(int im=0; im< nMu; im++)
	{
	  
	  TLorentzVector* thisMu = (TLorentzVector*)muP4->At(im);
	  float dR = thisMu->DeltaR(mu_l4[ip]);
	  h_dR->Fill(dR);
	  if(dR<0.2 && thisMu->Pt()>ptmax)
	    {
	      ptmax=thisMu->Pt();
	      recmuIndex[ip]=im;
	    }
	}
    }

    if(recmuIndex[0]==-1 || recmuIndex[1]==-1)
      {
	h_genrec_deno->Fill(genmet,95);
	h_genrec_deno_split[split]->Fill(genmet,95);
	continue;
      }
    
    nPass[2]++;

    TLorentzVector recZ_l4(0,0,0,0);
    TLorentzVector recMu_l4[2];
    for(int ip=0;ip<2;ip++)
      recMu_l4[ip]=(*(TLorentzVector*)muP4->At(recmuIndex[ip]));
    recZ_l4 = recMu_l4[0]+recMu_l4[1];

    float met        = data.GetFloat("pfMetRawPt");
    float metphi     = data.GetFloat("pfMetRawPhi");

    float metx = met*TMath::Cos(metphi);
    float mety = met*TMath::Sin(metphi);
    
    metx += recZ_l4.Px();
    mety += recZ_l4.Py();

    float llmet = sqrt(metx*metx + mety*mety);
    h_genrec_deno->Fill(genmet,llmet);
    h_genrec_deno_split[split]->Fill(genmet,llmet);

    //3. has a good vertex
    Int_t nVtx        = data.GetInt("nVtx");
    if(nVtx<1)continue;
    nPass[3]++;


    // saving myMuons
    vector<bool> &isHighPtMuon = *((vector<bool>*) data.GetPtr("isHighPtMuon"));
    vector<bool> &isCustomTrackerMuon = *((vector<bool>*) data.GetPtr("isCustomTrackerMuon"));
    vector<bool> &isLooseMuon = *((vector<bool>*) data.GetPtr("isLooseMuon"));
    Float_t* muMiniIso       = data.GetPtrFloat("muMiniIso");
    std::vector<int> myMuons;
    for(int ip=0; ip< 2; ip++)
      {
	int im=recmuIndex[ip];
    	TLorentzVector* thisMu = (TLorentzVector*)muP4->At(im);

    	if(fabs(thisMu->Eta())>2.1)continue;

    	if(thisMu->Pt() < 10)continue;

    	if(!isLooseMuon[im])continue;
    	
    	if(muMiniIso[im]>0.1)continue;

    	myMuons.push_back(im);
      }

    if(myMuons.size()<2)continue;
    nPass[4]++;


   //6. select a good Z boson

    Float_t mll  = recZ_l4.M();

    h_mz->Fill(mll);
    h_mz_split[split]->Fill(mll);
	    
    if(mll<70 || mll>110)continue;

    float ptmax =  TMath::Max(recMu_l4[0].Pt(),recMu_l4[1].Pt());
    float ptmin =  TMath::Min(recMu_l4[0].Pt(),recMu_l4[1].Pt());

    // leading pt must be larger than 50 GeV
    if(ptmax<50)continue;

    Float_t ptll = recZ_l4.Pt();
    if(ptll<100)continue;

    h_lpt[0]->Fill(ptmax);
    h_lpt[1]->Fill(ptmin);

    nPass[5]++;

    h_trigmet->Fill(llmet);
    h_trigmet_split[split]->Fill(llmet);

    //2. pass electron or muon trigger
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
	   (thisTrig.find("HLT_Mu45")!= std::string::npos && results==1)
	    )
 	  {
 	    passTrigger=true;
 	    break;
 	  }
	

      }

    
    if(!passTrigger)continue;
    nPass[6]++;

    
    h_genmet_after->Fill(genmet);
    h_recmet_after->Fill(llmet);
    h_metold->Fill(met);
    h_genrec_numr->Fill(genmet,llmet);

    h_genmet_after_split[split]->Fill(genmet);
    h_recmet_after_split[split]->Fill(llmet);
    h_metold_split[split]->Fill(met);
    h_genrec_numr_split[split]->Fill(genmet,llmet);


  } // end of loop over entries

  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;

  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  h_dR->Write();
  h_genrec_deno->Write();
  h_genrec_numr->Write();
  h_genmet->Write();
  h_genmet_pre->Write();
  h_trigmet->Write();
  h_genmet_after->Write();
  h_recmet_after->Write();
  h_metold->Write();
  h_mz->Write();
  for(int i=0;i<2;i++)
    {
      h_lpt[i]->Write();
      h_genrec_deno_split[i]->Write();
      h_genrec_numr_split[i]->Write();
      h_genmet_split[i]->Write();
      h_genmet_pre_split[i]->Write();
      h_trigmet_split[i]->Write();
      h_metold_split[i]->Write();
      h_recmet_after_split[i]->Write();
      h_genmet_after_split[i]->Write();
      h_mz_split[i]->Write();
    }
    
  outFile->Close();

}
