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

const float mzmin=70;
const float mzmax=110;
const float ptmaxcut=50;
const float ptmincut=10;
const float etamaxcut=2.1;
const float miniisocut=0.1;
const int nvtxmin=1;
const float recmetcut=150;

using namespace std;
// mode:0 variable bin width, 24 bins
// mode:1 fixed bin width, 17 bins
void xAna_metzmm(std::string inputFile, int mode=0){

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
      // check if it's data first
      if(inputFile.find("Run2015")!= std::string::npos)
	outputFile="histo_singlemuon.root";
      else if(inputFile.find("TT")!= std::string::npos)
	outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*SPRING15/}; test2=${test%%_0803*}; echo \"histo_${test2}_all.root\"",
					     inputFile.data()));	
      else
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

  std::vector<float> xLow;
  for(int i=0;i<=14;i++)
    xLow.push_back(150+i*25);
  for(int i=1;i<=10;i++)
    xLow.push_back(xLow[14]+i*50);
  
  const int nbins = xLow.size()-1;
  float xBin[nbins+1];
  for(unsigned int i=0; i < xLow.size(); i++)
    {
      cout << "xLow = " << xLow[i] << endl;
      xBin[i]=xLow[i];
    }

  TH2F* h_genrec;
  TH1F* h_pt;
  if(mode==0)
    {
      h_genrec= new TH2F("h_genrec","",nbins,xBin,nbins,xBin); 
      h_pt= new TH1F("h_pt","",nbins,xBin);
    }
  else if(mode==1)
    {
      const int nfixbins=17;
      const float xmin=150;
      const float xmax=1000;
      h_genrec= new TH2F("h_genrec","",nfixbins,xmin,xmax,nfixbins,xmin,xmax); 
      h_pt = new TH1F("h_pt","", nfixbins,xmin,xmax);
    }
      
  else // wrong mode
    return;
  h_genrec->SetXTitle("generator-level");
  h_genrec->SetYTitle("reconstruction-level");
  h_pt->SetXTitle("#slash{E}_{T} [GeV]");

  TH2F* h_genrec_deno= (TH2F*)h_genrec->Clone("h_genrec_deno");
  TH2F* h_genrec_numr= (TH2F*)h_genrec->Clone("h_genrec_numr");

  TH1F* h_pt0 = new TH1F("h_pt0","",100,0,1000);
  h_pt0->SetXTitle("#slash{E}_{T} [GeV]");


  
  // MC-like histograms
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
  h_mz->SetXTitle("M_{#ell#ell} [GeV]");
  TH1F* h_lpt[2];

  // data-like-histograms
  TH1F* h_metold_data = (TH1F*)h_metold->Clone("h_metold_data");
  TH1F* h_recmet_after_data = (TH1F*)h_recmet_after->Clone("h_recmet_after_data");

  TH1F* h_mz_data = (TH1F*)h_mz->Clone("h_mz_data");
  TH1F* h_lpt_data[2];

  TH1F* h_metold_split_data[2];
  TH1F* h_recmet_after_split_data[2];
  TH1F* h_mz_split_data[2];

  // MC-like histograms
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
    h_lpt_data[i] = (TH1F*)h_pt0->Clone(Form("h_lpt_data%d",i));

    h_genrec_deno_split[i] = (TH2F*)h_genrec->Clone(Form("h_genrec_deno_split%d",i));
    h_genrec_numr_split[i] = (TH2F*)h_genrec->Clone(Form("h_genrec_numr_split%d",i));
    h_genmet_split[i] = (TH1F*)h_genmet->Clone(Form("h_genmet_split%d",i));
    h_genmet_pre_split[i] = (TH1F*)h_genmet_pre->Clone(Form("h_genmet_pre_split%d",i));
    h_trigmet_split[i] = (TH1F*)h_trigmet->Clone(Form("h_trigmet_split%d",i));
    h_metold_split[i] = (TH1F*)h_metold->Clone(Form("h_metold_split%d",i));
    h_recmet_after_split[i] = (TH1F*)h_recmet_after->Clone(Form("h_recmet_after_split%d",i));
    h_genmet_after_split[i] = (TH1F*)h_genmet_after->Clone(Form("h_genmet_after_split%d",i));
    h_mz_split[i] = (TH1F*)h_mz->Clone(Form("h_mz_split%d",i));

    h_metold_split_data[i] = (TH1F*)h_metold->Clone(Form("h_metold_split_data%d",i));
    h_recmet_after_split_data[i] = (TH1F*)h_recmet_after->Clone(Form("h_recmet_after_split_data%d",i));
    h_mz_split_data[i] = (TH1F*)h_mz->Clone(Form("h_mz_split_data%d",i));


  }



  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
  
    int split=jEntry%2;
    
    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // obtain data-like figures first; do not skip events in this part
    // there will be many ugly curly braces
    Int_t nVtx        = data.GetInt("nVtx");
    float met        = data.GetFloat("pfMetRawPt");
    float metphi     = data.GetFloat("pfMetRawPhi");    
    float metx = met*TMath::Cos(metphi);
    float mety = met*TMath::Sin(metphi);
    Int_t nMu          = data.GetInt("nMu");
    TClonesArray* muP4 = (TClonesArray*) data.GetPtrTObject("muP4");
    vector<bool> &isHighPtMuon = *((vector<bool>*) data.GetPtr("isHighPtMuon"));
    vector<bool> &isCustomTrackerMuon = *((vector<bool>*) data.GetPtr("isCustomTrackerMuon"));
    vector<bool> &isLooseMuon = *((vector<bool>*) data.GetPtr("isLooseMuon"));
    Float_t* muMiniIso       = data.GetPtrFloat("muMiniIso");
    Int_t* muCharge = data.GetPtrInt("muCharge");

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


    if(nVtx>=nvtxmin && passTrigger){
      nPass[10]++;
      std::vector<int> goodMuons;

      for(int im=0; im< nMu; im++)
	{

	  TLorentzVector* thisMu = (TLorentzVector*)muP4->At(im);

	  if(fabs(thisMu->Eta())>etamaxcut)continue;

	  if(thisMu->Pt() < ptmincut)continue;

	  if(!isLooseMuon[im])continue;
    	
	  if(muMiniIso[im]>miniisocut)continue;

	  goodMuons.push_back(im);
	}

      bool findMPair=false;
      TLorentzVector dataZ_l4(0,0,0,0);

      for(unsigned int i=0; i< goodMuons.size(); i++)
      {
	int im = goodMuons[i];
	TLorentzVector* thisMu = (TLorentzVector*)muP4->At(im);

	for(unsigned int j=0; j< i; j++)
	  {
	    int jm= goodMuons[j];
	    
	    if(muCharge[im]*muCharge[jm]>0)continue;

	    TLorentzVector* thatMu = (TLorentzVector*)muP4->At(jm);

	    Float_t mll  = (*thisMu+*thatMu).M();
	    Float_t ptll = (*thisMu+*thatMu).Pt();

	    if(mll<mzmin || mll>mzmax)continue;

	    float ptmax =  TMath::Max(thisMu->Pt(),thatMu->Pt());
	    float ptmin =  TMath::Min(thisMu->Pt(),thatMu->Pt());

	    // leading pt must be larger than 50 GeV
	    if(ptmax< ptmaxcut)continue;

	    if(!findMPair){
	      dataZ_l4=(*thisMu+*thatMu);

	      h_lpt_data[0]->Fill(ptmax);
	      h_lpt_data[1]->Fill(ptmin);
	    }

	    findMPair=true;
	    break;
	  }	
      } // end of double loop

    if(findMPair)
      {
	nPass[11]++;

	float llmet_data = sqrt( pow(metx+dataZ_l4.Px(),2) + 
				 pow(mety+dataZ_l4.Py(),2));
	
	if(llmet_data>recmetcut){
	  nPass[12]++;
	  h_mz_data->Fill(dataZ_l4.M());
	  h_mz_split_data[split]->Fill(dataZ_l4.M());
	  h_metold_data ->Fill(met);
	  h_metold_split_data[split]->Fill(met);
	  h_recmet_after_data->Fill(llmet_data);
	  h_recmet_after_split_data[split]->Fill(llmet_data);
	}

      }

    } //if nVtx>1
    

    // first obtain the Z-momentum at the generator-level
    bool isData     = data.GetBool("isData");

    if(!isData){
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

      float llmet = sqrt( pow(metx+recZ_l4.Px(),2) + 
			  pow(mety+recZ_l4.Py(),2));
      h_genrec_deno->Fill(genmet,llmet);
      h_genrec_deno_split[split]->Fill(genmet,llmet);

      //3. has a good vertex
      if(nVtx<1)continue;
      nPass[3]++;


      // saving myMuons
      std::vector<int> myMuons;
      for(int ip=0; ip< 2; ip++)
	{
	  int im=recmuIndex[ip];
	  TLorentzVector* thisMu = (TLorentzVector*)muP4->At(im);

	  if(fabs(thisMu->Eta())> etamaxcut)continue;

	  if(thisMu->Pt() < ptmincut)continue;

	  if(!isLooseMuon[im])continue;
    	
	  if(muMiniIso[im]>miniisocut)continue;

	  myMuons.push_back(im);
	}

      if(myMuons.size()<2)continue;
      nPass[4]++;


      //6. select a good Z boson

      Float_t mll  = recZ_l4.M();

      h_mz->Fill(mll);
      h_mz_split[split]->Fill(mll);
	    
      if(mll< mzmin || mll> mzmax)continue;
      nPass[5]++;

      float ptmax =  TMath::Max(recMu_l4[0].Pt(),recMu_l4[1].Pt());
      float ptmin =  TMath::Min(recMu_l4[0].Pt(),recMu_l4[1].Pt());

      // leading pt must be larger than 50 GeV
      if(ptmax< ptmaxcut)continue;
      nPass[6]++;

      h_lpt[0]->Fill(ptmax);
      h_lpt[1]->Fill(ptmin);

      h_trigmet->Fill(llmet);
      h_trigmet_split[split]->Fill(llmet);

    
      if(!passTrigger)continue;
      nPass[7]++;


      h_genrec_numr->Fill(genmet,llmet);
      h_genrec_numr_split[split]->Fill(genmet,llmet);

      h_metold->Fill(met);
      h_metold_split[split]->Fill(met);

      if(llmet< recmetcut)continue;
      nPass[8]++;

      h_genmet_after->Fill(genmet);
      h_genmet_after_split[split]->Fill(genmet);

      h_recmet_after->Fill(llmet);
      h_recmet_after_split[split]->Fill(llmet);
    }

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

  h_metold_data->Write();
  h_recmet_after_data->Write();
  h_mz_data->Write();

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

      h_lpt_data[i]->Write();
      h_metold_split_data[i]->Write();
      h_recmet_after_split_data[i]->Write();
      h_mz_split_data[i]->Write();

    }
    
  outFile->Close();

}
