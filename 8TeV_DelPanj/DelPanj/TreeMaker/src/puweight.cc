/*
  Lovedeep Kaur Saini
  Panjab University, 
  Chandigarh
*/
#include <iostream>
#include "DelPanj/TreeMaker/interface/puweight.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

puweight::puweight(std::string name, TTree* tree):
  baseTree(name, tree)
{

  SetBranches();

}


puweight::~puweight(){

}


void
puweight::Fill(const edm::Event& iEvent){
  Clear();

  if(iEvent.isRealData()) {
    nTrueInt_ = -999999999.;
    nPUVert_  = -999999999;
  }
  else {
    edm::Handle<std::vector< PileupSummaryInfo > >  PupInfo;
    iEvent.getByLabel(edm::InputTag("addPileupInfo"), PupInfo);

    std::vector<PileupSummaryInfo>::const_iterator PVI;


    Float_t npT=-1.;
    Int_t npIT=-1.;

    for(PVI = PupInfo->begin(); PVI != PupInfo->end(); ++PVI) {

      Int_t BX = PVI->getBunchCrossing();
//       std::cout << "BX = " << BX << std::endl;
      if(BX == 0) {
	npT = PVI->getTrueNumInteractions();
	//	std::cout << "npT = " << npT << std::endl;
	npIT = PVI->getPU_NumInteractions();
      }
    }

    nTrueInt_ = npT;
    nPUVert_  = npIT;
  }
}

void
puweight::SetBranches(){
  AddBranch(&nTrueInt_,  "nTrueInt");
  AddBranch(&nPUVert_,   "nPUVert");
}






void 
puweight::Clear(){
  nTrueInt_=-99999.;
  nPUVert_ =-99999.;
}

