//root -q -b -l juwu.C++\(\"inputFile\"\,\"outputFile\"\)


#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <TH1D.h>
#include <TRandom.h>
#include <TLorentzVector.h>
#include <TClonesArray.h>
#include <TString.h>
#include <TFile.h>
#include "untuplizer.h"

using namespace std;
void xAna_example(std::string inputFile, Float_t signalM=-1){


  TreeReader data(inputFile.data());


  TH1F* h_nvtx = new TH1F("h_nvtx","",100,0,100);
  TH1F* h_nvtx2 = new TH1F("h_nvtx2","",100,0,100);
  TH1F* h_CA8jetTau21   = new TH1F("h_CA8jetTau21","",20,0,1);
  TH1F* h_CA8jetPrunedM = new TH1F("h_CA8jetPrunedM","",100,100,200);
  TH1F* h_mX = new TH1F("h_mX","",1000,500,2500);

  //histogram anoucement
  TH1F* h_mZ   = new TH1F("h_mZ","",100,0,10000);

  double nDeno[3]={0};
  double nNumr[3]={0};
  
  //Event loop
   // for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){
   for(Long64_t jEntry=0; jEntry<30 ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);


    Int_t    nCA8jet     = data.GetInt("ADDnJet");
    for(int jetIndex=0; jetIndex < nCA8jet; jetIndex++){

      Int_t    nSubPrunedJet = data.GetPtrInt("ADDnSubSDJet")[jetIndex];

      vector<float>   *subPx =  data.GetPtrVectorFloat("ADDsubjetSDPx", nCA8jet);
      std::cout << "nSubJets = " <<  nSubPrunedJet << std::endl;

      for(int ij=0; ij < nSubPrunedJet; ij++)
	{
	  std::cout << " ij = " << ij << " , = " << subPx[jetIndex][ij] << std::endl;
	  
	}

      Int_t    nSV = data.GetPtrInt("ADDjet_nSV")[jetIndex];
      
      vector<float>   *SVMass =  data.GetPtrVectorFloat("ADDjet_SVMass", nCA8jet);
      std::cout << "nSV = " << nSV << std::endl;

      for(int ij=0; ij < nSV; ij++)
	{
	  std::cout << " ij = " << ij << " , = " << SVMass[jetIndex][ij] << std::endl;
	  
	}
    }

   TClonesArray* eleP4 = (TClonesArray*) data.GetPtrTObject("eleP4");

    Int_t nEle        = data.GetInt("nEle");
    for(int ie=0; ie< nEle; ie++)
      {
	TLorentzVector* thisEle = (TLorentzVector*)eleP4->At(ie);
	cout << "Px = " << thisEle->Px() << endl;
      }	

    

  }

}
