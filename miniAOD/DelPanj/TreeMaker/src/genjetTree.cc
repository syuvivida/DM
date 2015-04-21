#include "DelPanj/TreeMaker/interface/genjetTree.h"
#include <CLHEP/Vector/LorentzVector.h>
#include "FWCore/Framework/interface/ESHandle.h"
#include "TMath.h"
#include "Math/VectorUtil.h"
#include "DataFormats/PatCandidates/interface/PackedGenParticle.h"
typedef math::XYZTLorentzVector LorentzVector;

genjetTree::genjetTree(std::string desc, TTree* tree, const edm::ParameterSet& iConfig):
  baseTree(desc, tree)
{
  GenJets_4Momentum  = new TClonesArray("TLorentzVector");
  
  SetBranches();
  
}


genjetTree::~genjetTree(){

}


void
genjetTree::Fill(const edm::Event& iEvent, edm::EventSetup const& iSetup){
  Clear();
  
  using namespace pat;
  // // Get the mean pt per unit area ("rho")                                     
  // edm::Handle< double > h_rho;
  // iEvent.getByLabel( rhoSrc_, h_rho );

  // // Get the primary vertex collection                                         
  edm::Handle< std::vector<reco::GenJet>> h_genjets;
  iEvent.getByLabel( "slimmedGenJets", h_genjets );

  std::vector<reco::GenJet>::const_iterator genjets = h_genjets->begin();   

  edm::Handle<pat::PackedGenParticleCollection> h_genParticles;
  iEvent.getByLabel("packedGenParticles", h_genParticles);
  const vector<pat::PackedGenParticle> *packedgenps_coll = h_genParticles.product();
  int nmu=0;
  for(vector<pat::PackedGenParticle>::const_iterator pkgenps_it = packedgenps_coll->begin(); pkgenps_it != packedgenps_coll->end(); pkgenps_it++) {
    //std::cout<<" pdgid = "<<pkgenps_it->pdgId()<<std::endl;
    //if(fabs(pkgenps_it->pdgId())==23){
    //for(unsigned int i=0 ; i < pkgenps_it->numberOfDaughters(); ++i){
    //packedID = fabs(pkgenps_it->daughter(i)->pdgId());
    int packedID = fabs(pkgenps_it->pdgId());
    if(packedID==13 ) nmu++;
    //}// end of for 
    // }// end of Z if
  }//ebd of packed candidate 
  if(nmu==2) std::cout<<" Z found = ----------"<<nmu<<std::endl;

  ngenMuons = nmu;  
  for(;genjets!=h_genjets->end();genjets++){
    std::cout<<" genjet info = "<<genjets->px()<<std::endl;
    
    TLorentzVector p4(genjets->px(),genjets->py(),genjets->pz(),genjets->energy());
    new( (*GenJets_4Momentum)[genjet_n] )  TLorentzVector(p4);
    //jetTau1_.push_back(jet->userFloat("tau1"));

    

    genjet_n++;
    
  } // end of loop over gen xsjets


}



void
genjetTree::SetBranches(){
  AddBranch(&ngenMuons, "ngenMuons");
  AddBranch(&genjet_n, "genjet_n");
  AddBranch(&GenJets_4Momentum, "GenJets_4Momentum");
}


void
genjetTree::Clear(){
  
  genjet_n = 0;
  ngenMuons = 0;
  GenJets_4Momentum->Clear();
}
