#ifndef __ELEC_TREE_H_
#define __ELEC_TREE_H_


#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "TTree.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DelPanj/TreeMaker/interface/utils.h"
#include "DelPanj/TreeMaker/interface/eSelector.h"
#include "DelPanj/TreeMaker/interface/baseTree.h"

using namespace std;
using namespace edm;

class patElecTree : public baseTree {


 public:
  patElecTree(std::string name, TTree* tree, const edm::ParameterSet& cfg);
  ~patElecTree();
  void Fill(const edm::Event& iEvent);
  void SetBranches();
  void Clear();

 private:
//TTree* tree_;
  //Dont Allow User to Call the Default Constructor.
  patElecTree();


  bool isData_;

  eSelector e2012ID_;
  edm::InputTag eleRhoIsoInputTag_;
  edm::InputTag patElecLabel_;





  std::vector<Int_t> patElecType;
 //pt, eta, phi, M : Enough to caluclate
  //px,py,pz etc.==>No need to store later

  Float_t eleRho_;
  Int_t nEle_;
  std::vector<Float_t> patElecEt_;
  std::vector<Float_t> patElecEnergy_;
  std::vector<Float_t> patElecPt_;
  std::vector<Float_t> patElecEta_;
  std::vector<Float_t> patElecPhi_;
  std::vector<Float_t> patElecM_;
  std::vector<Float_t> patElecScEta_;
  std::vector<Float_t> patElecSigIhIh_;
  std::vector<Float_t> patElecDelEtaIn_;
  std::vector<Float_t> patElecDelPhiIn_;
  std::vector<Float_t> patElecHoE_;
  std::vector<Float_t> patElecTrkIso_;
  std::vector<Float_t> patElecHcalIso_;
  std::vector<Float_t> patElecEcalIso_;
  std::vector<Float_t> patElecCorrPfIso_;
  std::vector<Float_t> patElecTauCorrPfIso_;
  std::vector<Float_t> patElecUserTrkIso_;
  std::vector<Float_t> patElecUserCalIso_;

  std::vector<Float_t> patElecEoverP_;
  std::vector<Float_t> patElecDxy_;
  std::vector<Float_t> patElecDz_;
  std::vector<Float_t> patElecChHadIso_;
  std::vector<Float_t> patElecNeHadIso_;
  std::vector<Float_t> patElecGamIso_;

  // conversion rejection
  std::vector<Float_t> patElecMissingHits_ ;
  std::vector<Int_t>    patElecPassConv_;
  std::vector<Float_t> patElecInBarrel_;
  std::vector<Float_t> patElecInEndcap_;

  std::vector<Int_t>    patElecPassID_;

};

#endif

