

#include "untuplizer.h"
#include "TH1D.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TDirectory.h"
#include <iostream>
#include <fstream>
#include <TSystem.h>
#include <TString.h>

using namespace std;

void errmc(float nsig,float ntotal,float& eff, float& err)
{
  eff = nsig/ntotal;
  err = sqrt( (1-eff)*eff/ntotal);
  cout << "Efficiency of " << nsig << "/" << ntotal << "= " <<eff << " +- " << err << endl;
  return;
}


int DMGenEff(string input,int option=0){
	
  TreeReader data(input.data());
  
  TString MassMed=gSystem->GetFromPipe(Form("file=%s; test=${file##*MSC}; test2=${test%%_*}; echo \"$test2\"",input.data()));
  cout << "MassMed = " << MassMed.Data() << endl;

  int mass = atoi(MassMed.Data());
  cout << "mass = " << mass << endl;

  TString MassDM=gSystem->GetFromPipe(Form("file=%s; test=${file##*MDM}; test2=${test%%.root*}; echo \"$test2\"",input.data()));
  cout << "MassDM = " << MassDM.Data() << endl;

	
  TH1D* th1[20];
  
  float xmin=0;
  float xmax= mass>2000? 10000:2500;
  int nbins = xmax/10.;
	
  th1[0]=new TH1D("genMET_true","genMET_true",nbins,xmin,xmax);
  th1[1]=new TH1D("Higgs_pt","Higgs_pt",nbins,xmin,xmax);
  th1[2]=new TH1D("b0_pt","b0_pt",nbins,xmin,xmax);
  th1[3]=new TH1D("b1_pt","b1_pt",nbins,xmin,xmax);
  th1[4]=new TH1D("deltaR","deltaR",60,0,6);
  th1[5]=new TH1D("genMET_DM","genMET_DM",nbins,xmin,xmax);
	
  Long64_t nTotal=0;
  Long64_t nPass=0;
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
    data.GetEntry(jEntry);
    nTotal++;
    float genMET_true=data.GetFloat("genMET_true");
    th1[0]->Fill(genMET_true);
    
    if(genMET_true < 200)continue;

    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
    int nGenPar=data.GetInt("nGenPar");
    int * genParSt=data.GetPtrInt("genParSt");
    int * genParId=data.GetPtrInt("genParId");
    
    int bIndex[2]={-1,-1};
    TLorentzVector* thisJet ,*DM1, *DM2;
    bool findThis=false, findDM1=false, findDM2=false;
		
    int DMId=18;
    
    for(int i=0;i<nGenPar;i++){
      
      if(genParId[i]==25&&genParSt[i]==22 ){
	thisJet=(TLorentzVector*)genParP4->At(i);
	th1[1]->Fill(thisJet->Pt());
	findThis=true;
      }
      
      if(genParId[i]==DMId&&genParSt[i]==1 ){
	//cout<<"found";
	DM1=(TLorentzVector*)genParP4->At(i);
	findDM1=true;
      }
      if(genParId[i]==-DMId&&genParSt[i]==1 ){
	//cout<<"found";
	DM2=(TLorentzVector*)genParP4->At(i);
	findDM2=true;
      }
      
      //if(genParSt[i]!=23)continue;
      if(fabs(genParId[i])==5 && bIndex[0]==-1 &&genParSt[i]==23){
	bIndex[0]=i;
      }
      else if (fabs(genParId[i])==5 && bIndex[0]!=-1 &&genParSt[i]==23 ){
	bIndex[1]=i;
      }
      
    }
    TLorentzVector* thisbJet =(TLorentzVector*)genParP4->At(bIndex[0]);
    TLorentzVector* thatbJet =(TLorentzVector*)genParP4->At(bIndex[1]);
		
    th1[2]->Fill(thisbJet->Pt());
    th1[3]->Fill(thatbJet->Pt());
    th1[4]->Fill(thisbJet->DeltaR(*thatbJet)); 
    if(findDM1 && findDM2)
      th1[5]->Fill( (*DM1+*DM2).Pt());
    if(!findThis)continue;
    if(thisJet->Pt()<200)continue;
    if(fabs(thisJet->Eta())>2.4)continue;
    nPass++;
  }
  
  float eff, efferr;
  errmc(nPass,nTotal,eff,efferr);
  ofstream fout;
  fout.open("ZpBaryonic_geneff.dat",ios::app | ios::out);
  fout << MassMed << " " << MassDM << " " << eff << " " << efferr << endl;
  return 0;
	
	
}

