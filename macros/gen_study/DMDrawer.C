

#include "untuplizer.h"
#include "TH1D.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TDirectory.h"
#include <iostream>
#include <TSystem.h>
#include <TString.h>

using namespace std;

int DMDrawer(string input, bool debug=false){
	
  TreeReader data(input.data());

  TString output=gSystem->GetFromPipe(Form("file=%s; test=${file/gentuple/histo}; echo \"$test\"",input.data()));
  cout << "output = " << output.Data() << endl;
  
  TH1D* th1[20];
  
  float xmin=0;
  float xmax= 1000;
  int nbins = xmax/10.;
	
  th1[0]=new TH1D("genMET_true","genMET_true",nbins,xmin,xmax);
  th1[1]=new TH1D("Higgs_pt","Higgs_pt",nbins,xmin,xmax);
  th1[2]=new TH1D("deltaR","deltaR",60,0,6);
  th1[3]=new TH1D("genMET_DM","genMET_DM",nbins,xmin,xmax);
  th1[4]=new TH1D("HT_parton","HT_parton",nbins,xmin,xmax);   //  th1[5]=new TH1D("HT_jet","HT_jet",nbins,xmin,xmax);
  th1[5]=new TH1D("MT_parton","MT_parton",nbins,xmin,xmax);

  th1[6]=new TH1D("MH_jetAK4","MH_jetAK4", 100,0,500);
  th1[7]=new TH1D("ptH_jetAK4","ptH_jetAK4",nbins,xmin,xmax);

  th1[8]=new TH1D("MH_jetAK8","MH_jetAK8",100,0,500);
  th1[9]=new TH1D("ptH_jetAK8","ptH_jetAK8",nbins,xmin,xmax);

	
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if(debug && jEntry>10)break;
    data.GetEntry(jEntry);
    // true MET (after parton shower)
    float genMET_true=data.GetFloat("genMET_true");
    th1[0]->Fill(genMET_true);
		
    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
    int nGenPar=data.GetInt("nGenPar");
    int * genParSt=data.GetPtrInt("genParSt");
    int * genParId=data.GetPtrInt("genParId");
		
    TLorentzVector* thisH=NULL ,*DM1=NULL, *DM2=NULL, *thisbjet=NULL, *thatbjet=NULL;
    bool findH=false, findDM1=false, findDM2=false, findb1=false, findb2=false;
		
    const int DMId=18;

    // loop over the generator particles
    for(int i=0;i<nGenPar;i++){
      
      if(genParId[i]==25 && genParSt[i]==22 ){
	thisH=(TLorentzVector*)genParP4->At(i);
	th1[1]->Fill(thisH->Pt());
	findH=true;
	if(debug)thisH->Print();
      }
      
      if(genParId[i]==DMId && genParSt[i]==1 ){
	DM1=(TLorentzVector*)genParP4->At(i);
	findDM1=true;
	if(debug)DM1->Print();
      }
      else if(genParId[i]== -DMId && genParSt[i]==1 ){
	DM2=(TLorentzVector*)genParP4->At(i);
	findDM2=true;
	if(debug)DM2->Print();
      }
      
      if(genParId[i]==5 && genParSt[i]==23){
	thisbjet =(TLorentzVector*)genParP4->At(i);
	findb1=true;
	if(debug)thisbjet->Print();
      }
      else if (genParId[i]== -5 && genParSt[i]==23){
	thatbjet =(TLorentzVector*)genParP4->At(i);
	findb2=true;
	if(debug)thatbjet->Print();
      }
			
    } // end of loop over genParticles

	
    // true deltaR between the two b quarks
    if(findb1 && findb2)
      th1[2]->Fill(thisbjet->DeltaR(*thatbjet)); 

    // true MET before parton shower
    if(findDM1 && findDM2)
      th1[3]->Fill( (*DM1+*DM2).Pt());

    // parton-level HT
    if(findH && findDM1 && findDM2)
      th1[4]->Fill( thisH->Pt()+(*DM1+*DM2).Pt() );

    
    // parton-level MT

    TLorentzVector* MET_vector=new TLorentzVector((*DM1+*DM2).Px(),
						  (*DM1+*DM2).Py(),
						  0,
						  (*DM1+*DM2).Pt());

    TLorentzVector* pHTran_vector= new TLorentzVector(thisH->Px(),
						    thisH->Py(),
						    0,
						    thisH->Et());

    th1[5]->Fill( (*MET_vector + *pHTran_vector).M() );

    // loop over number of genJets
    TClonesArray* ak4GenJetP4 = (TClonesArray*) data.GetPtrTObject("ak4GenJetP4");
    int ak4nGenJet=data.GetInt("ak4nGenJet");

    int ak4JetIndex[2]={-1,-1};
    bool findBothJets=false;
    for(int i=0; i<ak4nGenJet; i++){
      
     TLorentzVector* thisJet = (TLorentzVector*)ak4GenJetP4->At(i);
     if(thisJet->Pt()<30)continue;
     if(fabs(thisJet->Eta())>2.4)continue;

     // check deltaR between b-quark and AK4b-jets
     if(thisbjet->DeltaR(*thisJet) > 0.4 &&
	thatbjet->DeltaR(*thisJet) > 0.4)continue;

     if(ak4JetIndex[0]<0)
       ak4JetIndex[0]=i;
     else if(ak4JetIndex[1]<0)
       ak4JetIndex[1]=i;
     else
       {
	 findBothJets=true;
	 break;
       }

    }

    if(debug)
      cout << "ak4 jet index: " << ak4JetIndex[0] << "\t" << ak4JetIndex[1] << endl;

    if(findBothJets)
      {
	TLorentzVector HJet(0,0,0,0);
	TLorentzVector* ak4bjet[2];
	for(int ij=0; ij<2; ij++){
	  ak4bjet[ij]
	    = (TLorentzVector*)ak4GenJetP4->At(ak4JetIndex[ij]);
	  HJet += (*ak4bjet[ij]);
	}
	th1[6]->Fill(HJet.M());
	th1[7]->Fill(HJet.Pt());
      }

    TClonesArray* ak8GenJetP4 = (TClonesArray*) data.GetPtrTObject("ak8GenJetP4");
    int ak8nGenJet=data.GetInt("ak8nGenJet");

    for(int i=0; i<ak8nGenJet; i++){
      
      TLorentzVector* thisJet = (TLorentzVector*)ak8GenJetP4->At(i);
      if(thisJet->Pt()<170)continue;
      if(fabs(thisJet->Eta())>2.4)continue;

     // check deltaR between b-quark and AK8 Higgs-jets
     if(thisbjet->DeltaR(*thisJet) > 0.8 &&
	thatbjet->DeltaR(*thisJet) > 0.8)continue;

      if(debug)
	cout << "ak8 jet index: " << i << endl;
      
      th1[8]->Fill(thisJet->M());
      th1[9]->Fill(thisJet->Pt());     
      break;
    }


		
  } // end of loop over entries
	
  gSystem->mkdir("output");
  TFile* outputFile=new TFile(Form("output/%s",output.Data()),"recreate");
  for(int i=0;i<10;i++)th1[i]->Write();
  outputFile->Close();
	
	
  return 0;
	
	
}

