#include "DelPanj/TreeMaker/interface/muSelector.h"

/*
  This works according to the official muon recipe in the following page:
  https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideMuonId#Tight_Muon_selection/
*/


muSelector::muSelector(const edm::ParameterSet ps):      
  ptX_  (ps.getParameter<Double_t>("pt")),
  ptErrX_(ps.getParameter<Double_t>("ptErr")),
  etaX_ (ps.getParameter<Double_t>("eta")),
  phiX_ (ps.getParameter<Double_t>("phi")),
  reqTrigMatch_ (ps.getParameter<Bool_t>("requireTrigMatch")),
  highPtID_(ps.getParameter<Bool_t>("highPtID")),
  trackerMuID_(ps.getParameter<Bool_t>("trackerMuID")),
  idPar_(ps.getParameter<edm::ParameterSet> ("idPar"))
{
  normalizedChi2X_=(idPar_.getParameter<Double_t>("normalizedChi2"));
  muonHitsX_=(idPar_.getParameter<Double_t>("muonHits"));
  nMatchesX_=(idPar_.getParameter<Double_t>("nMatches"));
  dxyX_ =(idPar_.getParameter<Double_t>("dxy"));
  dzX_ =(idPar_.getParameter<Double_t>("dz"));
  trackerHitsX_=(idPar_.getParameter<Double_t>("trackerHits"));
  pixelHitsX_=(idPar_.getParameter<Double_t>("pixelHits"));
  isoX_ =(idPar_.getParameter<Double_t>("isoRel"));



}


std::map<std::string, Bool_t>  muSelector::CutRecord(const pat::Muon& mu){
  
  std::map<std::string, Bool_t> cuts;
  reco::TrackRef cktTrack = GetBestTrack(mu); // new Tune if high pt ID
  Double_t pt  = highPtID_? cktTrack->pt() : mu.pt();
  Double_t eta = highPtID_? cktTrack->eta(): mu.eta();
  Double_t phi = highPtID_? cktTrack->phi(): mu.phi();
  Double_t pterr = cktTrack->pt()>1e-3? cktTrack->ptError()/cktTrack->pt():-999;

  //Kinematic and Fiducial Acceptance
  cuts["ptx"]         = pt > ptX_;
  cuts["etax"]        = fabs(eta) < etaX_;
  cuts["phix"]        = fabs(phi) < phiX_;
  cuts["pterrx"]      = pterr < ptErrX_; 
  //MuonIdentification
  cuts["dxy"]         = highPtID_?  (fabs(cktTrack->dxy(pv_.position())) < dxyX_) : (fabs(mu.dB()) < dxyX_) ;
  cuts["dz"]          = fabs(cktTrack->dz( pv_.position())) < dzX_;


  Bool_t isTrackMuon   = mu.isTrackerMuon();
  Bool_t isGlobalMuon  = mu.isGlobalMuon();

  
  if(isTrackMuon && trackerMuID_)
    {
      cuts["trkLayers"]     = mu.innerTrack()->hitPattern().trackerLayersWithMeasurement() > trackerHitsX_;
      cuts["pixelHits"]     = mu.innerTrack()->hitPattern().numberOfValidPixelHits() > pixelHitsX_;
      cuts["nMatches"]      = mu.numberOfMatchedStations() > nMatchesX_;
    }
  else if(isGlobalMuon)
    {//If it is a global or a tracker muon
      cuts["trkLayers"]     = mu.innerTrack()->hitPattern().trackerLayersWithMeasurement() > trackerHitsX_;
      cuts["pixelHits"]     = mu.innerTrack()->hitPattern().numberOfValidPixelHits() > pixelHitsX_;
      cuts["muonHits"]      = mu.globalTrack()->hitPattern().numberOfValidMuonHits() > muonHitsX_;
      cuts["nMatches"]      = mu.numberOfMatchedStations() > nMatchesX_;
      cuts["normalizedChi2"]= highPtID_? 1 : mu.globalTrack()->normalizedChi2() < normalizedChi2X_; 
    }
  else
    {
      cuts["trkLayers"]      = 0;
      cuts["pixelHits"]      = 0;
      cuts["muonHits"]       = 0;
      cuts["nMatches"]       = 0;    
      cuts["normalizedChi2"] = 0;
    }
  

  //Muon Isolation... configurable.
  Double_t iso4 = 999.;
  iso4 = GetCorrMuonPfIso(mu)/TMath::Max((Double_t)0.1,pt);

  cuts["isoX"]   = iso4 < isoX_;
  
  //Do the trigger Match stuff.
  if(reqTrigMatch_){
    cuts["isTrigMatch"]= mu.triggerObjectMatches().size() > 0 ;
  }
  else 
    cuts["isTrigMatch"]= 1;
  
  return cuts;
}
