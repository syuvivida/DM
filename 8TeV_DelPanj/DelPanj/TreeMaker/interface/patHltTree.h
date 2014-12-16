#ifndef patHlttree_h
#define patHlttree_h

#include<iostream>
#include<string>
#include<vector>

#include "TTree.h"
#include "FWCore/Framework/interface/Event.h" 
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DelPanj/TreeMaker/interface/baseTree.h"

class patHltTree : public baseTree{

 public:
  patHltTree(std::string name,TTree* tree);
  void Fill(const edm::Event& iEvent);
  void Clear();
  void SetBranches();

 private:
  patHltTree(){};
  Int_t nTrigs_;
  std::vector<Int_t> trigResult_;
  std::vector<std::string> trigName_;
};

#endif
