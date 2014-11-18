#ifndef __puweight__
#define __puweight__

#include <memory>
#include <string>
#include <iostream>
#include "TTree.h" 
#include "FWCore/Framework/interface/Event.h" 
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "SimDataFormats/PileupSummaryInfo/interface/PileupSummaryInfo.h"
#include "DelPanj/TreeMaker/interface/baseTree.h"

class puweight : public baseTree {

 public:
  puweight(std::string name, TTree* tree);
  ~puweight();
  void Fill(const edm::Event& iEvent); 
  void Clear();
 private:
  puweight(){};//Don't allow user
  void SetBranches();


  Float_t nTrueInt_;
  Int_t   nPUVert_;

};

#endif

