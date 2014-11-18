#include "DelPanj/TreeMaker/interface/jetTree.h"
#include <CLHEP/Vector/LorentzVector.h>
#include "FWCore/Framework/interface/ESHandle.h"
#include "CondFormats/JetMETObjects/interface/JetCorrectorParameters.h"
#include "TMath.h"
#include "Math/VectorUtil.h"

typedef math::XYZTLorentzVector LorentzVector;

jetTree::jetTree(std::string desc, TTree* tree, const edm::ParameterSet& iConfig):
  baseTree(desc, tree),
  isCA8Jet_(true),
  JetLabel_(iConfig.getParameter<edm::InputTag>(Form("%sJets",desc.data()))),
  PrunedJetLabel_ ( iConfig.getParameter<edm::InputTag>("PrunedJets")),
  rhoSrc_   (iConfig.getParameter<edm::InputTag>("rhoSrc") ),                     
  pvSrc_    (iConfig.getParameter<edm::InputTag>("pvSrc") ),                      
  jecPayloadNames_( iConfig.getParameter<std::vector<std::string> >(Form("%sjecPayloadNames",desc.data()) )), 
  jecUncName_( iConfig.getParameter<std::string>(Form("%sjecUncName",desc.data())) ),	
  jet2012ID_()
{
  
  std::size_t found = desc.find("CA8");
  if (found!=std::string::npos)
    isCA8Jet_=true;
  else
    isCA8Jet_=false;

  SetBranches();

  //Get the factorized jet corrector parameters.                                                                                                                        
  // std::vector<JetCorrectorParameters> vPar;
  // for ( std::vector<std::string>::const_iterator payloadBegin = 
  //  	  jecPayloadNames_.begin(),
  // 	  payloadEnd = jecPayloadNames_.end(), ipayload = payloadBegin; 
  //  	ipayload != payloadEnd; ++ipayload ) 
  //   {
  //     JetCorrectorParameters pars(*ipayload);
  //     vPar.push_back(pars);
  //   }

  // // Make the FactorizedJetCorrector and Uncertainty                                                                                                                    
  // jec_ = boost::shared_ptr<FactorizedJetCorrector> ( new FactorizedJetCorrector(vPar) );
  // jecUnc_ = boost::shared_ptr<JetCorrectionUncertainty>( new JetCorrectionUncertainty(jecUncName_) );


}


jetTree::~jetTree(){

}


