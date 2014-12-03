#include "DelPanj/TreeMaker/interface/patMuonTree.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Math/VectorUtil.h"


patMuonTree::patMuonTree(std::string name, TTree* tree, const edm::ParameterSet& iConfig):
  baseTree(name,tree),
  patMuonLabel_ ( iConfig.getParameter<edm::InputTag>("patMuons"))
{
  SetBranches();
}


patMuonTree::~patMuonTree(){

} 


void
patMuonTree::Fill(const edm::Event& iEvent){
  Clear();

  edm::Handle<pat::MuonCollection> patMuonHandle;
  if(not iEvent.getByLabel(patMuonLabel_,patMuonHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "
	     <<patMuonLabel_<<std::endl; exit(0);}

  pat::MuonCollection muColl(*(patMuonHandle.product()));
  std::sort(muColl.begin(),muColl.end(),PtGreater());


  pat::MuonCollection::const_iterator mu;

  for(mu=muColl.begin(); mu!=muColl.end(); mu++){

    int muonIndex=-1;
    int tempTrackIndex=-1;
    int tempSegIndex=-1;
    int nSegments=-1;
    for(pat::MuonCollection::const_iterator jm=muColl.begin(); jm!=muColl.end(); jm++){

      muonIndex++;

      if(mu->innerTrack()==jm->innerTrack() && tempTrackIndex==-1)
	tempTrackIndex=muonIndex;
      if(jm->isTrackerMuon() && mu->isTrackerMuon() && 
	 muon::sharedSegments(*jm,*mu)>0 &&  tempSegIndex==-1)
	{
	  tempSegIndex  =muonIndex;
	  nSegments     =muon::sharedSegments(*jm,*mu);
	}


    } // loop over jmuon
    
    nMu_++;

    patMuonType_.push_back(mu->type());
    patMuonPt_.push_back(mu->pt());
    patMuonEta_.push_back(mu->eta());
    patMuonPhi_.push_back(mu->phi());
    patMuonM_.push_back(mu->mass());
    patMuonTrkIso_.push_back(mu->trackIso());
    patMuonHcalIso_.push_back(mu->hcalIso());
    patMuonEcalIso_.push_back(mu->ecalIso());
    patMuonCharge_.push_back(mu->charge());
    
    //--------------------------------------------------------//
    // new added variables

    isGlobalMuon_.push_back(mu->isGlobalMuon());
    isTrackerMuon_.push_back(mu->isTrackerMuon());
    patMuonITrkIndex_.push_back(tempTrackIndex);
    patMuonSegIndex_.push_back(tempSegIndex);
    patMuonNSeg_.push_back(nSegments);
    patMuonGood_.push_back(muon::isGoodMuon(*mu, muon::TrackerMuonArbitrated)? 1:0);
    // reco::TrackRef cktTrack = globalMuonID_.GetBestTrack(*mu);
    // patMuonPtErrx_.push_back(cktTrack->ptError()/cktTrack->pt());
    // patMuondxy_.push_back(cktTrack->dxy(myPv.position()));
    // patMuondz_.push_back(cktTrack->dz(myPv.position()));
    
    if(!mu->isTrackerMuon() && !mu->isGlobalMuon()){
      patMuonTrkLayers_.push_back(-1);
      patMuonPixelHits_.push_back(-1);
      patMuonMatches_.push_back(-1);

    }else{
      patMuonTrkLayers_.push_back(mu->innerTrack()->hitPattern().trackerLayersWithMeasurement());
      patMuonPixelHits_.push_back(mu->innerTrack()->hitPattern().numberOfValidPixelHits());
      patMuonMatches_.push_back(mu->numberOfMatchedStations());

    }

    if(!mu->isGlobalMuon())
      patMuonHits_.push_back(-1);
    else
      patMuonHits_.push_back(mu->globalTrack()->hitPattern().numberOfValidMuonHits());
    
    //--------------------------------------------------------//

    Float_t iso1 = 999.;
    Float_t iso2 = 999.;
    Float_t iso3 = 999.;
    Float_t isoPU = -999.; 
  
    iso1  =  mu->pfIsolationR04().sumChargedHadronPt;
    iso2  =  mu->pfIsolationR04().sumNeutralHadronEt;
    iso3  =  mu->pfIsolationR04().sumPhotonEt;
    isoPU =  mu->pfIsolationR04().sumPUPt;    

    patMuonChHadIso_.push_back(iso1);
    patMuonNeHadIso_.push_back(iso2);
    patMuonGamIso_.push_back(iso3);
    patMuonPUPt_.push_back(isoPU);

    Float_t corrPfIso = iso1 + std::max(0.0, iso2 + iso3 - 0.5*isoPU );
    patMuonCorrPfIso_.push_back(corrPfIso);
      
    Float_t rawTrackIso = mu->trackIso();

    Float_t corrTrackIso = rawTrackIso;
    // remove the contribution of global muons in the track isolation
    for(pat::MuonCollection::const_iterator jmu=muColl.begin(); 
	jmu!=muColl.end(); jmu++){

      if(jmu==mu)continue;

      Float_t dR = ROOT::Math::VectorUtil::DeltaR(mu->p4(),jmu->p4());
      Float_t thisMuPt = jmu->pt();
      if(dR<0.3)corrTrackIso -= thisMuPt;

    } // loop over jmu  
    
    patMuonCorrTrkIso_.push_back(corrTrackIso);

  }
}


void 
patMuonTree::SetBranches(){

  AddBranch(&nMu_,"nMu");
  AddBranch(&patMuonType_, "muType");
  AddBranch(&patMuonPt_, "muPt");
  AddBranch(&patMuonEta_, "muEta");
  AddBranch(&patMuonPhi_, "muPhi");
  AddBranch(&patMuonM_, "muM");
  AddBranch(&patMuonTrkIso_, "muTrkIso");
  AddBranch(&patMuonCorrTrkIso_, "muCorrTrkIso");
  AddBranch(&patMuonHcalIso_, "muHcalIso");
  AddBranch(&patMuonEcalIso_, "muEcalIso");
  AddBranch(&patMuonCharge_, "muCharge");

  AddBranch(&patMuonChHadIso_, "muChHadIso");
  AddBranch(&patMuonNeHadIso_, "muNeHadIso");
  AddBranch(&patMuonGamIso_, "muGamIso");
  AddBranch(&patMuonPUPt_, "muPUPt");
  AddBranch(&patMuonCorrPfIso_, "muCorrPfIso");


  // new added
  AddBranch(&isGlobalMuon_, "isGlobalMuon");
  AddBranch(&isTrackerMuon_, "isTrackerMuon");
  AddBranch(&patMuonPtErrx_, "muPtErrx");
  AddBranch(&patMuondxy_, "mudxy");
  AddBranch(&patMuondz_, "mudz");
  AddBranch(&patMuonTrkLayers_, "muTrkLayers");
  AddBranch(&patMuonPixelHits_, "muPixelHits");
  AddBranch(&patMuonHits_, "muHits");
  AddBranch(&patMuonMatches_, "muMatches");
  AddBranch(&patMuonITrkIndex_, "muITrkID");
  AddBranch(&patMuonSegIndex_, "muSegID");
  AddBranch(&patMuonNSeg_, "muNSegs");
  AddBranch(&patMuonGood_, "muGood");
}


void
patMuonTree::Clear(){

  nMu_ =0;
  patMuonType_.clear();
  patMuonPt_.clear();
  patMuonEta_.clear();
  patMuonPhi_.clear();
  patMuonM_.clear();
  patMuonTrkIso_.clear();
  patMuonCorrTrkIso_.clear();
  patMuonHcalIso_.clear();
  patMuonEcalIso_.clear();
  patMuonCharge_.clear();

  patMuonChHadIso_.clear();
  patMuonNeHadIso_.clear();
  patMuonGamIso_.clear();
  patMuonPUPt_.clear();
  patMuonCorrPfIso_.clear();


  // new added
  isGlobalMuon_.clear();
  isTrackerMuon_.clear();
  patMuonPtErrx_.clear();
  patMuondxy_.clear();
  patMuondz_.clear();
  patMuonTrkLayers_.clear();
  patMuonPixelHits_.clear();
  patMuonHits_.clear();
  patMuonMatches_.clear();
  patMuonITrkIndex_.clear();
  patMuonSegIndex_.clear();
  patMuonNSeg_.clear();
  patMuonGood_.clear();

}
