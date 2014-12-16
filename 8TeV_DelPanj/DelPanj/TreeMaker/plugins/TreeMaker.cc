// -*- C++ -*-
//
// Package:    TreeMaker
// Class:      TreeMaker
// Original Author:  Anil Singh, Ashok Kumar
//                   Panjab University, Delhi University
//         Created:  Tue Jul  6 21:04:59 CEST 2010
//


// system include files
#include <memory>
#include <string>

#include "DelPanj/TreeMaker/interface/TreeMaker.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
TreeMaker::TreeMaker(const edm::ParameterSet& iConfig)

{
  fillPUweightInfo_=0;
  fillEventInfo_=0;
  fillGenInfo_=0;
  fillTrigInfo_=0;
  fillMuonInfo_=0;
  fillElecInfo_=0;
  fillJetInfo_=0;
  fillMetInfo_=0;
  fillPhotInfo_=0; 


  fillPUweightInfo_ = iConfig.getParameter<Bool_t>("fillPUweightInfo_");
  fillEventInfo_ = iConfig.getParameter<Bool_t>("fillEventInfo_");
  fillGenInfo_   = iConfig.getParameter<Bool_t>("fillGenInfo_");
  fillMuonInfo_  = iConfig.getParameter<Bool_t>("fillMuonInfo_");
  fillElecInfo_  = iConfig.getParameter<Bool_t>("fillElecInfo_");
  fillJetInfo_   = iConfig.getParameter<Bool_t>("fillJetInfo_");
  fillMetInfo_   = iConfig.getParameter<Bool_t>("fillMetInfo_");
  fillTrigInfo_  = iConfig.getParameter<Bool_t>("fillTrigInfo_");
  fillPhotInfo_  = iConfig.getParameter<Bool_t>("fillPhotInfo_");


  edm::Service<TFileService> fs;

  //file = new TFile(outFileName_.c_str(),"recreate");
  ;
  //tree_ = new TTree("tree","tree");

  tree_ = fs->make<TTree>("tree","tree");
  if( fillPUweightInfo_) puweight_=new puweight("pu_",tree_);
  if( fillEventInfo_) eventInfo_=new eventInfo("info_",tree_); 
  if( fillGenInfo_)   genInfoTree_ = new genInfoTree("",tree_,iConfig);
  if( fillMuonInfo_)  patMuTree_= new patMuonTree("",tree_,iConfig);
  if( fillElecInfo_)  patElecTree_ = new patElecTree("",tree_,iConfig);

  if( fillMetInfo_)   patMetTree_= new patMetTree("pf",tree_,iConfig);
  if( fillJetInfo_)   CA8jetTree_=new jetTree("CA8",tree_,iConfig);
  if( fillJetInfo_)   AK5jetTree_=new jetTree("AK5",tree_,iConfig);
  if( fillTrigInfo_)  patHltTree_ = new patHltTree("hlt_",tree_); 
  if( fillPhotInfo_)  photonTree_ = new photonTree("", tree_, iConfig); 
   
}


TreeMaker::~TreeMaker()
{
  //delete tree_;
}

void
TreeMaker::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  using namespace edm;

  if( fillPUweightInfo_) puweight_  ->Fill(iEvent);
  if( fillEventInfo_) eventInfo_   ->Fill(iEvent);
  if( fillGenInfo_)   genInfoTree_ ->Fill(iEvent);
  if( fillElecInfo_)  patElecTree_ ->Fill(iEvent);

  if( fillMuonInfo_)  patMuTree_   ->Fill(iEvent);
  if( fillJetInfo_)   CA8jetTree_  ->Fill(iEvent, iSetup);
  if( fillJetInfo_)   AK5jetTree_  ->Fill(iEvent, iSetup);
  if( fillMetInfo_)   patMetTree_  ->Fill(iEvent);
  if( fillTrigInfo_)  patHltTree_  ->Fill(iEvent);
  if( fillPhotInfo_)  photonTree_  ->Fill(iEvent);
  tree_->Fill();
}


void
TreeMaker::beginJob(){
}




void
TreeMaker::endJob() {

}

DEFINE_FWK_MODULE(TreeMaker);
