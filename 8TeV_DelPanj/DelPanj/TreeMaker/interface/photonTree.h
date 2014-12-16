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
  edm::InputTag photonLabel_;

  //variables which would become branches
  Int_t nPho_;
  std::vector<Float_t> photonPt_;
  std::vector<Float_t> photonEta_;
  std::vector<Float_t> photonPhi_;

};

#endif

