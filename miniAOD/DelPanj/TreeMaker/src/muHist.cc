#include "DelPanj/TreeMaker/interface/muHist.h"
#include "DelPanj/TreeMaker/interface/utils.h"

muHist::muHist(std::string cutLevel,  edm::Service<TFileService>& theFile){
 
  pt_  =  InitHist("PTMU",cutLevel,"MuonTransverseMomentum[GeV]",40,0,200.,theFile);
  eta_ =  InitHist("ETAMU",cutLevel,"MuonPseudo Rapidity",40,-4,4, theFile);
  dxy_ =  InitHist("DXYMU",cutLevel,"BeamCorrectedDxy[cm]",1000,-0.5,0.5,theFile);
  normChi2_ = InitHist("NCHI2",cutLevel,"NormalizedChi2(GlobalFit)",50,0,50,theFile);
  trkHits_ = InitHist("TRKHITS",cutLevel,"TrackerHits",50,0,50,theFile);
  muonHits_ = InitHist("MUONHITS",cutLevel,"MuonHits",50,0,50,theFile);
  nMatches_ = InitHist("NMATCHES",cutLevel,"NumMatches",50,0,50,theFile);
  eclIso_ = InitHist("ECALISO",cutLevel,"AbsoluteECALIsoDeposit_GeV" ,100,0.0,3.0,theFile);
  trkIso_ = InitHist("HCALISO",cutLevel,"AbsoluteHCALIsoDeposit_GeV",100,0.0,3.0, theFile);
  hclIso_ = InitHist("TRKISO", cutLevel,"AbsoluteTRACKSumPt_GeV",100,0.0,3.0,theFile);
}

TH1D* muHist::InitHist(std::string qty,  std::string cutLevel,
		      std::string title,int nbins, double rlow,
		      double rhigh,  edm::Service<TFileService>& theFile){
  TH1D* m; 
  std::string name = "h_"+qty+"_"+cutLevel;
  title = cutLevel+"_"+title;
  m = theFile->make<TH1D>(name.c_str(),title.c_str(),nbins,rlow,rhigh);
  return m;
}

void muHist::Fill(pat::Muon& mu){
  
}

void muHist::FillAfterCuts(pat::Muon& mu, reco::BeamSpot& beamSpotHandle, std::map<std::string, bool>& mwp){


    bool isTrackMuon =mu.isTrackerMuon();
    bool isGlobalMuon =mu.isGlobalMuon();
    bool isStandAloneMuon =mu.isStandAloneMuon();

    reco::TrackRef gm;
    if(isStandAloneMuon)
      gm = mu.standAloneMuon();
    if(isTrackMuon)
      gm = mu.track();
    if(isGlobalMuon)
      gm = mu.globalTrack();

  
  if(PassAllBut("ptx",mwp))pt_->Fill(mu.pt());
  if(PassAllBut("etax",mwp))eta_->Fill(mu.eta());

  if(PassAllBut("iso2",mwp))eclIso_ ->Fill(mu.ecalIso());
  if(PassAllBut("iso3",mwp))hclIso_ ->Fill(mu.hcalIso());
  if(PassAllBut("iso1",mwp))trkIso_ ->Fill(mu.trackIso()); 

  if(PassAllBut("dxy",mwp))dxy_-> Fill(gm->dxy(beamSpotHandle.position()));
  if(PassAllBut("normalizedChi2",mwp))normChi2_-> Fill(gm->normalizedChi2());
  if(PassAllBut("trackerHits",mwp))trkHits_-> Fill(gm->hitPattern().numberOfValidTrackerHits());
  //if(PassAllBut("pixelHits",mwp))             -> Fill  (gm->hitPattern().numberOfValidPixelHits());
  if(PassAllBut("muonHits",mwp))muonHits_-> Fill (gm->hitPattern().numberOfValidMuonHits());
  if(PassAllBut("nMatches",mwp))nMatches_->Fill (mu.numberOfMatches());
  
}
 





