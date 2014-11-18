#ifndef __JET_TREE_H_
#define __JET_TREE_H_





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



class jetTree  : public baseTree{

 public:
  jetTree(std::string name, TTree* tree, const edm::ParameterSet& cfg);//name=patJetAk05
  ~jetTree();
  void Fill(const edm::Event& iEvent, edm::EventSetup const& iSetup) ; 
  void SetBranches();
  void Clear();
 
 private:
  jetTree();

  bool isCA8Jet_;

  edm::InputTag JetLabel_;
  edm::InputTag PrunedJetLabel_;
  edm::InputTag rhoSrc_;
  edm::InputTag pvSrc_;
  

  std::vector<std::string> jecPayloadNames_;
  std::string              jecUncName_;

  jetSelector jet2012ID_;

  boost::shared_ptr<JetCorrectionUncertainty> jecUnc_;
  boost::shared_ptr<FactorizedJetCorrector> jec_;
  

  //Branches common to all the jets.
  Int_t nJet_;
  std::vector<Float_t> jetPt_;
  std::vector<Float_t> jetEta_;
  std::vector<Float_t> jetPhi_;
  std::vector<Float_t> jetM_;
  std::vector<Float_t> jetEn_;
  std::vector<Float_t> jetCorrUncUp_;
  std::vector<Float_t> jetCorrUncDown_;
  std::vector<Int_t>   jetCharge_;
  std::vector<Int_t>   jetPartonFlavor_;
  std::vector<Int_t>   jetPassID_;

  // btag information
  std::vector<Float_t> jetSSV_;
  std::vector<Float_t> jetCSV_;        
  std::vector<Float_t> jetTCHP_;
  std::vector<Float_t> jetTCHE_;
  std::vector<Float_t> jetJP_;
  std::vector<Float_t> jetJBP_;

  std::vector<Float_t> jetTau1_;
  std::vector<Float_t> jetTau2_;
  std::vector<Float_t> jetTau3_;
  std::vector<Float_t> jetTau4_;

  std::vector<Float_t> jetMuEF_;
  std::vector<Float_t> jetPhoEF_;
  std::vector<Float_t> jetCEmEF_;
  std::vector<Float_t> jetCHadEF_;
  std::vector<Float_t> jetNEmEF_;
  std::vector<Float_t> jetNHadEF_;
  std::vector<Float_t> jetCMulti_;

  // pruned information

  std::vector<Float_t> jetPrunedPt_;
  std::vector<Float_t> jetPrunedEta_;
  std::vector<Float_t> jetPrunedPhi_;
  std::vector<Float_t> jetPrunedM_;
  std::vector<Float_t> jetPrunedEn_;
  std::vector<Float_t> jetPrunedCorrUncUp_;
  std::vector<Float_t> jetPrunedCorrUncDown_;
  std::vector<Int_t>   jetPrunedCharge_;
  std::vector<Int_t>   jetPrunedPartonFlavor_;


  std::vector<Float_t> jetPrunedSSV_;
  std::vector<Float_t> jetPrunedCSV_;        
  std::vector<Float_t> jetPrunedTCHP_;
  std::vector<Float_t> jetPrunedTCHE_;
  std::vector<Float_t> jetPrunedJP_;
  std::vector<Float_t> jetPrunedJBP_;

  // subjet of pruned jets
  Int_t nSubPrunedJet_;
  
  std::vector<Float_t> subjetPrunedPt_;
  std::vector<Float_t> subjetPrunedEta_;
  std::vector<Float_t> subjetPrunedPhi_;
  std::vector<Float_t> subjetPrunedM_;
  std::vector<Float_t> subjetPrunedEn_;
  std::vector<Int_t>   subjetPrunedCharge_;
  std::vector<Int_t>   subjetPrunedPartonFlavor_;


  //  std::vector<Float_t> subjetPrunedSSV_;
  std::vector<Float_t> subjetPrunedCSV_;        
  //  std::vector<Float_t> subjetPrunedTCHP_;
  //  std::vector<Float_t> subjetPrunedTCHE_;
  //  std::vector<Float_t> subjetPrunedJP_;
  //  std::vector<Float_t> subjetPrunedJBP_;
  

};

#endif
