#ifndef __GENJET_TREE_H_
#define __GENJET_TREE_H_


#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "TTree.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DelPanj/TreeMaker/interface/utils.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DelPanj/TreeMaker/interface/baseTree.h"
#include "DelPanj/TreeMaker/interface/jetSelector.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectionUncertainty.h"
#include "CondFormats/JetMETObjects/interface/FactorizedJetCorrector.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include <boost/shared_ptr.hpp>


using namespace std;
using namespace edm;



class genjetTree  : public baseTree{

 public:
  genjetTree(std::string name, TTree* tree, const edm::ParameterSet& cfg);//name=patJetAk05
  ~genjetTree();
  void Fill(const edm::Event& iEvent, edm::EventSetup const& iSetup) ; 
  void SetBranches();
  void Clear();
  TClonesArray *GenJets_4Momentum;
  Int_t genjet_n;
  Int_t ngenMuons;
 private:
  genjetTree();
  
  
};

#endif