void
jetTree::Fill(const edm::Event& iEvent, edm::EventSetup const& iSetup){
  Clear();

  // // Get the mean pt per unit area ("rho")                                     
  // edm::Handle< double > h_rho;
  // iEvent.getByLabel( rhoSrc_, h_rho );

  // // Get the primary vertex collection                                         
  edm::Handle< std::vector<reco::Vertex> > h_pv;
  iEvent.getByLabel( pvSrc_, h_pv );


  edm::Handle<pat::JetCollection> JetHandle;
  iEvent.getByLabel(JetLabel_,JetHandle);

  if(not iEvent.getByLabel(JetLabel_,JetHandle)){
    std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "
	     <<JetLabel_<<std::endl; exit(0);}

  // edm::Handle<std::vector<pat::Jet> > PrunedJetHandle;
  // iEvent.getByLabel(PrunedJetLabel_,PrunedJetHandle);

  // if(isCA8Jet_ && not iEvent.getByLabel(PrunedJetLabel_,PrunedJetHandle)){
  //   std::cout<<"FATAL EXCEPTION: "<<"Following Not Found: "
  // 	     <<PrunedJetLabel_<<std::endl; exit(0);}

  pat::JetCollection jets(*(JetHandle.product()));
  std::sort(jets.begin(),jets.end(),PtGreater());

  std::vector<pat::Jet>::const_iterator jet =jets.begin();   

  for(;jet!=jets.end();jet++){
    nJet_++;
    //Stuff common for all jets.

    jetTau1_.push_back(jet->userFloat("tau1"));
    jetTau2_.push_back(jet->userFloat("tau2"));
    jetTau3_.push_back(jet->userFloat("tau3"));
    jetTau4_.push_back(jet->userFloat("tau4"));
    
    // now making correction of jet energy
    // reco::Candidate::LorentzVector uncorrJet;
    // uncorrJet = jet->correctedP4(0);


    // // Get the correction itself. This needs the jet area,                     
    // // the rho value, and the number of primary vertices to                    
    // // run the correction.                                                     
    // jec_->setJetEta( uncorrJet.eta() );
    // jec_->setJetPt ( uncorrJet.pt() );
    // jec_->setJetE  ( uncorrJet.energy() );
    // jec_->setJetA  ( jet->jetArea() );
    // jec_->setRho   ( *(h_rho.product()) );
    // jec_->setNPV   ( h_pv->size() );

    // Float_t corr = jec_->getCorrection();

    // // Now access the uncertainty on the jet energy correction.                
    // // Pass the corrected jet pt to the "setJetPt" method.                     

    // // Access the "scale up" uncertainty (+1)                                  
    // jecUnc_->setJetEta( uncorrJet.eta() );
    // jecUnc_->setJetPt( corr * uncorrJet.pt() );
    // jetCorrUncUp_.push_back(jecUnc_->getUncertainty(1));

    // jecUnc_->setJetEta( uncorrJet.eta() );
    // jecUnc_->setJetPt( corr * uncorrJet.pt() );
    // jetCorrUncDown_.push_back(jecUnc_->getUncertainty(-1));

    jetPt_.push_back(jet->pt());
    jetEta_.push_back(jet->eta());
    jetPhi_.push_back(jet->phi());
    jetM_.push_back(jet->mass());
    jetEn_.push_back(jet->energy());


    jetCharge_.push_back(jet->charge());
    jetPartonFlavor_.push_back(jet->partonFlavour());
    
    std::map<std::string, bool> Pass = jet2012ID_.MergedJetCut(*jet);
    Int_t passOrNot = PassAll(Pass); 
    jetPassID_.push_back(passOrNot);
    jetSSV_.push_back(jet->bDiscriminator("simpleSecondaryVertexHighPurBJetTags"));
    jetCSV_.push_back(jet->bDiscriminator("combinedSecondaryVertexBJetTags"));        
    jetTCHP_.push_back(jet->bDiscriminator("trackCountingHighPurBJetTags"));
    jetTCHE_.push_back(jet->bDiscriminator("trackCountingHighEffBJetTags"));
    jetJP_.push_back(jet->bDiscriminator("jetProbabilityBJetTags"));
    jetJBP_.push_back(jet->bDiscriminator("jetBProbabilityBJetTags"));

    jetMuEF_.push_back(jet->muonEnergyFraction());
    jetPhoEF_.push_back(jet->photonEnergyFraction());
    jetCEmEF_.push_back(jet->chargedEmEnergyFraction());
    jetCHadEF_.push_back(jet->chargedHadronEnergyFraction());
    jetNEmEF_.push_back(jet->neutralEmEnergyFraction());
    jetNHadEF_.push_back(jet->neutralHadronEnergyFraction());
    jetCMulti_.push_back(jet->chargedMultiplicity());

    jetPrunedM_.push_back(jet->userFloat("ak8PFJetsCHSPrunedLinks"));

    // look for pruned jet
    if(!isCA8Jet_)continue; // stop looking for Prunedjets and subjets
    // Float_t dRmin = 99999.0;
    // Float_t prunedJetArea=0;

    // pat::JetCollection::const_iterator pj;
    // bool findPrunedJet=false;

    // reco::Candidate::LorentzVector uncorrPrunedJet(0,0,0,0);
    // if(iEvent.getByLabel(PrunedJetLabel_,PrunedJetHandle)){
    //   for(pat::JetCollection::const_iterator jetPruned = PrunedJetHandle->begin(); 
    // 	  jetPruned != PrunedJetHandle->end(); ++jetPruned) {
    // 	Float_t dRtmp = ROOT::Math::VectorUtil::DeltaR(jet->p4(),jetPruned->p4());

    // 	if(dRtmp<dRmin && dRtmp<0.8 ){//matching failed if greater than jet radius
    // 	  dRmin=dRtmp;
    // 	  findPrunedJet=true;
	  
    // 	  pj = jetPruned;

    // 	  uncorrPrunedJet = jetPruned->correctedP4(0);
    // 	  prunedJetArea = jetPruned->jetArea();


    // 	}
    //   } // end of loop over PrunedJets
    // } // if one could find CA8PrunedJet
    
    // // now making correction for pruned jets
    // if(findPrunedJet){

    //   jec_->setJetEta( uncorrPrunedJet.eta() );
    //   jec_->setJetPt ( uncorrPrunedJet.pt() );
    //   jec_->setJetE  ( uncorrPrunedJet.energy() );
    //   jec_->setJetA  ( prunedJetArea );
    //   jec_->setRho   ( *(h_rho.product()) );
    //   jec_->setNPV   ( h_pv->size() );
      

    //   Float_t corrPruned   = jec_->getCorrection();
      
    //   jecUnc_->setJetEta( uncorrPrunedJet.eta() );
    //   jecUnc_->setJetPt( corrPruned *uncorrPrunedJet.pt() );
    //   jetPrunedCorrUncUp_.push_back(jecUnc_->getUncertainty(+1));

    //   jecUnc_->setJetEta( uncorrPrunedJet.eta() );
    //   jecUnc_->setJetPt( corrPruned *uncorrPrunedJet.pt() );
    //   jetPrunedCorrUncDown_.push_back(jecUnc_->getUncertainty(-1));

    //   jetPrunedPt_.push_back(pj->pt());
    //   jetPrunedEta_.push_back(pj->eta());
    //   jetPrunedPhi_.push_back(pj->phi());
    //   jetPrunedM_.push_back(pj->mass());
    //   jetPrunedEn_.push_back(pj->energy());
    //   jetPrunedCharge_.push_back(pj->charge());
    //   jetPrunedPartonFlavor_.push_back(pj->partonFlavour());


    //   jetPrunedSSV_.push_back(pj->bDiscriminator("simpleSecondaryVertexHighPurBJetTags"));
    //   jetPrunedCSV_.push_back(pj->bDiscriminator("combinedSecondaryVertexBJetTags"));        
    //   jetPrunedTCHP_.push_back(pj->bDiscriminator("trackCountingHighPurBJetTags"));
    //   jetPrunedTCHE_.push_back(pj->bDiscriminator("trackCountingHighEffBJetTags"));
    //   jetPrunedJP_.push_back(pj->bDiscriminator("jetProbabilityBJetTags"));
    //   jetPrunedJBP_.push_back(pj->bDiscriminator("jetBProbabilityBJetTags"));

    //   nSubPrunedJet_ = pj->numberOfDaughters();

    //   for(int sj=0; sj< nSubPrunedJet_ ; ++sj ){
    // 	const pat::Jet* subjet = dynamic_cast<const pat::Jet*>(pj->daughter(sj));

  
    // 	subjetPrunedPt_.push_back(subjet->pt()); 
    // 	subjetPrunedEta_.push_back(subjet->eta()); 
    // 	subjetPrunedPhi_.push_back(subjet->phi());
    // 	subjetPrunedM_.push_back(subjet->mass());
    // 	subjetPrunedEn_.push_back(subjet->energy());
    // 	subjetPrunedCharge_.push_back(subjet->charge());
    // 	subjetPrunedPartonFlavor_.push_back(subjet->partonFlavour());

    // 	// subjetPrunedSSV_.push_back(subjet->bDiscriminator("simpleSecondaryVertexHighPurBJetTags")); 
    // 	subjetPrunedCSV_.push_back(subjet->bDiscriminator("combinedSecondaryVertexBJetTags"     )); 
    // 	// subjetPrunedTCHP_.push_back(subjet->bDiscriminator("trackCountingHighPurBJetTags"        )); 
    // 	// subjetPrunedTCHE_.push_back(subjet->bDiscriminator("trackCountingHighEffBJetTags"        ));
    // 	// subjetPrunedJP_.push_back(subjet->bDiscriminator("jetProbabilityBJetTags"              ));
    // 	// subjetPrunedJBP_.push_back(subjet->bDiscriminator("jetBProbabilityBJetTags"             ));       
      
    //   }// end of loop over prunedjets
    // }
    // else // if pruned jet is not found
    //   {

    // 	jetPrunedPt_.push_back(-9999.);
    // 	jetPrunedEta_.push_back(-9999.);
    // 	jetPrunedPhi_.push_back(-9999.);
    // 	jetPrunedM_.push_back(-9999.);
    // 	jetPrunedEn_.push_back(-9999.);
    // 	jetPrunedCharge_.push_back(-9999);
    // 	jetPrunedPartonFlavor_.push_back(-9999);

    // 	jetPrunedCorrUncUp_.push_back(-9999.);
    // 	jetPrunedCorrUncDown_.push_back(-9999.);

    // 	jetPrunedSSV_.push_back(-9999.);
    // 	jetPrunedCSV_.push_back(-9999.);        
    // 	jetPrunedTCHP_.push_back(-9999.);
    // 	jetPrunedTCHE_.push_back(-9999.);
    // 	jetPrunedJP_.push_back(-9999.);
    // 	jetPrunedJBP_.push_back(-9999.);

    // 	nSubPrunedJet_ = 0;

    //   }

    
  } // end of loop over jets


}



