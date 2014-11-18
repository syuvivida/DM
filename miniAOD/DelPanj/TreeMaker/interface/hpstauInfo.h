#ifndef __HPSTAU_INFO_H_
#define __HPSTAU_INFO_H_
//#include "MVAElectronID.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"
#include "TrackingTools/TransientTrack/interface/GsfTransientTrack.h"
#include "Geometry/Records/interface/GlobalTrackingGeometryRecord.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "TTree.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "utils.h"
#include "baseTree.h"
#include "DataFormats/PatCandidates/interface/Tau.h"


class hpstauInfo : public baseTree{

 public:
  hpstauInfo(std::string name, TTree* tree, bool debug, const edm::ParameterSet& cfg);
  ~hpstauInfo();
  void Fill(const edm::Event& iEvent, const edm::EventSetup& iSetup);
  void SetBranches();
  void Clear();
  bool debug_;
 private:
  hpstauInfo(){};
  edm::InputTag tauLabel_;
  //variables which would become branches
  
  
  std::vector<float> HPSTau_NewVz;
  std::vector<float> taupt;
  
  std::vector<bool>  HPSTau_leadPFChargedHadrCand;
  std::vector<bool> HPSTau_leadPFChargedHadrCand_trackRef;
  
  std::vector<bool> disc_againstElectronLoose;                                 
  std::vector<bool> disc_againstElectronMedium;
  std::vector<bool> disc_againstElectronTight;
  std::vector<bool> disc_againstElectronLooseMVA5;
  std::vector<bool> disc_againstElectronMediumMVA5;
  std::vector<bool> disc_againstElectronTightMVA5;
  std::vector<bool> disc_againstElectronVLooseMVA5;
  std::vector<bool> disc_againstElectronVTightMVA5;
  std::vector<bool> disc_againstMuonLoose;
  std::vector<bool> disc_againstMuonMedium;
  std::vector<bool> disc_againstMuonTight;
  std::vector<bool> disc_againstMuonLoose2;
  std::vector<bool> disc_againstMuonMedium2;
  std::vector<bool> disc_againstMuonTight2;
  std::vector<bool> disc_againstMuonLooseMVA;
  std::vector<bool> disc_againstMuonMediumMVA;
  std::vector<bool> disc_againstMuonTightMVA;
  std::vector<bool> disc_againstMuonLoose3;
  std::vector<bool> disc_againstMuonTight3;
  		    
  std::vector<bool> disc_byVLooseCombinedIsolationDeltaBetaCorr;
  std::vector<bool> disc_byLooseCombinedIsolationDeltaBetaCorr;
  std::vector<bool> disc_byMediumCombinedIsolationDeltaBetaCorr;
  std::vector<bool> disc_byTightCombinedIsolationDeltaBetaCorr;
  		    
  std::vector<bool> disc_byLooseIsolation;
  		    
  std::vector<bool> disc_byVLooseIsolationMVA3newDMwLT;
  std::vector<bool> disc_byLooseIsolationMVA3newDMwLT;
  std::vector<bool> disc_byMediumIsolationMVA3newDMwLT;
  std::vector<bool> disc_byTightIsolationMVA3newDMwLT;
  std::vector<bool> disc_byVTightIsolationMVA3newDMwLT;
  std::vector<bool> disc_byVVTightIsolationMVA3newDMwLT;
  		    
  std::vector<bool> disc_byVLooseIsolationMVA3newDMwoLT;
  std::vector<bool> disc_byLooseIsolationMVA3newDMwoLT;
  std::vector<bool> disc_byMediumIsolationMVA3newDMwoLT;
  std::vector<bool> disc_byTightIsolationMVA3newDMwoLT;
  std::vector<bool> disc_byVTightIsolationMVA3newDMwoLT;
  std::vector<bool> disc_byVVTightIsolationMVA3newDMwoLT;
  		    
  std::vector<bool> disc_byVLooseIsolationMVA3oldDMwLT;
  std::vector<bool> disc_byLooseIsolationMVA3oldDMwLT;
  std::vector<bool> disc_byMediumIsolationMVA3oldDMwLT;
  std::vector<bool> disc_byTightIsolationMVA3oldDMwLT;
  std::vector<bool> disc_byVTightIsolationMVA3oldDMwLT;
  std::vector<bool> disc_byVVTightIsolationMVA3oldDMwLT;
  		    
  std::vector<bool> disc_byVLooseIsolationMVA3oldDMwoLT;
  std::vector<bool> disc_byLooseIsolationMVA3oldDMwoLT;
  std::vector<bool> disc_byMediumIsolationMVA3oldDMwoLT;
  std::vector<bool> disc_byTightIsolationMVA3oldDMwoLT;
  std::vector<bool> disc_byVTightIsolationMVA3oldDMwoLT;
  std::vector<bool> disc_byVVTightIsolationMVA3oldDMwoLT;
  		    
  std::vector<bool> disc_byLooseCombinedIsolationDeltaBetaCorr3Hits;
  std::vector<bool> disc_byMediumCombinedIsolationDeltaBetaCorr3Hits;
  std::vector<bool> disc_byTightCombinedIsolationDeltaBetaCorr3Hits;
  		    
  std::vector<bool> disc_decayModeFinding;
  std::vector<bool> disc_decayModeFindingNewDMs;
  std::vector<bool> disc_decayModeFindingOldDMs;
  
  std::vector<float> disc_chargedIsoPtSum;
  std::vector<float> disc_neutralIsoPtSum;
  std::vector<float> disc_puCorrPtSum; 
  
  int HPSTau_n;
  std::vector<int> HPSTau_charge;
  TClonesArray *HPSTau_4Momentum, *HPSTau_Vposition ;
};

#endif

