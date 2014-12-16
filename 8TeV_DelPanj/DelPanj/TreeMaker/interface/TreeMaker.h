#ifndef  TREE_MAKER_H
#define  TREE_MAKER_H

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DelPanj/TreeMaker/interface/puweight.h"
#include "DelPanj/TreeMaker/interface/eventInfo.h"
#include "DelPanj/TreeMaker/interface/genInfoTree.h"
#include "DelPanj/TreeMaker/interface/patMuonTree.h"
#include "DelPanj/TreeMaker/interface/patElecTree.h"
#include "DelPanj/TreeMaker/interface/patMetTree.h"
#include "DelPanj/TreeMaker/interface/patHltTree.h"
#include "DelPanj/TreeMaker/interface/jetTree.h"
#include "DelPanj/TreeMaker/interface/photonTree.h"

#include "TTree.h"
#include "TFile.h"

class TreeMaker : public edm::EDAnalyzer {
   public:
      explicit TreeMaker(const edm::ParameterSet&);
      ~TreeMaker();


   private:
      virtual void beginJob() ;
      virtual void analyze(const edm::Event&, const edm::EventSetup& );
      virtual void endJob() ;
      TFile* file;
      TTree* tree_;
      std::string outFileName_ ;

      Bool_t fillPUweightInfo_; 
      Bool_t fillEventInfo_;

      Bool_t fillGenInfo_;
      Bool_t fillTrigInfo_;
      Bool_t fillMuonInfo_;
      Bool_t fillElecInfo_;
      Bool_t fillMetInfo_;
      Bool_t fillJetInfo_;
      Bool_t fillPhotInfo_;

      puweight *puweight_;
      eventInfo   *eventInfo_;
      genInfoTree *genInfoTree_;    
      patMuonTree *patMuTree_;
      patElecTree *patElecTree_;

      patMetTree  *patMetTree_;
      jetTree     *CA8jetTree_;
      jetTree     *AK5jetTree_;
      photonTree  *photonTree_;
      patHltTree  *patHltTree_;      


};


#endif
