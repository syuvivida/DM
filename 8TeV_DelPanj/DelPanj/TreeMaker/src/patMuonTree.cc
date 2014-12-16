#include "DelPanj/TreeMaker/interface/patMuonTree.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "Math/VectorUtil.h"


patMuonTree::patMuonTree(std::string name, TTree* tree, const edm::ParameterSet& iConfig):
  baseTree(name,tree),
  globalMuonID_ ( iConfig.getParameter<edm::ParameterSet>("globalMuonIDSet")),
  trackerMuonID_ ( iConfig.getParameter<edm::ParameterSet>("trackerMuonIDSet")),
  patMuonLabel_ ( iConfig.getParameter<edm::InputTag>("patMuons"))
{
  SetBranches();
}


patMuonTree::~patMuonTree(){

} 


void
patMuonTree::Fill(const edm::Event& iEvent){
  Clear();

  //   Bool_t isData = iEvent.isRealData();

  Int_t ngoodVtx=0;
  Int_t nVtx=0;
  reco::Vertex myPv;
 
  edm::Handle<reco::VertexCollection> recVtxs_;
  
  if (iEvent.getByLabel("goodOfflinePrimaryVertices", recVtxs_)){
    for (size_t i=0; i<recVtxs_->size(); ++i){
      if (!((*recVtxs_)[i].isFake())){
 	nVtx++;
 	if(nVtx==1)myPv = (*recVtxs_)[i]; // minimum requirement, not a fake
 	// further quality requirement
 	if(((*recVtxs_)[i].ndof() >= 4 && fabs((*recVtxs_)[i].z()) <= 24  && fabs((*recVtxs_)[i].position().rho()) <= 2) )
 	  {
	    ngoodVtx++;
 	    if(ngoodVtx==1)myPv = (*recVtxs_)[i];
 	  }
      }
      if(ngoodVtx<1 && nVtx<1 && recVtxs_->size()>0)
	myPv = (*recVtxs_)[0];

    } // loop over vertex collections
  } // if vertex handle is valid
    
  
  globalMuonID_.SetVertex(myPv);
  trackerMuonID_.SetVertex(myPv);

  edm::Handle<pat::MuonCollection> patMuonHandle;
  if(not iEvent.getByLabel(patMuonLabel_,patMuonHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "
	     <<patMuonLabel_<<std::endl; exit(0);}

  pat::MuonCollection muColl(*(patMuonHandle.product()));
  bestMuonPtGreater tempComp(this);
  std::sort(muColl.begin(),muColl.end(), tempComp);


  pat::MuonCollection::const_iterator mu;

  for(mu=muColl.begin(); mu!=muColl.end(); mu++){
    
    nMu_++;
    patMuonType_.push_back(mu->type());
    Float_t pt = globalMuonID_.GetBestMuonPt(*mu); // if it's hightPtID and a global muon, returns muon::tevOptimized                                                                             // else returns mu->pt()   
    patMuonPt_.push_back(pt);
    patMuonSimplePt_.push_back(mu->pt());
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

    reco::TrackRef cktTrack = globalMuonID_.GetBestTrack(*mu);
    patMuonPtErrx_.push_back(cktTrack->ptError()/cktTrack->pt());
    patMuondxy_.push_back(cktTrack->dxy(myPv.position()));
    patMuondz_.push_back(cktTrack->dz(myPv.position()));
    
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

    std::map<std::string, Bool_t> PassG = globalMuonID_.CutRecord(*mu);
    Int_t passOrNot  = 0;
    passOrNot |= PassAll(PassG)*2;
    std::map<std::string, Bool_t> PassT = trackerMuonID_.CutRecord(*mu);
    passOrNot |= PassAll(PassT)*4;

    patMuonPassID_.push_back(passOrNot);

    Float_t corrPfIso = globalMuonID_.GetCorrMuonPfIso(*mu);
    patMuonCorrPfIso_.push_back(corrPfIso);

    Float_t corrTauPfIso =  globalMuonID_.GetTauCorrMuonPfIso(*mu); 
    patMuonTauCorrPfIso_.push_back(corrTauPfIso);

    Float_t rawTrackIso = mu->trackIso();

    Float_t corrTrackIso = rawTrackIso;

    // remove the contribution of global muons in the track isolation
    for(pat::MuonCollection::const_iterator jmu=muColl.begin(); 
	jmu!=muColl.end(); jmu++){

      if(jmu==mu)continue;

      std::map<std::string, Bool_t> jPassG = globalMuonID_.CutRecord(*jmu);
      Int_t jpassOrNot  = 0;
      jpassOrNot |= PassAll(jPassG)*2;
      std::map<std::string, Bool_t> jPassT = trackerMuonID_.CutRecord(*jmu);
      jpassOrNot |= PassAll(jPassT)*4;

      if(jpassOrNot==0)continue;

      Float_t dR = ROOT::Math::VectorUtil::DeltaR(mu->p4(),jmu->p4());
      Float_t thisMuPt = globalMuonID_.GetBestMuonPt(*jmu);
      if(dR<0.3)corrTrackIso -= thisMuPt;

    } // loop over jmu  
    
    patMuonCorrTrkIso_.push_back(corrTrackIso);

  }
}


void 
patMuonTree::SetBranches(){

  AddBranch(&nMu_,"nMu");
  AddBranch(&patMuonType_,"muType");
  AddBranch(&patMuonPt_, "muPt");
  AddBranch(&patMuonSimplePt_, "muSimplePt");
  AddBranch(&patMuonEta_, "muEta");
  AddBranch(&patMuonPhi_, "muPhi");
  AddBranch(&patMuonM_, "muM");
  AddBranch(&patMuonTrkIso_, "muTrkIso");
  AddBranch(&patMuonCorrTrkIso_, "muCorrTrkIso");
  AddBranch(&patMuonHcalIso_, "muHcalIso");
  AddBranch(&patMuonEcalIso_, "muEcalIso");
  AddBranch(&patMuonCharge_, "muCharge");
  AddBranch(&patMuonPassID_, "muPassID");
  AddBranch(&patMuonChHadIso_, "muChHadIso");
  AddBranch(&patMuonNeHadIso_, "muNeHadIso");
  AddBranch(&patMuonGamIso_, "muGamIso");
  AddBranch(&patMuonPUPt_, "muPUPt");
  AddBranch(&patMuonCorrPfIso_, "muCorrPfIso");
  AddBranch(&patMuonTauCorrPfIso_, "muTauCorrPfIso");

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

}


void
patMuonTree::Clear(){

  nMu_ =0;
  patMuonType_.clear();
  patMuonPt_.clear();
  patMuonSimplePt_.clear();
  patMuonEta_.clear();
  patMuonPhi_.clear();
  patMuonM_.clear();
  patMuonTrkIso_.clear();
  patMuonCorrTrkIso_.clear();
  patMuonHcalIso_.clear();
  patMuonEcalIso_.clear();
  patMuonCharge_.clear();
  patMuonPassID_.clear();
  patMuonChHadIso_.clear();
  patMuonNeHadIso_.clear();
  patMuonGamIso_.clear();
  patMuonPUPt_.clear();
  patMuonCorrPfIso_.clear();
  patMuonTauCorrPfIso_.clear();

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

}
