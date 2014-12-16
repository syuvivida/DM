/*
Lovedeep Kaur Saini
Panjab University, 
Chandigarh
*/
#include <iostream>
#include "DelPanj/TreeMaker/interface/eventInfo.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"

eventInfo::eventInfo(std::string name, TTree* tree):
  baseTree(name, tree)
{

  SetBranches();

}


eventInfo::~eventInfo(){
 
}


void
eventInfo::Fill(const edm::Event& iEvent){
  Clear();

  isData_ = iEvent.isRealData()? 1:0;
  nEvt_   = iEvent.id().event();
  nRun_   = iEvent.id().run();
  nLumiS_ = iEvent.luminosityBlock();
  bunchX_ = iEvent.bunchCrossing();


  edm::Handle<reco::VertexCollection> recVtxs_;

  if (iEvent.getByLabel("goodOfflinePrimaryVertices", recVtxs_)) {
    for (size_t i=0; i<recVtxs_->size(); ++i) {
      if((*recVtxs_)[i].ndof() >= 4 && fabs((*recVtxs_)[i].z()) <= 24 && fabs((*recVtxs_)[i].position().rho()) <= 2
	 && !((*recVtxs_)[i].isFake())
	 )
	{
	  nVtx_++;
	  vertexX_.push_back((*recVtxs_)[i].x());
	  vertexY_.push_back((*recVtxs_)[i].y());
	  vertexZ_.push_back((*recVtxs_)[i].z());
	} // if satifying good vertices
    }
  }
}

void
eventInfo::SetBranches(){
  AddBranch(&isData_,"isData");
  AddBranch(&nEvt_,"eventId");
  AddBranch(&nRun_,  "runId");
  AddBranch(&nLumiS_, "lumiSection");
  AddBranch(&bunchX_, "bunchXing");
  AddBranch(&nVtx_, "nVtx");
  AddBranch(&vertexX_, "vx");
  AddBranch(&vertexY_, "vy");
  AddBranch(&vertexZ_, "vz");
}


void 
eventInfo::Clear(){
  isData_ = 0;
  nEvt_   = -99999;
  nRun_   = -99999;
  nLumiS_ = -99999;
  bunchX_ = -99999;
  nVtx_ = 0;
  vertexX_.clear();
  vertexY_.clear();
  vertexZ_.clear();
}

