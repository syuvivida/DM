#include "DelPanj/TreeMaker/interface/jetSelector.h"


jetSelector::jetSelector()      
{
}


std::map<std::string, Bool_t>  jetSelector::MergedJetCut(const pat::Jet& jet){
  
  std::map<std::string, Bool_t> cuts;
  Float_t eta = jet.eta();

  //Kinematic and Fiducial Acceptance
  cuts["etax"]        = fabs(eta) < 2.4;
  cuts["muEF"]        = jet.muonEnergyFraction() < 0.99;
  cuts["phoEF"]       = jet.photonEnergyFraction() < 0.99;
  cuts["cEmEF"]       = jet.chargedEmEnergyFraction() < 0.99;
  cuts["nHadEF"]      = jet.neutralHadronEnergyFraction() < 0.99;
  cuts["cHadEF"]      = jet.chargedHadronEnergyFraction() > 0.0;


  // // not sure if this is needed
  cuts["trkSWBug"]    = !(fabs(eta)>1.0 && fabs(eta)<1.5 && 
    			  jet.chargedMultiplicity()/
			  (TMath::Max((Float_t)0.1,(Float_t)jet.neutralMultiplicity()))>2.0);
  return cuts;
}

std::map<std::string, Bool_t>  jetSelector::LooseJetCut(const pat::Jet& jet){
  
  std::map<std::string, Bool_t> cuts;
  Float_t eta = jet.eta();

  //Kinematic and Fiducial Acceptance
  cuts["etax"]        = fabs(eta) < 2.4;
  cuts["nHadEF"]      = jet.neutralHadronEnergyFraction() < 0.99;
  cuts["nEmHF"]       = jet.neutralEmEnergyFraction() < 0.99;

  
  return cuts;
}



std::map<std::string, Bool_t>  jetSelector::TightJetCut(const pat::Jet& jet){

  std::map<std::string, Bool_t> cuts;
  Float_t eta = jet.eta();

  //Kinematic and Fiducial Acceptance
  cuts["etax"]        = fabs(eta) < 2.4;
  cuts["nHadEF"]      = jet.neutralHadronEnergyFraction() < 0.99;
  cuts["nEmHF"]       = jet.neutralEmEnergyFraction() < 0.99;
  cuts["cHadEF"]      = jet.chargedHadronEnergyFraction() > 0.0;
  cuts["cEmEF"]       = jet.chargedEmEnergyFraction() < 0.99;
  cuts["cMulti"]      = jet.chargedMultiplicity() > 0;

  return cuts;
}

