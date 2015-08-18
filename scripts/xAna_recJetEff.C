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
#include <TGraphAsymmErrors.h>

using namespace std;
void xAna_recJetEff(std::string inputFile, bool myDefinition=true){

  std::vector<string> infiles;
  bool readOneFile=true;
  TString outputFile;
  TString myName=myDefinition? "myDefinition":"JETMET";
  if(inputFile.find(".txt")!= std::string::npos)
    {
      readOneFile=false;
      TString endfix=gSystem->GetFromPipe(Form("file=%s; test=${file##*/}; echo \"${test%%.txt*}\"",inputFile.data()));
      outputFile = "eff_" + myName + "_all.root";
    }

  if(readOneFile)
    {
      infiles.push_back(inputFile);
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*/}; echo \"eff_%s_${test}\"",inputFile.data(),myName.Data()));
    }
  else{
    FILE *fTable = fopen(inputFile.data(),"r");
    int flag=1;
    int nfile=0;

    while (flag!=-1){
      char filename[300];
      flag=fscanf(fTable,"%s",filename);
      // first reading input file
      if(flag!=-1){
	std::string tempFile = filename;    
	infiles.push_back(tempFile);
	nfile++;
      }
    }

    cout << "nfiles = " << nfile << endl;
  }
  

  cout << "output file name = " << outputFile.Data() << endl;
  //get TTree from file ...
  TreeReader data(infiles);
  
  Long64_t nTotal=0;
  Long64_t nPass[20]={0};
  TH1F* h_pt=new TH1F("h_pt","",125,0,2500);
  h_pt->SetXTitle("Higgs p_{T} [GeV]");

  TH1F* h_deno=(TH1F*)h_pt->Clone("h_deno");
  TH1F* h_numrFAT=(TH1F*)h_pt->Clone("h_numrFAT");
  TH1F* h_numrTHIN=(TH1F*)h_pt->Clone("h_numrTHIN");
  TH1F* h_numrFATCSV=(TH1F*)h_pt->Clone("h_numrFATCSV");
  TH1F* h_numrTHINCSV=(TH1F*)h_pt->Clone("h_numrTHINCSV");

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

    //// FAT Jet
     
    Int_t nFATJet         = data.GetInt("FATnJet");
    TClonesArray* fatjetP4 = (TClonesArray*) data.GetPtrTObject("FATjetP4");
    Float_t*  fatjetCISVV2 = data.GetPtrFloat("FATjetCISVV2");

    const float dRFATMax=0.8;
    bool findAFATJet=false;
    bool findAFATBJet=false;

    for(int ij=0; ij<nFATJet; ij++)
      {
    	
     	TLorentzVector* thisJet = (TLorentzVector*)fatjetP4->At(ij);
	if(thisJet->Pt()<30)continue;
	if(fabs(thisJet->Eta())>2.5)continue;
	
	if(thisJet->DeltaR(*b_p4[0]) > dRFATMax && myDefinition)continue;
	if(thisJet->DeltaR(*b_p4[1]) > dRFATMax && myDefinition)continue;

	if(thisJet->DeltaR(*higgs_p4) > 0.1 && !myDefinition)continue;

	findAFATJet=true;

	if(fatjetCISVV2[ij] < 0.605)continue;
	if(fatjetCISVV2[ij] < 0)continue;
	if(fatjetCISVV2[ij] > 1)continue;

	findAFATBJet=true;

	if(findAFATJet && findAFATBJet)break;
	
      }
    
    if(findAFATJet)
      {
	h_numrFAT->Fill(higgs_p4->Pt());
	nPass[1]++;

	if(findAFATBJet)
	  {
	    h_numrFATCSV->Fill(higgs_p4->Pt());
	    nPass[2]++;
	  }

      }

    //// THIN Jet
    Int_t nTHINJet         = data.GetInt("THINnJet");
    TClonesArray* thinjetP4 = (TClonesArray*) data.GetPtrTObject("THINjetP4");
    Float_t*  thinjetCISVV2 = data.GetPtrFloat("THINjetCISVV2");

    bool findATHINJet[2]={false,false};
    bool findATHINB[2]={false,false};
    const float dRTHINMax=myDefinition? 0.4 : 0.1;
    int bjetindex[2]={-1,-1};
    for(int ij=0; ij<nTHINJet; ij++)
      {
    	
     	TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(ij);
	if(thisJet->Pt()<30)continue;
	if(fabs(thisJet->Eta())>2.5)continue;
	
	// exclude when the jet is matched to both
	if(thisJet->DeltaR(*b_p4[0]) < dRTHINMax && thisJet->DeltaR(*b_p4[1]) < dRTHINMax)continue;
	else if(thisJet->DeltaR(*b_p4[0]) < dRTHINMax && thisJet->DeltaR(*b_p4[1]) > dRTHINMax)
	  {
	    findATHINJet[0]=true;
	    if(thinjetCISVV2[ij]>0.605 && thinjetCISVV2[ij]<1)
	      findATHINB[0]=true;
	    bjetindex[0] = ij;
	  }

	else if(thisJet->DeltaR(*b_p4[0]) > dRTHINMax && thisJet->DeltaR(*b_p4[1]) < dRTHINMax)
	  {
	    findATHINJet[1]=true;
	    if(thinjetCISVV2[ij]>0.605 && thinjetCISVV2[ij]<1)
	      findATHINB[1]=true;
	    bjetindex[1] = ij;
	  }
	    
	if(findATHINJet[0] && findATHINJet[1] && 
	   findATHINB[0] && findATHINB[1])break;

	
      }


    
    if(findATHINJet[0] && findATHINJet[1])
      {

	// for(int i=0;i<2;i++)
	//   std::cout << "b jet index " << i <<  " = " << bjetindex[i] << endl;

	h_numrTHIN->Fill(higgs_p4->Pt());
	nPass[3]++;
	if(findATHINB[0] && findATHINB[1])
	  {
	    // for(int i=0;i<2;i++)
	    //   std::cout << "real b jet index " << i <<  " = " << bjetindex[i] << endl;

	    h_numrTHINCSV->Fill(higgs_p4->Pt());
	    nPass[4]++;
	  }
      }
    
   

  } // end of loop over entries
  TEfficiency* fEff = new TEfficiency(*h_numrFAT,*h_deno);
  fEff->SetName("fatJetEff");
  fEff->SetTitle("AK8 jet efficiency; Higgs p_{T} [GeV]; Efficiency");
  TGraphAsymmErrors* graph_fEff=fEff->CreateGraph();
  graph_fEff->SetName("graph_fEff");

  TEfficiency* tEff = new TEfficiency(*h_numrTHIN,*h_deno);
  tEff->SetName("thinJetEff");
  tEff->SetTitle("Two AK4 jet efficiency; Higgs p_{T} [GeV]; Efficiency");
  TGraphAsymmErrors* graph_tEff=tEff->CreateGraph();
  graph_tEff->SetName("graph_tEff");

  TEfficiency* fEffCSV = new TEfficiency(*h_numrFATCSV,*h_numrFAT);
  fEffCSV->SetName("fatJetCSVEff");
  fEffCSV->SetTitle("AK8 CSV efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* tEffCSV = new TEfficiency(*h_numrTHINCSV,*h_numrTHIN);
  tEffCSV->SetName("thinJetCSVEff");
  tEffCSV->SetTitle("Two AK4 CSV efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEffTotal = new TEfficiency(*h_numrFATCSV,*h_deno);
  fEffTotal->SetName("fatJetTotalEff");
  fEffTotal->SetTitle("AK8 total efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* tEffTotal = new TEfficiency(*h_numrTHINCSV,*h_deno);
  tEffTotal->SetName("thinJetTotalEff");
  tEffTotal->SetTitle("Two AK4 total efficiency; Higgs p_{T} [GeV]; Efficiency");

  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  h_deno->Write();
  h_numrFAT->Write();
  h_numrTHIN->Write();
  h_numrFATCSV->Write();
  h_numrTHINCSV->Write();

  fEff->Write();
  tEff->Write(); 

  fEffTotal->Write();
  tEffTotal->Write(); 

  fEffCSV->Write();
  tEffCSV->Write(); 

  graph_fEff->Write();
  graph_tEff->Write();

  outFile->Close();
  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;
    
  
  

}
