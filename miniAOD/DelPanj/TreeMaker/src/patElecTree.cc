#include "DelPanj/TreeMaker/interface/patElecTree.h"
// for conversion finder
#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"



/*
  NOTE: For the users of particle flow.

  Really a large number of issues have to be 
  explained for the case of the particle flow
  electrons. This module at the moment treats
  the electrons as if they were the non particle flow
  objects. We will make all our selections similar to 
  the regular objects. Particle flow electrons need 
  deeper study.
*/



patElecTree::patElecTree(std::string name, TTree* tree, const edm::ParameterSet& iConfig):
  baseTree(name,tree),
  pvSrc_    (iConfig.getParameter<edm::InputTag>("pvSrc") ), 
  patElecLabel_ ( iConfig.getParameter<edm::InputTag>("patElectrons"))
{
  SetBranches();
}


patElecTree::~patElecTree(){

} 



void
patElecTree::Fill(const edm::Event& iEvent){
  Clear();
  edm::Handle<pat::ElectronCollection> patElecHandle;
  if(not iEvent.getByLabel(patElecLabel_,patElecHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "
	     <<patElecLabel_<<std::endl; exit(0);}

  edm::Handle< std::vector<reco::Vertex> > recVtxs_;
  reco::Vertex myPv;

  Int_t nVtx=0;
  Int_t ngoodVtx=0;
  if (  iEvent.getByLabel( pvSrc_, recVtxs_ )) {
    for (size_t i=0; i<recVtxs_->size(); ++i) {
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
  

  pat::ElectronCollection eleColl(*(patElecHandle.product()));
  std::sort(eleColl.begin(),eleColl.end(),PtGreater());

  pat::ElectronCollection::const_iterator ele;

  
  for(ele=eleColl.begin(); ele!=eleColl.end(); ele++){

    nEle_++;
    patElecEt_.push_back(ele->et());
    patElecEnergy_.push_back(ele->energy());
    patElecPt_.push_back(ele->p4().pt());
    patElecEta_.push_back(ele->eta());
    patElecPhi_.push_back(ele->phi());
    patElecM_.push_back(ele->mass());

    Float_t supercluster_eta =-999;    
    supercluster_eta = ele->superCluster()->eta();
    patElecScEta_.push_back(supercluster_eta);

     
     
    //     Float_t supercluster_e =-999;
    Float_t sigihih = -999;
    Float_t deletain = -999;
    Float_t delphiin = -999;
    Float_t hoe = -999;


    sigihih          = ele->sigmaIetaIeta();
    delphiin         = ele->deltaPhiSuperClusterTrackAtVtx();
    deletain         = ele->deltaEtaSuperClusterTrackAtVtx();
    hoe              = ele->hadronicOverEm();
    
    //     //Don't know how this gonna fare in case of track-only PFElectrons.
    patElecSigIhIh_.push_back(sigihih);
    patElecDelEtaIn_.push_back(deletain);
    patElecDelPhiIn_.push_back(delphiin);
    patElecHoE_.push_back(hoe);

    patElecTrkIso_.push_back(ele->dr03TkSumPt());
    patElecHcalIso_.push_back(ele->dr03HcalTowerSumEt());
    patElecEcalIso_.push_back(ele->dr03EcalRecHitSumEt());
    Float_t ooemoop = fabs(
			  1.0/TMath::Max((Float_t)1e-3,(Float_t)ele->ecalEnergy()) - 
			  1.0/TMath::Max((Float_t)1e-3,(Float_t)sqrt(ele->trackMomentumAtVtx().mag2()))
			  );

    patElecEoverP_.push_back(ooemoop);
    patElecDxy_.push_back(ele->gsfTrack().get()->dxy(myPv.position()));
    patElecDz_.push_back(ele->gsfTrack().get()->dz(myPv.position()));




    patElecInBarrel_.push_back(ele->isEB());
    patElecInEndcap_.push_back(ele->isEE());
    patElecPassConv_.push_back(ele->passConversionVeto());
    

    Float_t iso1 = 999.;
    Float_t iso2 = 999.;
    Float_t iso3 = 999.;
    Float_t isoPU = -999.; 
  
    iso1  =  ele->pfIsolationVariables().sumChargedHadronPt;
    iso2  =  ele->pfIsolationVariables().sumNeutralHadronEt;
    iso3  =  ele->pfIsolationVariables().sumPhotonEt;
    isoPU =  ele->pfIsolationVariables().sumPUPt;    
    Float_t corrPfIso = iso1 + std::max(0.0, iso2 + iso3 - 0.5*isoPU );

    patElecChHadIso_.push_back(iso1);
    patElecNeHadIso_.push_back(iso2);
    patElecGamIso_.push_back(iso3);
    patElecPUPt_.push_back(isoPU);

    patElecCorrPfIso_.push_back(corrPfIso);



  }
}






void 
patElecTree::SetBranches(){

  AddBranch(&nEle_, "nEle");
  AddBranch(&eleRho_, "eleRho");
  AddBranch(&patElecEt_, "eleEt");
  AddBranch(&patElecEnergy_, "eleEnergy");
  AddBranch(&patElecPt_, "elePt");
  AddBranch(&patElecEta_, "eleEta");
  AddBranch(&patElecPhi_, "elePhi");
  AddBranch(&patElecM_, "eleM");
  AddBranch(&patElecScEta_, "eleScEta");
  AddBranch(&patElecSigIhIh_, "eleSigIhIh");
  AddBranch(&patElecDelEtaIn_, "eleDelEtaIn");
  AddBranch(&patElecDelPhiIn_, "eleDelPhiIn");
  AddBranch(&patElecHoE_, "eleHoE");
  AddBranch(&patElecTrkIso_, "eleTrkIso");
  AddBranch(&patElecHcalIso_, "eleHcalIso");
  AddBranch(&patElecEcalIso_, "eleEcalIso");
  



  AddBranch(&patElecEoverP_, "eleEoverP");

  AddBranch(&patElecDxy_, "eleDxy");
  AddBranch(&patElecDz_, "eleDz");

  AddBranch(&patElecChHadIso_, "eleChHadIso");
  AddBranch(&patElecNeHadIso_, "eleNeHadIso");
  AddBranch(&patElecGamIso_, "eleGamIso");
  AddBranch(&patElecPUPt_, "elePUPt");
  AddBranch(&patElecCorrPfIso_, "eleCorrPfIso");
  
  // conversion rejection
  AddBranch(&patElecInBarrel_,"eleInBarrel");
  AddBranch(&patElecInEndcap_,"eleInEndcap");

  AddBranch(&patElecPassConv_,"elePassConv");


}


void
patElecTree::Clear(){
  nEle_ =0;
  patElecEt_.clear();
  patElecEnergy_.clear();
  patElecPt_.clear();
  patElecEta_.clear();
  patElecPhi_.clear();
  patElecM_.clear();
  patElecScEta_.clear();
  patElecSigIhIh_.clear();
  patElecDelEtaIn_.clear();
  patElecDelPhiIn_.clear();
  patElecHoE_.clear();
  patElecTrkIso_.clear();
  patElecHcalIso_.clear();
  patElecEcalIso_.clear();
  patElecEoverP_.clear();
  patElecDxy_.clear();
  patElecDz_.clear();
  patElecChHadIso_.clear();
  patElecNeHadIso_.clear();
  patElecGamIso_.clear();
  patElecPUPt_.clear();
  patElecCorrPfIso_.clear();


  
  patElecPassConv_.clear();
  patElecInBarrel_.clear();
  patElecInEndcap_.clear();


}

















