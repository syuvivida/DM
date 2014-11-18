#include "DelPanj/TreeMaker/interface/patMetTree.h"

patMetTree::patMetTree(std::string name, TTree* tree, const edm::ParameterSet& iConfig):
  baseTree(name,tree)
{
  patMetRawLabel_ = iConfig.getParameter<edm::InputTag>("patMetRaw");
  patMetLabel_ = iConfig.getParameter<edm::InputTag>("patMet");
  SetBranches();
}


patMetTree::~patMetTree(){
} 


void
patMetTree::Fill(const edm::Event& iEvent){
  Clear();
  edm::Handle<pat::METCollection> patMetRawHandle;
  if(not iEvent.getByLabel(patMetRawLabel_,patMetRawHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "
	     <<patMetRawLabel_<<std::endl; exit(0);}
  edm::Handle<pat::METCollection> patMetHandle;
  if(not iEvent.getByLabel(patMetLabel_,patMetHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "
	     <<patMetLabel_<<std::endl; exit(0);}


  // raw distributions

  pat::METCollection::const_iterator met=patMetHandle.product()->begin();
  patMetCorrPt_    = met->et();
  patMetCorrPhi_   = met->phi();
  patMetCorrSumEt_ = met->sumEt();
  patMetCorrSig_   = met->significance() < 1.e10 ? met->significance() : 0;

  // corrected distributions
  pat::METCollection::const_iterator metraw=patMetRawHandle.product()->begin();
  
  patMetRawPt_ = metraw->et();
  patMetRawPhi_ = metraw->phi();
  patMetRawSumEt_ = metraw->sumEt();
  patMetRawCov00_ = metraw->getSignificanceMatrix()(0,0);
  patMetRawCov01_ = metraw->getSignificanceMatrix()(0,1);
  patMetRawCov10_ = metraw->getSignificanceMatrix()(1,0);
  patMetRawCov11_ = metraw->getSignificanceMatrix()(1,1);

  

} 



void 
patMetTree::SetBranches(){

  AddBranch(&patMetCorrPt_, "MetCorrPt");
  AddBranch(&patMetCorrPhi_, "MetCorrPhi"); 
  AddBranch(&patMetCorrSumEt_, "MetCorrSumEt");
  AddBranch(&patMetCorrSig_, "MetCorrSig");
  AddBranch(&patMetRawPt_, "MetRawPt");
  AddBranch(&patMetRawPhi_, "MetRawPhi");
  AddBranch(&patMetRawSumEt_, "MetRawSumEt");
  AddBranch(&patMetRawCov00_, "MetRawCov00");
  AddBranch(&patMetRawCov01_, "MetRawCov01");
  AddBranch(&patMetRawCov10_, "MetRawCov10");
  AddBranch(&patMetRawCov11_, "MetRawCov11");

}


void
patMetTree::Clear(){

  Float_t dummy = -99999;
  patMetCorrPt_= dummy;  
  patMetCorrPhi_= dummy; 
  patMetCorrSumEt_= dummy;
  patMetCorrSig_= dummy;
  patMetRawPt_= dummy;
  patMetRawPhi_= dummy;
  patMetRawSumEt_= dummy;
  patMetRawCov00_= dummy;
  patMetRawCov01_= dummy;
  patMetRawCov10_= dummy;
  patMetRawCov11_= dummy;


}
