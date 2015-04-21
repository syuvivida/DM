#include "../interface/hpstauInfo.h"

hpstauInfo::hpstauInfo(std::string name, TTree* tree, bool debug, const pset& iConfig):baseTree(name,tree){
  if(debug) std::cout<<"in tau constructor"<<std::endl;
  tauLabel_       = iConfig.getUntrackedParameter<edm::InputTag> ("tauLabel_");
  HPSTau_4Momentum           = new TClonesArray("TLorentzVector");
  HPSTau_Vposition           = new TClonesArray("TVector3");
  if(debug) std::cout<<"in rho constructor: calling SetBrances()"<<std::endl;
  SetBranches();
  
  debug_ = debug ;
}

hpstauInfo::~hpstauInfo(){
  delete tree_;
}

void hpstauInfo::Fill(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  Clear();

  if(debug_)    std::cout<<"getting HPS Tau discriminators"<<std::endl;

    edm::Handle<pat::TauCollection>  tauHandle;
  iEvent.getByLabel(tauLabel_,tauHandle);
  if(not iEvent.getByLabel(tauLabel_,tauHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found:selectedPatTaus "<<std::endl;
    exit(0);
  }
  
  pat::TauCollection::const_iterator tau;
  for(tau=tauHandle->begin(); tau!=tauHandle->end(); tau++){

    
    if(debug_) std::cout<<" pt,eta,phi of "<<HPSTau_n+1
			<<" tau is : "<<tau->pt() 
			<<" : "<<tau->eta()
			<<" : "<<tau->phi()
			<<std::endl;
    TLorentzVector p4(tau->px(),tau->py(),tau->pz(),tau->energy());
    new( (*HPSTau_4Momentum)[HPSTau_n]) TLorentzVector(p4);
    TVector3 v3(tau->vx(),tau->vy(),tau->vz());
    new( (*HPSTau_Vposition)[HPSTau_n]) TVector3(v3);
    HPSTau_charge.push_back((int)tau->charge());
    
    //new disc for CSA14
    disc_againstElectronLoose.push_back(tau->tauID("againstElectronLoose"));
    disc_againstElectronMedium.push_back(tau->tauID("againstElectronMedium"));
    disc_againstElectronTight.push_back(tau->tauID("againstElectronTight"));
    disc_againstElectronLooseMVA5.push_back(tau->tauID("againstElectronLooseMVA5"));
    disc_againstElectronMediumMVA5.push_back(tau->tauID("againstElectronMediumMVA5"));
    disc_againstElectronTightMVA5.push_back(tau->tauID("againstElectronTightMVA5"));
    disc_againstElectronVLooseMVA5.push_back(tau->tauID("againstElectronVLooseMVA5"));
    disc_againstElectronVTightMVA5.push_back(tau->tauID("againstElectronVTightMVA5"));
    disc_againstMuonLoose.push_back(tau->tauID("againstMuonLoose"));
    disc_againstMuonMedium.push_back(tau->tauID("againstMuonMedium"));
    disc_againstMuonTight.push_back(tau->tauID("againstMuonTight"));
    disc_againstMuonLoose2.push_back(tau->tauID("againstMuonLoose2"));
    disc_againstMuonMedium2.push_back(tau->tauID("againstMuonMedium2"));
    disc_againstMuonTight2.push_back(tau->tauID("againstMuonTight2"));
    disc_againstMuonLooseMVA.push_back(tau->tauID("againstMuonLooseMVA"));
    disc_againstMuonMediumMVA.push_back(tau->tauID("againstMuonMediumMVA"));
    disc_againstMuonTightMVA.push_back(tau->tauID("againstMuonTightMVA"));
    disc_againstMuonLoose3.push_back(tau->tauID("againstMuonLoose3"));
    disc_againstMuonTight3.push_back(tau->tauID("againstMuonTight3"));
    
    //disc_byVLooseCombinedIsolationDeltaBetaCorr.push_back(tau->tauID("byVLooseCombinedIsolationDeltaBetaCorr"));
    //disc_byLooseCombinedIsolationDeltaBetaCorr.push_back(tau->tauID("byLooseCombinedIsolationDeltaBetaCorr"));
    //disc_byMediumCombinedIsolationDeltaBetaCorr.push_back(tau->tauID("byMediumCombinedIsolationDeltaBetaCorr"));
    //disc_byTightCombinedIsolationDeltaBetaCorr.push_back(tau->tauID("byTightCombinedIsolationDeltaBetaCorr"));
    
    //disc_byLooseIsolation.push_back(tau->tauID("byLooseIsolation"));
    
    disc_byVLooseIsolationMVA3newDMwLT.push_back(tau->tauID("byVLooseIsolationMVA3newDMwLT"));
    disc_byLooseIsolationMVA3newDMwLT.push_back(tau->tauID("byLooseIsolationMVA3newDMwLT"));
    disc_byMediumIsolationMVA3newDMwLT.push_back(tau->tauID("byMediumIsolationMVA3newDMwLT"));
    disc_byTightIsolationMVA3newDMwLT.push_back(tau->tauID("byTightIsolationMVA3newDMwLT"));
    disc_byVTightIsolationMVA3newDMwLT.push_back(tau->tauID("byVTightIsolationMVA3newDMwLT"));
    disc_byVVTightIsolationMVA3newDMwLT.push_back(tau->tauID("byVVTightIsolationMVA3newDMwLT"));
    
    disc_byVLooseIsolationMVA3newDMwoLT.push_back(tau->tauID("byVLooseIsolationMVA3newDMwoLT"));
    disc_byLooseIsolationMVA3newDMwoLT.push_back(tau->tauID("byLooseIsolationMVA3newDMwoLT"));
    disc_byMediumIsolationMVA3newDMwoLT.push_back(tau->tauID("byMediumIsolationMVA3newDMwoLT"));
    disc_byTightIsolationMVA3newDMwoLT.push_back(tau->tauID("byTightIsolationMVA3newDMwoLT"));
    disc_byVTightIsolationMVA3newDMwoLT.push_back(tau->tauID("byVTightIsolationMVA3newDMwoLT"));
    disc_byVVTightIsolationMVA3newDMwoLT.push_back(tau->tauID("byVVTightIsolationMVA3newDMwoLT"));
    
    disc_byVLooseIsolationMVA3oldDMwLT.push_back(tau->tauID("byVLooseIsolationMVA3oldDMwLT"));
    disc_byLooseIsolationMVA3oldDMwLT.push_back(tau->tauID("byLooseIsolationMVA3oldDMwLT"));
    disc_byMediumIsolationMVA3oldDMwLT.push_back(tau->tauID("byMediumIsolationMVA3oldDMwLT"));
    disc_byTightIsolationMVA3oldDMwLT.push_back(tau->tauID("byTightIsolationMVA3oldDMwLT"));
    disc_byVTightIsolationMVA3oldDMwLT.push_back(tau->tauID("byVTightIsolationMVA3oldDMwLT"));
    disc_byVVTightIsolationMVA3oldDMwLT.push_back(tau->tauID("byVVTightIsolationMVA3oldDMwLT"));
    
    disc_byVLooseIsolationMVA3oldDMwoLT.push_back(tau->tauID("byVLooseIsolationMVA3oldDMwoLT"));
    disc_byLooseIsolationMVA3oldDMwoLT.push_back(tau->tauID("byLooseIsolationMVA3oldDMwoLT"));
    disc_byMediumIsolationMVA3oldDMwoLT.push_back(tau->tauID("byMediumIsolationMVA3oldDMwoLT"));
    disc_byTightIsolationMVA3oldDMwoLT.push_back(tau->tauID("byTightIsolationMVA3oldDMwoLT"));
    disc_byVTightIsolationMVA3oldDMwoLT.push_back(tau->tauID("byVTightIsolationMVA3oldDMwoLT"));
    disc_byVVTightIsolationMVA3oldDMwoLT.push_back(tau->tauID("byVVTightIsolationMVA3oldDMwoLT"));
    
    disc_byLooseCombinedIsolationDeltaBetaCorr3Hits.push_back(tau->tauID("byLooseCombinedIsolationDeltaBetaCorr3Hits"));
    disc_byMediumCombinedIsolationDeltaBetaCorr3Hits.push_back(tau->tauID("byMediumCombinedIsolationDeltaBetaCorr3Hits"));
    disc_byTightCombinedIsolationDeltaBetaCorr3Hits.push_back(tau->tauID("byTightCombinedIsolationDeltaBetaCorr3Hits"));
    
    disc_decayModeFinding.push_back(tau->tauID("decayModeFinding"));
    disc_decayModeFindingNewDMs.push_back(tau->tauID("decayModeFindingNewDMs"));
    //disc_decayModeFindingOldDMs.push_back(tau->tauID("decayModeFindingOldDMs"));
    
    disc_chargedIsoPtSum.push_back(tau->tauID("chargedIsoPtSum"));
    disc_neutralIsoPtSum.push_back(tau->tauID("neutralIsoPtSum"));
    disc_puCorrPtSum.push_back(tau->tauID("puCorrPtSum"));
    
    
    HPSTau_leadPFChargedHadrCand.push_back(tau->leadPFChargedHadrCand().isNonnull() );    
    HPSTau_leadPFChargedHadrCand_trackRef.push_back(tau->leadPFChargedHadrCand().isNonnull() &&  tau->leadPFChargedHadrCand()->trackRef().isNonnull());
    
    //std::cout<<" middle pt = "<<tau->pt()<<std::endl;
    
    using namespace reco;
    using namespace pat;
    using namespace edm;

    ESHandle<MagneticField> B;
    iSetup.get<IdealMagneticFieldRecord > ().get(B);
    const MagneticField* magField = B.product();
    Handle<reco::BeamSpot> theBeamSpotHandle;
    iEvent.getByLabel(std::string("offlineBeamSpot"), theBeamSpotHandle);
    const reco::BeamSpot* beamSpot = theBeamSpotHandle.product();
    ESHandle<GlobalTrackingGeometry> geomHandle;
    iSetup.get<GlobalTrackingGeometryRecord > ().get(geomHandle);
    
    float newvz=0.0;
    if(tau->leadPFChargedHadrCand().isNonnull()) {
      if (tau->leadPFChargedHadrCand()->trackRef().isNonnull()) {
	reco::TransientTrack track(tau->leadPFChargedHadrCand()->trackRef(), magField, geomHandle);
	TransverseImpactPointExtrapolator extrapolator(magField);
	TrajectoryStateOnSurface closestOnTransversePlaneState = extrapolator.extrapolate(track.impactPointState(), GlobalPoint(beamSpot->position().x(), beamSpot->position().y(), 0.0));
	newvz = (closestOnTransversePlaneState.globalPosition().z());
      }
      
      if(tau->leadPFChargedHadrCand()->gsfTrackRef().isNonnull()) {
	reco::GsfTransientTrack track(tau->leadPFChargedHadrCand()->gsfTrackRef(),magField,geomHandle);
	TransverseImpactPointExtrapolator extrapolator(magField);
	TrajectoryStateOnSurface closestOnTransversePlaneState = extrapolator.extrapolate(track.impactPointState(),GlobalPoint(beamSpot->position().x(),beamSpot->position().y(),0.0));
	newvz = (closestOnTransversePlaneState.globalPosition().z());
      }
    }
    
    HPSTau_NewVz.push_back(newvz);
    taupt.push_back(tau->pt());
    //std::cout<<"other  pt = "<<tau->pt()<<"  :  new vz = "<<newvz<<std::endl;
    HPSTau_n++;
  }//end of for loop
  std::cout<<" -------------------------- HPSTau_n = "<<HPSTau_n<<std::endl;
  if(debug_)    std::cout<<"got HPS Tau discriminators info"<<std::endl;
}



void hpstauInfo::SetBranches(){
  if(debug_)    std::cout<<"setting branches: calling AddBranch of baseTree"<<std::endl;

  AddBranch(&HPSTau_n  ,"HPSTau_n");
  AddBranch(&taupt  ,"taupt");
  
  AddBranch(&HPSTau_4Momentum,"HPSTau_4Momentum");
  AddBranch(&HPSTau_Vposition,"HPSTau_Vposition");

  AddBranch(&HPSTau_leadPFChargedHadrCand,"HPSTau_leadPFChargedHadrCand");
  AddBranch(&HPSTau_leadPFChargedHadrCand_trackRef,"HPSTau_leadPFChargedHadrCand_trackRef");
   
  AddBranch(&disc_againstElectronLoose ,"disc_againstElectronLoose");
  AddBranch(&disc_againstElectronMedium ,"disc_againstElectronMedium");
  AddBranch(&disc_againstElectronTight ,"disc_againstElectronTight");
  AddBranch(&disc_againstElectronLooseMVA5 ,"disc_againstElectronLooseMVA5");
  AddBranch(&disc_againstElectronMediumMVA5 ,"disc_againstElectronMediumMVA5");
  AddBranch(&disc_againstElectronTightMVA5 ,"disc_againstElectronTightMVA5");
  AddBranch(&disc_againstElectronVLooseMVA5 ,"disc_againstElectronVLooseMVA5");
  AddBranch(&disc_againstElectronVTightMVA5 ,"disc_againstElectronVTightMVA5");
  AddBranch(&disc_againstMuonLoose ,"disc_againstMuonLoose");
  AddBranch(&disc_againstMuonMedium ,"disc_againstMuonMedium");
  AddBranch(&disc_againstMuonTight ,"disc_againstMuonTight");
  AddBranch(&disc_againstMuonLoose2 ,"disc_againstMuonLoose2");
  AddBranch(&disc_againstMuonMedium2 ,"disc_againstMuonMedium2");
  AddBranch(&disc_againstMuonTight2 ,"disc_againstMuonTight2");
  AddBranch(&disc_againstMuonLooseMVA ,"disc_againstMuonLooseMVA");
  AddBranch(&disc_againstMuonMediumMVA ,"disc_againstMuonMediumMVA");
  AddBranch(&disc_againstMuonTightMVA ,"disc_againstMuonTightMVA");
  AddBranch(&disc_againstMuonLoose3 ,"disc_againstMuonLoose3");
  AddBranch(&disc_againstMuonTight3 ,"disc_againstMuonTight3");

  AddBranch(&disc_byVLooseCombinedIsolationDeltaBetaCorr ,"disc_byVLooseCombinedIsolationDeltaBetaCorr");
  AddBranch(&disc_byLooseCombinedIsolationDeltaBetaCorr ,"disc_byLooseCombinedIsolationDeltaBetaCorr");
  AddBranch(&disc_byMediumCombinedIsolationDeltaBetaCorr ,"disc_byMediumCombinedIsolationDeltaBetaCorr");
  AddBranch(&disc_byTightCombinedIsolationDeltaBetaCorr ,"disc_byTightCombinedIsolationDeltaBetaCorr");

  AddBranch(&disc_byLooseIsolation ,"disc_byLooseIsolation");

  AddBranch(&disc_byVLooseIsolationMVA3newDMwLT ,"disc_byVLooseIsolationMVA3newDMwLT");
  AddBranch(&disc_byLooseIsolationMVA3newDMwLT ,"disc_byLooseIsolationMVA3newDMwLT");
  AddBranch(&disc_byMediumIsolationMVA3newDMwLT ,"disc_byMediumIsolationMVA3newDMwLT");
  AddBranch(&disc_byTightIsolationMVA3newDMwLT ,"disc_byTightIsolationMVA3newDMwLT");
  AddBranch(&disc_byVTightIsolationMVA3newDMwLT ,"disc_byVTightIsolationMVA3newDMwLT");
  AddBranch(&disc_byVVTightIsolationMVA3newDMwLT ,"disc_byVVTightIsolationMVA3newDMwLT");

  AddBranch(&disc_byVLooseIsolationMVA3newDMwoLT ,"disc_byVLooseIsolationMVA3newDMwoLT");
  AddBranch(&disc_byLooseIsolationMVA3newDMwoLT ,"disc_byLooseIsolationMVA3newDMwoLT");
  AddBranch(&disc_byMediumIsolationMVA3newDMwoLT ,"disc_byMediumIsolationMVA3newDMwoLT");
  AddBranch(&disc_byTightIsolationMVA3newDMwoLT ,"disc_byTightIsolationMVA3newDMwoLT");
  AddBranch(&disc_byVTightIsolationMVA3newDMwoLT ,"disc_byVTightIsolationMVA3newDMwoLT");
  AddBranch(&disc_byVVTightIsolationMVA3newDMwoLT ,"disc_byVVTightIsolationMVA3newDMwoLT");

  AddBranch(&disc_byVLooseIsolationMVA3oldDMwLT ,"disc_byVLooseIsolationMVA3oldDMwLT");
  AddBranch(&disc_byLooseIsolationMVA3oldDMwLT ,"disc_byLooseIsolationMVA3oldDMwLT");
  AddBranch(&disc_byMediumIsolationMVA3oldDMwLT ,"disc_byMediumIsolationMVA3oldDMwLT");
  AddBranch(&disc_byTightIsolationMVA3oldDMwLT ,"disc_byTightIsolationMVA3oldDMwLT");
  AddBranch(&disc_byVTightIsolationMVA3oldDMwLT ,"disc_byVTightIsolationMVA3oldDMwLT");
  AddBranch(&disc_byVVTightIsolationMVA3oldDMwLT ,"disc_byVVTightIsolationMVA3oldDMwLT");

  AddBranch(&disc_byVLooseIsolationMVA3oldDMwoLT ,"disc_byVLooseIsolationMVA3oldDMwoLT");
  AddBranch(&disc_byLooseIsolationMVA3oldDMwoLT ,"disc_byLooseIsolationMVA3oldDMwoLT");
  AddBranch(&disc_byMediumIsolationMVA3oldDMwoLT ,"disc_byMediumIsolationMVA3oldDMwoLT");
  AddBranch(&disc_byTightIsolationMVA3oldDMwoLT ,"disc_byTightIsolationMVA3oldDMwoLT");
  AddBranch(&disc_byVTightIsolationMVA3oldDMwoLT ,"disc_byVTightIsolationMVA3oldDMwoLT");
  AddBranch(&disc_byVVTightIsolationMVA3oldDMwoLT ,"disc_byVVTightIsolationMVA3oldDMwoLT");

  AddBranch(&disc_byLooseCombinedIsolationDeltaBetaCorr3Hits ,"disc_byLooseCombinedIsolationDeltaBetaCorr3Hits");
  AddBranch(&disc_byMediumCombinedIsolationDeltaBetaCorr3Hits ,"disc_byMediumCombinedIsolationDeltaBetaCorr3Hits");
  AddBranch(&disc_byTightCombinedIsolationDeltaBetaCorr3Hits ,"disc_byTightCombinedIsolationDeltaBetaCorr3Hits");

  AddBranch(&disc_decayModeFinding ,"disc_decayModeFinding");
  AddBranch(&disc_decayModeFindingNewDMs ,"disc_decayModeFindingNewDMs");
  //AddBranch(&disc_decayModeFindingOldDMs ,"disc_decayModeFindingOldDMs");
  
  AddBranch(&disc_chargedIsoPtSum ,"disc_chargedIsoPtSum");
  AddBranch(&disc_neutralIsoPtSum ,"disc_neutralIsoPtSum");
  AddBranch(&disc_puCorrPtSum ,"disc_puCorrPtSum"); 


  
  AddBranch(&HPSTau_NewVz,"HPSTau_NewVz");
  AddBranch(&HPSTau_charge,"HPSTau_charge");
 
  if(debug_)    std::cout<<"set branches"<<std::endl;
}

void hpstauInfo::Clear(){
  if(debug_)std::cout<<"clearing HpsTau info"<<std::endl;
  HPSTau_n =0;
  HPSTau_4Momentum->Clear();
  HPSTau_Vposition->Clear();
  taupt.clear();

  HPSTau_leadPFChargedHadrCand.clear();
  HPSTau_leadPFChargedHadrCand_trackRef.clear();

  
  
  disc_againstElectronLoose.clear();                                 
  disc_againstElectronMedium.clear();
  disc_againstElectronTight.clear();
  disc_againstElectronLooseMVA5.clear();
  disc_againstElectronMediumMVA5.clear();
  disc_againstElectronTightMVA5.clear();
  disc_againstElectronVLooseMVA5.clear();
  disc_againstElectronVTightMVA5.clear();
  disc_againstMuonLoose.clear();
  disc_againstMuonMedium.clear();
  disc_againstMuonTight.clear();
  disc_againstMuonLoose2.clear();
  disc_againstMuonMedium2.clear();
  disc_againstMuonTight2.clear();
  disc_againstMuonLooseMVA.clear();
  disc_againstMuonMediumMVA.clear();
  disc_againstMuonTightMVA.clear();
  disc_againstMuonLoose3.clear();
  disc_againstMuonTight3.clear();
  
  disc_byVLooseCombinedIsolationDeltaBetaCorr.clear();
  disc_byLooseCombinedIsolationDeltaBetaCorr.clear();
  disc_byMediumCombinedIsolationDeltaBetaCorr.clear();
  disc_byTightCombinedIsolationDeltaBetaCorr.clear();
  
  disc_byLooseIsolation.clear();
  
  disc_byVLooseIsolationMVA3newDMwLT.clear();
  disc_byLooseIsolationMVA3newDMwLT.clear();
  disc_byMediumIsolationMVA3newDMwLT.clear();
  disc_byTightIsolationMVA3newDMwLT.clear();
  disc_byVTightIsolationMVA3newDMwLT.clear();
  disc_byVVTightIsolationMVA3newDMwLT.clear();
  
  disc_byVLooseIsolationMVA3newDMwoLT.clear();
  disc_byLooseIsolationMVA3newDMwoLT.clear();
  disc_byMediumIsolationMVA3newDMwoLT.clear();
  disc_byTightIsolationMVA3newDMwoLT.clear();
  disc_byVTightIsolationMVA3newDMwoLT.clear();
  disc_byVVTightIsolationMVA3newDMwoLT.clear();
  
  disc_byVLooseIsolationMVA3oldDMwLT.clear();
  disc_byLooseIsolationMVA3oldDMwLT.clear();
  disc_byMediumIsolationMVA3oldDMwLT.clear();
  disc_byTightIsolationMVA3oldDMwLT.clear();
  disc_byVTightIsolationMVA3oldDMwLT.clear();
  disc_byVVTightIsolationMVA3oldDMwLT.clear();
  
  disc_byVLooseIsolationMVA3oldDMwoLT.clear();
  disc_byLooseIsolationMVA3oldDMwoLT.clear();
  disc_byMediumIsolationMVA3oldDMwoLT.clear();
  disc_byTightIsolationMVA3oldDMwoLT.clear();
  disc_byVTightIsolationMVA3oldDMwoLT.clear();
  disc_byVVTightIsolationMVA3oldDMwoLT.clear();
  
  disc_byLooseCombinedIsolationDeltaBetaCorr3Hits.clear();
  disc_byMediumCombinedIsolationDeltaBetaCorr3Hits.clear();
  disc_byTightCombinedIsolationDeltaBetaCorr3Hits.clear();
  
  disc_decayModeFinding.clear();
  disc_decayModeFindingNewDMs.clear();
  //disc_decayModeFindingOldDMs.clear();
  
  disc_chargedIsoPtSum.clear();
  disc_neutralIsoPtSum.clear();
  disc_puCorrPtSum.clear(); 

  HPSTau_NewVz.clear();
  HPSTau_charge.clear();
  
  if(debug_) std::cout<<"cleared"<<std::endl;
}