void
jetTree::SetBranches(){
  
  AddBranch(&nJet_, "nJet");
  AddBranch(&jetPt_, "jetPt");
  AddBranch(&jetEta_, "jetEta");
  AddBranch(&jetPhi_, "jetPhi");
  AddBranch(&jetM_, "jetMass");
  AddBranch(&jetEn_, "jetEn");

  AddBranch(&jetCorrUncUp_, "jetCorrUncUp");
  AddBranch(&jetCorrUncDown_, "jetCorrUncDown");
  AddBranch(&jetCharge_, "jetCharge");
  AddBranch(&jetPartonFlavor_, "jetPartonFlavor");
  AddBranch(&jetPassID_, "jetPassID");

  AddBranch(&jetSSV_, "jetSSV");
  AddBranch(&jetCSV_, "jetCSV");        
  AddBranch(&jetTCHP_, "jetTCHP");
  AddBranch(&jetTCHE_, "jetTCHE");
  AddBranch(&jetJP_, "jetJP");
  AddBranch(&jetJBP_, "jetJBP");

  AddBranch(&jetTau1_, "jetTau1");
  AddBranch(&jetTau2_, "jetTau2");
  AddBranch(&jetTau3_, "jetTau3");
  AddBranch(&jetTau4_, "jetTau4");

  AddBranch(&jetMuEF_, "jetMuEF");
  AddBranch(&jetPhoEF_, "jetPhoEF");
  AddBranch(&jetCEmEF_, "jetCEmEF");
  AddBranch(&jetCHadEF_, "jetCHadEF");
  AddBranch(&jetNEmEF_, "jetNEmEF");
  AddBranch(&jetNHadEF_, "jetNHadEF");
  AddBranch(&jetCMulti_, "jetCMulti");
  
  if(isCA8Jet_){
    AddBranch(&jetPrunedPt_, "jetPrunedPt");
    AddBranch(&jetPrunedEta_, "jetPrunedEta");
    AddBranch(&jetPrunedPhi_, "jetPrunedPhi");
    AddBranch(&jetPrunedM_, "jetPrunedMass");
    AddBranch(&jetPrunedEn_, "jetPrunedEn");
    AddBranch(&jetPrunedCorrUncUp_, "jetPrunedCorrUncUp");
    AddBranch(&jetPrunedCorrUncDown_, "jetPrunedCorrUncDown");
    AddBranch(&jetPrunedCharge_, "jetPrunedCharge");
    AddBranch(&jetPrunedPartonFlavor_, "jetPrunedPartonFlavor");


    AddBranch(&jetPrunedSSV_, "jetPrunedSSV");
    AddBranch(&jetPrunedCSV_, "jetPrunedCSV");        
    AddBranch(&jetPrunedTCHP_, "jetPrunedTCHP");
    AddBranch(&jetPrunedTCHE_, "jetPrunedTCHE");
    AddBranch(&jetPrunedJP_, "jetPrunedJP");
    AddBranch(&jetPrunedJBP_, "jetPrunedJBP");
	    
  AddBranch(&nSubPrunedJet_,"nSubPrunedJet");
  
  AddBranch(&subjetPrunedPt_, "subjetPrunedPt");
  AddBranch(&subjetPrunedEta_, "subjetPrunedEta");
  AddBranch(&subjetPrunedPhi_, "subjetPrunedPhi");
  AddBranch(&subjetPrunedM_, "subjetPrunedMass");
  AddBranch(&subjetPrunedEn_, "subjetPrunedEn");
  AddBranch(&subjetPrunedCharge_, "subjetPrunedCharge");
  AddBranch(&subjetPrunedPartonFlavor_, "subjetPrunedPartonFlavor");


  // AddBranch(&subjetPrunedSSV_, "subjetPrunedSSV");
  AddBranch(&subjetPrunedCSV_, "subjetPrunedCSV");        
  // AddBranch(&subjetPrunedTCHP_, "subjetPrunedTCHP");
  // AddBranch(&subjetPrunedTCHE_, "subjetPrunedTCHE");
  // AddBranch(&subjetPrunedJP_, "subjetPrunedJP");
  // AddBranch(&subjetPrunedJBP_, "subjetPrunedJBP");
  
  }

}


