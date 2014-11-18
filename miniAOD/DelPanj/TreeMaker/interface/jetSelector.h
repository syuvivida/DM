#ifndef __JETSELECTOR_H__
#define __JETSELECTOR_H__

/*
jetSelector
Optimal Usage: patJets

Shin-Shan Eiko Yu,
National Central University

*/
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "FWCore/Framework/interface/ESHandle.h"

// for making histograms
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"



class jetSelector{
   
 public:
  jetSelector();
  std::map<std::string, Bool_t> MergedJetCut(const pat::Jet& jet);
  std::map<std::string, Bool_t> LooseJetCut(const pat::Jet& jet);
  std::map<std::string, Bool_t> TightJetCut(const pat::Jet& jet);
  ~jetSelector(){}

};

#endif
