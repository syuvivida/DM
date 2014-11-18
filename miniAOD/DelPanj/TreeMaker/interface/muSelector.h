#ifndef __MUSELECTOR_H__
#define __MUSELECTOR_H__

/*
MuonSelector
Optimal Usage: patMuons.

Lovedeep Kaur,
Panjab University.

Shin-Shan Eiko Yu,
National Central University

*/

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
//#include "DataFormats/Math/interface/LorentzVector.h"
// for tracks
#include "DataFormats/TrackReco/interface/Track.h"
#include "DataFormats/MuonReco/interface/MuonCocktails.h" 
// for vertexing
#include "FWCore/Framework/interface/ESHandle.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
// for making histograms
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TMath.h"

class muSelector{
   
 public:
  muSelector(const edm::ParameterSet ps);
  std::map<std::string, Bool_t> CutRecord(const pat::Muon& mu);


  void SetVertex(reco::Vertex primaryVertex){pv_ = primaryVertex;}

  // remove the contribution of nearby-tau
  Double_t GetTauCorrMuonPfIso(const pat::Muon& mu)
  {
    Double_t iso1 = mu.userIsolation(pat::PfChargedHadronIso);
    Double_t iso2 = mu.userIsolation(pat::PfNeutralHadronIso);
    Double_t iso3 = mu.userIsolation(pat::PfGammaIso);
    Double_t iso4 = iso1 + iso2 + iso3;
    return iso4;
  }

  // remove the contribution of pileups using the official recipe
  Double_t GetCorrMuonPfIso(const pat::Muon& mu)
  {
    Double_t iso1 = 999.;
    Double_t iso2 = 999.;
    Double_t iso3 = 999.;
    Double_t iso4 = 999.;
    Double_t isoPU = -999.;

    iso1  =  mu.pfIsolationR04().sumChargedHadronPt;
    iso2  =  mu.pfIsolationR04().sumNeutralHadronEt;
    iso3  =  mu.pfIsolationR04().sumPhotonEt;
    isoPU =  mu.pfIsolationR04().sumPUPt;

    //Calculate combined and beta corrected Iso                                                                                                   
    iso4 = iso1 + TMath::Max((Double_t)(iso3+iso2-0.5*isoPU),(Double_t)0.);
    return iso4;
  }

  Double_t GetBestMuonPt(const pat::Muon& mu)
  {
    return highPtID_ && mu.isGlobalMuon()? 
      ((muon::tevOptimized(mu, 200, 17., 40., 0.25)).first)->pt():
      mu.pt();

  }

  reco::TrackRef GetBestTrack(const pat::Muon& mu)
  {
    return highPtID_ && mu.isGlobalMuon()? 
      (muon::tevOptimized(mu, 200, 17., 40., 0.25)).first:
      mu.muonBestTrack();
  }
  
  ~muSelector(){}
  
  Double_t ptX_;
  Double_t ptErrX_;
  Double_t etaX_;
  Double_t phiX_;

  // in ID parameters idPar_
  Double_t dxyX_;
  Double_t dzX_;
  Double_t normalizedChi2X_;
  Double_t trackerHitsX_;
  Double_t pixelHitsX_;
  Double_t muonHitsX_;
  Double_t nMatchesX_;
  Double_t isoX_;
  

  Bool_t reqTrigMatch_;
  Bool_t highPtID_;
  Bool_t trackerMuID_;
  edm::ParameterSet idPar_;



  reco::Vertex pv_;

};

#endif