void
jetTree::Clear(){
  nJet_ = 0;
  jetPt_.clear();
  jetEta_.clear();
  jetPhi_.clear();
  jetM_.clear();
  jetEn_.clear();
  jetCorrUncUp_.clear();
  jetCorrUncDown_.clear();
  jetCharge_.clear();
  jetPartonFlavor_.clear();
  jetPassID_.clear();


  jetSSV_.clear();
  jetCSV_.clear();        
  jetTCHP_.clear();
  jetTCHE_.clear();
  jetJP_.clear();
  jetJBP_.clear();

  jetTau1_.clear();
  jetTau2_.clear();
  jetTau3_.clear();
  jetTau4_.clear();

  jetMuEF_.clear();
  jetPhoEF_.clear();
  jetCEmEF_.clear();
  jetCHadEF_.clear();
  jetNEmEF_.clear();
  jetNHadEF_.clear();
  jetCMulti_.clear();

  jetPrunedPt_.clear();
  jetPrunedEta_.clear();
  jetPrunedPhi_.clear();
  jetPrunedM_.clear();
  jetPrunedEn_.clear();
  jetPrunedCorrUncUp_.clear();
  jetPrunedCorrUncDown_.clear();
  jetPrunedCharge_.clear();
  jetPrunedPartonFlavor_.clear();


  jetPrunedSSV_.clear();
  jetPrunedCSV_.clear();        
  jetPrunedTCHP_.clear();
  jetPrunedTCHE_.clear();
  jetPrunedJP_.clear();
  jetPrunedJBP_.clear();


  nSubPrunedJet_ = 0;
  
  subjetPrunedPt_.clear();
  subjetPrunedEta_.clear();
  subjetPrunedPhi_.clear();
  subjetPrunedM_.clear();
  subjetPrunedEn_.clear();
  subjetPrunedCharge_.clear();
  subjetPrunedPartonFlavor_.clear();


  // subjetPrunedSSV_.clear();
  subjetPrunedCSV_.clear();        
  // subjetPrunedTCHP_.clear();
  // subjetPrunedTCHE_.clear();
  // subjetPrunedJP_.clear();
  // subjetPrunedJBP_.clear();
  


}
