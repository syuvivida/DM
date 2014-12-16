#ifndef _GEN_INFO_TREE_H_
#define _GEN_INFO_TREE_H_

#include <memory>
#include <vector>
#include <string>
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/JetReco/interface/GenJet.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DelPanj/TreeMaker/interface/baseTree.h"
#include "TH1.h"
#include "TFile.h"
#include "TTree.h"

//
// class declaration
//

class genInfoTree : public baseTree{

 public:
  genInfoTree(std::string name, TTree* tree, const edm::ParameterSet& cfg);
  ~genInfoTree();
  void Fill(const edm::Event& iEvent);
  void SetBranches();
  void Clear();
  
  
  edm::InputTag genPartLabel_;
  
  


  Float_t ptHat_;      // added by Eiko
  Float_t mcWeight_;   // added by Eiko

  Int_t nGenPar_;
  std::vector<Float_t> genParE_;
  std::vector<Float_t> genParPt_;
  std::vector<Float_t> genParEta_;
  std::vector<Float_t> genParPhi_;
  std::vector<Float_t> genParM_;
  std::vector<Int_t>    genParQ_;
  std::vector<Int_t>    genParId_;
  std::vector<Int_t>    genParSt_;
  std::vector<Int_t>    genMomParId_; // added by Eiko
  std::vector<Int_t>    genParIndex_;    // added by Eiko
  std::vector<Int_t>    genNMo_;
  std::vector<Int_t>    genNDa_;
  std::vector<Int_t>    genMo1_;
  std::vector<Int_t>    genMo2_;
  std::vector<Int_t>    genDa1_;
  std::vector<Int_t>    genDa2_;

  Int_t nGenJet_;
  std::vector<Float_t> genJetE_;
  std::vector<Float_t> genJetPt_;
  std::vector<Float_t> genJetEta_; 
  std::vector<Float_t> genJetPhi_; 
};

#endif
