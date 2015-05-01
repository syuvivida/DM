
#include "DelPanj/TreeMaker/interface/photonTree.h"

photonTree::photonTree(std::string name, TTree* tree, const pset& iConfig):baseTree(name,tree){
  photonTreeLabel_  = iConfig.getParameter<edm::InputTag> ("photonLabelPY");
 // usePFObjects_ = iConfig.getParameter<bool> ("usePFlow");
 rho25C           = iConfig.getParameter<InputTag>("rho25PY");  
 SetBranches();
}

photonTree::~photonTree(){

}

void photonTree::Fill(const edm::Event& iEvent){
  Clear();
  //fetch the input collection
  edm::Handle<reco::PhotonCollection>  photonHandle;
  if(not iEvent.getByLabel(photonTreeLabel_,photonHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "<<photonTreeLabel_<<std::endl; 
    exit(0);
  }  
  reco::PhotonCollection phColl(*(photonHandle.product()));

 //sort the objects by transverse momentum
  std::sort(phColl.begin(),phColl.end(),PtGreater());


  reco::PhotonCollection::const_iterator ph;
  for(ph=phColl.begin(); ph!=phColl.end(); ph++){
    nPho_++;
    photonPt_.push_back(ph->pt());
    photonEta_.push_back(ph->eta());
    photonPhi_.push_back(ph->eta());
    photonSigmaIetaIeta_.push_back(ph->sigmaIetaIeta());
    photonhadronicOverEm_.push_back(ph->hadronicOverEm());
    photonecalRecHitSumEtConeDR04_.push_back(ph->ecalRecHitSumEtConeDR04());
    photonhcalTowerSumEtConeDR04_.push_back(ph->hcalTowerSumEtConeDR04());
    photontrkSumPtHollowConeDR04_.push_back(ph->trkSumPtHollowConeDR04());

    //saveing fast jet rho correction
        edm::Handle<Double_t> rhoHandle25;
        iEvent.getByLabel(rho25C, rhoHandle25);
       
        rho25_ = *(rhoHandle25.product());
    

  }  

}

void photonTree::SetBranches(){
  AddBranch(&rho25_  ,"rho25");
  AddBranch(&nPho_  ,"nPho");
  AddBranch(&photonPt_ ,"phoPt");
  AddBranch(&photonEta_,"phoEta");
  AddBranch(&photonPhi_,"phoPhi");
  AddBranch(&photonSigmaIetaIeta_,"photonSigmaIetaIeta");
  AddBranch(&photonecalRecHitSumEtConeDR04_,"photonecalRecHitSumEtConeDR04");
  AddBranch(&photonhadronicOverEm_ ,"photonhadronicOverEm");
  AddBranch(&photonhcalTowerSumEtConeDR04_ ,"photonhcalTowerSumEtConeDR04");
  AddBranch(&photontrkSumPtHollowConeDR04_ ,"photontrkSumPtHollowConeDR04");





}

void photonTree::Clear(){
  nPho_ = 0; 
  rho25_= 0;
  photonPt_.clear();
  photonEta_.clear();
  photonPhi_.clear();
  photonSigmaIetaIeta_.clear();
  photonhadronicOverEm_.clear();
  photonecalRecHitSumEtConeDR04_.clear();
  photonhcalTowerSumEtConeDR04_.clear();
  photontrkSumPtHollowConeDR04_.clear();




}
