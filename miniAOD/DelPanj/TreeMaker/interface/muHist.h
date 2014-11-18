#ifndef __MHIST_H__
#define __MHIST_H__
#include "DelPanj/TreeMaker/interface/eSelector.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "DataFormats/BeamSpot/interface/BeamSpot.h"

#include "TH1D.h"
class muHist{

public:
  muHist(std::string cutLevel,  edm::Service<TFileService>& theFile);
  ~muHist();
  void Fill(pat::Muon& e);
  void FillAfterCuts(pat::Muon& e,  reco::BeamSpot& beamSpotHandle, std::map<std::string, bool>& ewp);
 private:
  muHist();//dont allow the default constructor.
  TH1D* InitHist( std::string qty,  std::string cutLevel, 
		  std::string title,int nbins, double rlow,
		  double rhigh,  edm::Service<TFileService>& theFile);
  TH1D* pt_;
  TH1D* eta_;
  TH1D* dxy_;
  TH1D* normChi2_;
  TH1D* trkHits_;
  TH1D* muonHits_;
  TH1D* nMatches_;
  TH1D* trkIso_;
  TH1D* eclIso_;
  TH1D* hclIso_;
};


#endif


