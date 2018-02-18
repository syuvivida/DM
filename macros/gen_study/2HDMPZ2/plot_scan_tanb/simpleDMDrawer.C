#include "untuplizer.h"
#include "TH1D.h"
#include <TH2.h>
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "TFile.h"
#include "TDirectory.h"
#include <iostream>
#include <TSystem.h>
#include <TString.h>

using namespace std;

int simpleDMDrawer(string input, bool debug=false){
	
  TreeReader data(input.data());

  string output="histo_" + input;
  cout << "output = " << output.data() << endl;
  
  TH1D* th1[20];
  TH2D* th2[5];
  
  float xmin=0;
  float xmax= 600;
  int nbins = xmax/10.;
	
  th1[0]=new TH1D("genMET_LHE","genMET_LHE",nbins,xmin,xmax);
  th1[1]=new TH1D("Higgs_pt","Higgs_pt",nbins,xmin,xmax);
  th1[2]=new TH1D("genMET_particle","genMET_particle",nbins,xmin,xmax);
  th1[3]=new TH1D("MET_LHE","MET_LHE",nbins,xmin,xmax);
  th1[4]=new TH1D("mchichi_LHE","mchichi_LHE",nbins,xmin,xmax);
  th1[5]=new TH1D("m3b_LHE","m3b_LHE",nbins*1.2,xmin,xmax*1.2);

  th2[0]=new TH2D("METvsmchichi","METvsmchichi",nbins,xmin,xmax,nbins,xmin,xmax);
  th2[1]=new TH2D("METvm3b","METvsm3b",nbins,xmin,xmax,1.2*nbins,xmin,1.2*xmax);
  th2[2]=new TH2D("mchichivsm3b","mchichivsm3b",nbins,xmin,xmax,1.2*nbins,xmin,1.2*xmax);

  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if(debug && jEntry>10)break;
    data.GetEntry(jEntry);
    // true MET (before parton shower)
    float genMET_true=data.GetFloat("partonMET");
    th1[0]->Fill(genMET_true);
		
    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");
    int nGenPar=data.GetInt("nGenPar");
    int * genParSt=data.GetPtrInt("genParSt");
    int * genParId=data.GetPtrInt("genParId");

    TClonesArray* lheParP4 = (TClonesArray*) data.GetPtrTObject("lheParP4");
    int nLHEPar=data.GetInt("nLHEPar");
    int * lheParSt=data.GetPtrInt("lheParSt");
    int * lheParId=data.GetPtrInt("lheParId");

		
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
      
			
    } // end of loop over genParticles

	
    // true MET after parton shower
    if(findDM1 && findDM2)
      th1[2]->Fill( (*DM1+*DM2).Pt());

    // check LHE-level mass distributions

    TLorentzVector* thisH_lhe=NULL ,*DM1_lhe=NULL, *DM2_lhe=NULL; 
    findH=false; findDM1=false; findDM2=false;
		
    // loop over the LHE particles
    for(int i=0;i<nLHEPar;i++){
      
      if(lheParId[i]==25 && lheParSt[i]==1 ){
	thisH_lhe=(TLorentzVector*)lheParP4->At(i);
	findH=true;
	if(debug)thisH_lhe->Print();
      }
      
      if(lheParId[i]==DMId && lheParSt[i]==1 ){
	DM1_lhe=(TLorentzVector*)lheParP4->At(i);
	findDM1=true;
	if(debug)DM1_lhe->Print();
      }
      else if(lheParId[i]== -DMId && lheParSt[i]==1 ){
	DM2_lhe=(TLorentzVector*)lheParP4->At(i);
	findDM2=true;
	if(debug)DM2_lhe->Print();
      }
      
			
    } // end of loop over genParticles

	
    // true MET from LHE
    if(findDM1 && findDM2)
      {
	th1[3]->Fill( (*DM1_lhe+*DM2_lhe).Pt());
	th1[4]->Fill( (*DM1_lhe+*DM2_lhe).M());
	th2[0]->Fill( (*DM1_lhe+*DM2_lhe).Pt(), (*DM1_lhe+*DM2_lhe).M());
      }
    // true transverse mass from LHE
    if(findDM1 && findDM2 && findH)
      {
	th1[5]->Fill( (*DM1_lhe+*DM2_lhe+*thisH_lhe).M());
	th2[1]->Fill( (*DM1_lhe+*DM2_lhe).Pt(), (*DM1_lhe+*DM2_lhe+*thisH_lhe).M());
	th2[2]->Fill( (*DM1_lhe+*DM2_lhe).M(), (*DM1_lhe+*DM2_lhe+*thisH_lhe).M());
      }
		
  } // end of loop over entries
	
  gSystem->mkdir("output2");
  TFile* outputFile=new TFile(Form("output2/%s",output.data()),"recreate");
  for(int i=0;i<6;i++)th1[i]->Write();
  for(int i=0;i<3;i++)th2[i]->Write();
  outputFile->Close();
	
	
  return 0;
	
	
}

