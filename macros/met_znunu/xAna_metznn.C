// example code to produce MET from Z->nunu

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
const int nvtxmin=1;
const float recmetcut=150;

using namespace std;
#define DEBUG 0
// mode:0 variable bin width, 24 bins
// mode:1 fixed bin width, 17 bins
void xAna_metznn(std::string inputFile,int mode=0){

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
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*ZJetsNuNu/}; test2=${test%%/crab*}; echo \"histo_${test2}_all.root\"",
					   inputFile.data()));
      cout << "output file name = " << outputFile.Data() << endl;      

      TSystemDirectory *base = new TSystemDirectory("root","root");

      base->SetDirectory(inputFile.data());
      TList *listOfFiles = base->GetListOfFiles();
      TIter fileIt(listOfFiles);
      TFile *fileH = new TFile();
      Long64_t nfiles=0;
      while(fileH = (TFile*)fileIt()) {
	std::string fileN = fileH->GetName();
	std::string baseString = "NCUGlobal"; 
	// cout << fileN << endl;
	if(fileN.find("fail") != std::string::npos)continue;
	if(fileH->IsFolder()){
	  std::string newDir=inputFile+fileN;
	  base->SetDirectory(newDir.data());
	  TList *listOfFiles2 = base->GetListOfFiles();
	  TIter fileIt2(listOfFiles2);
	  TFile *fileH2 = new TFile();  
	  while(fileH2 = (TFile*)fileIt2()) {
	    std::string fileN2 = fileH2->GetName();
	    // cout << fileN2 << endl;
	    if(fileH2->IsFolder())continue;
	    if(fileN2.find("fail") != std::string::npos)continue;
	    if(fileN2.find(baseString) == std::string::npos)continue;
	    // cout << fileN2.data() << endl;
	    infiles.push_back(Form("%s/%s",newDir.data(),fileN2.data()));
	    nfiles++;
	  }
	} 

      }

      std::cout << "Opened " << nfiles << " files" << std::endl;

    }
  
  TreeReader data(infiles);


  Long64_t nTotal=0;
  Long64_t nPass[20]={0};
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

  TH1F* h_genmet = (TH1F*)h_pt0->Clone("h_genmet");
  h_genmet->SetTitle("Generator-level before selections");

  TH1F* h_recmet = (TH1F*)h_pt0->Clone("h_recmet");
  h_recmet->SetTitle("Reconstruction-level before selections");

  TH1F* h_genmet_pre = (TH1F*)h_pt->Clone("h_genmet_pre");
  h_genmet_pre->SetTitle("Generator-level after pre-selections");

  TH1F* h_recmet_pre = (TH1F*)h_pt->Clone("h_recmet_pre");
  h_recmet_pre->SetTitle("Reconstruction-level after pre-selections");

  TH1F* h_trigmet = (TH1F*)h_pt->Clone("h_trigmet");
  h_trigmet->SetTitle("Reconstruction-level before trigger selections");

  TH1F* h_genmet_after = (TH1F*)h_pt->Clone("h_genmet_after");
  h_genmet_after->SetTitle("Generator-level after selections");

  TH1F* h_recmet_after = (TH1F*)h_pt->Clone("h_recmet_after");
  h_recmet_after->SetTitle("Reconstruction-level after all selections");

  TH1F* h_recmet_after_data = (TH1F*)h_pt->Clone("h_recmet_after_data");
  h_recmet_after_data->SetTitle("Reconstruction-level after all selections");


  TH2F* h_genrec_deno_split[2];
  TH2F* h_genrec_numr_split[2];
  TH1F* h_genmet_split[2];
  TH1F* h_recmet_split[2];
  TH1F* h_genmet_pre_split[2];
  TH1F* h_recmet_pre_split[2];
  TH1F* h_trigmet_split[2];
  TH1F* h_genmet_after_split[2];
  TH1F* h_recmet_after_split[2];
  TH1F* h_recmet_after_split_data[2];

  for(int i=0;i<2;i++)
    {

      h_genrec_deno_split[i]=(TH2F*)h_genrec->Clone(Form("h_genrec_deno_split%d",i));
      h_genrec_numr_split[i]=(TH2F*)h_genrec->Clone(Form("h_genrec_numr_split%d",i));
      h_genmet_split[i]=(TH1F*)h_genmet->Clone(Form("h_genmet_split%d",i));
      h_recmet_split[i]=(TH1F*)h_recmet->Clone(Form("h_recmet_split%d",i));
      h_genmet_pre_split[i]=(TH1F*)h_genmet_pre->Clone(Form("h_genmet_pre_split%d",i));
      h_recmet_pre_split[i]=(TH1F*)h_recmet_pre->Clone(Form("h_recmet_pre_split%d",i));
      h_trigmet_split[i]=(TH1F*)h_trigmet->Clone(Form("h_trigmet_split%d",i));
      h_genmet_after_split[i]=(TH1F*)h_genmet_after->Clone(Form("h_genmet_after_split%d",i));
      h_recmet_after_split[i]=(TH1F*)h_recmet_after->Clone(Form("h_recmet_after_split%d",i));

      h_recmet_after_split_data[i]=(TH1F*)h_recmet_after->Clone(Form("h_recmet_after_split_data%d",i));

    }

  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    int split = jEntry%2;
    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // first obtain the Z-momentum at the generator-level
    bool isData     = data.GetBool("isData");
    //1. has a good vertex
    Int_t nVtx        = data.GetInt("nVtx");
    float met        = data.GetFloat("pfMetRawPt");
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

    // data-like histograms
    if(nVtx>=nvtxmin && passTrigger && met>recmetcut)
      {
	nPass[10]++;
	h_recmet_after_data->Fill(met);
	h_recmet_after_split_data[split]->Fill(met);	
      }
    
    int nuIndex[2]={-1,-1};
    float genmet=999999.;
    if(!isData){
      // 0. check the generator-level information and make sure there is a Z->nu nu
      Int_t nGenPar        = data.GetInt("nGenPar");
      Int_t* genParId      = data.GetPtrInt("genParId");
      Int_t* genParSt      = data.GetPtrInt("genParSt");
      Int_t* genMomParId   = data.GetPtrInt("genMomParId");


      // look for neutrino events first, a dummy check, 100% efficieincy
      // meant to compute generator-level Z momentum

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
      genmet = z_l4.Pt();
      h_genmet->Fill(genmet);
      h_genmet_split[split]->Fill(genmet);
    } // if is MC sample

    h_recmet->Fill(met);
    h_recmet_split[split]->Fill(met);

    if(genmet<100)continue;
    nPass[1]++;

    if(!isData){
      h_genrec_deno->Fill(genmet,met);
      h_genrec_deno_split[split]->Fill(genmet,met);
      h_genmet_pre->Fill(genmet);
      h_genmet_pre_split[split]->Fill(genmet);
    }

    h_recmet_pre->Fill(met);
    h_recmet_pre_split[split]->Fill(met);

    if(nVtx<nvtxmin)continue;
    nPass[2]++;

    //2. offline MET cut (not applied yet)

    h_trigmet->Fill(met);

    h_trigmet_split[split]->Fill(met);

    if(!passTrigger)continue;
    nPass[3]++;

    if(!isData){
      h_genmet_after->Fill(genmet);    
      h_genmet_after_split[split]->Fill(genmet);    
      h_genrec_numr->Fill(genmet,met);
      h_genrec_numr_split[split]->Fill(genmet,met);
    }

    if(met<recmetcut)continue;
    nPass[4]++;

    h_recmet_after->Fill(met);
    h_recmet_after_split[split]->Fill(met);
 

  } // end of loop over entries

  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;

  TFile* outFile = new TFile(outputFile.Data(),"recreate");

  h_genrec_deno->Write();
  h_genrec_numr->Write();
  h_genmet->Write();
  h_genmet_pre->Write();
  h_genmet_after->Write();
  h_recmet->Write();
  h_recmet_pre->Write();
  h_trigmet->Write();
  h_recmet_after->Write();
  h_recmet_after_data->Write();

  for(int i=0;i<2;i++){
    h_genrec_deno_split[i]->Write();
    h_genrec_numr_split[i]->Write();
    h_genmet_split[i]->Write();
    h_genmet_pre_split[i]->Write();
    h_genmet_after_split[i]->Write();

    h_recmet_split[i]->Write();
    h_recmet_pre_split[i]->Write();
    h_trigmet_split[i]->Write();
    h_recmet_after_split[i]->Write();
    h_recmet_after_split_data[i]->Write();
  }

  outFile->Close();

}
