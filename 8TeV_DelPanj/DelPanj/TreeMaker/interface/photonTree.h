#ifndef __PHOTON_TREE_H_
#define __PHOTON_TREE_H_

/*
Log:
Sep 10, 2011
Anil Singh: Empty template created. 
*/

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "TTree.h"
#include "FWCore/Framework/interface/Event.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DelPanj/TreeMaker/interface/utils.h"
#include "DelPanj/TreeMaker/interface/baseTree.h"
#include "DataFormats/ParticleFlowCandidate/interface/PFCandidate.h"
#include "DataFormats/EgammaCandidates/interface/Photon.h"
using namespace std;
using namespace edm;

class photonTree : public baseTree{

 public:
  photonTree(std::string name, TTree* tree, const edm::ParameterSet& cfg);
  ~photonTree();
  void Fill(const edm::Event& iEvent);
  void SetBranches();
  void Clear();

 private:
  photonTree(){};
  Bool_t usePFObjects_;
  edm::InputTag photonTreeLabel_;
  edm::InputTag rho25C;


  //variables which would become branches
  Int_t nPho_;
  double rho25_;
  std::vector<Float_t> photonPt_;
  std::vector<Float_t> photonEta_;
  std::vector<Float_t> photonPhi_;
  std::vector<double> photonSigmaIetaIeta_;
  std::vector<double> photonhadronicOverEm_;
  std::vector<double> photonecalRecHitSumEtConeDR04_;
  std::vector<double> photonhcalTowerSumEtConeDR04_;
  std::vector<double> photontrkSumPtHollowConeDR04_;




};

#endif

