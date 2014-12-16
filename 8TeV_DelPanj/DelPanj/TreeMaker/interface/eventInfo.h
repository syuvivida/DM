#ifndef __eventInfo__
#define __eventInfo__

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "TTree.h" 
#include "FWCore/Framework/interface/Event.h" 
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "DelPanj/TreeMaker/interface/baseTree.h"

class eventInfo : public baseTree {

 public:
  eventInfo(std::string name, TTree* tree);
  ~eventInfo();
  void Fill(const edm::Event& iEvent); 
  void Clear();
 private:
  eventInfo(){};//Don't allow user
  void SetBranches();

  int isData_;
  int nEvt_;
  int nRun_;
  int nLumiS_;
  int bunchX_;

  int nVtx_;
  std::vector<Float_t> vertexX_;
  std::vector<Float_t> vertexY_;
  std::vector<Float_t> vertexZ_;		

};

#endif

