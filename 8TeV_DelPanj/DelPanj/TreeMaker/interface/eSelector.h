#ifndef __ESELECTOR_H__
#define __ESELECTOR_H__

/*
  ElectronSelector
  Optimal Usage: pat Electrons.

  Anil Singh,
  Panjab University.
  Shin-Shan Eiko Yu,
  National Central University
*/



#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/Math/interface/LorentzVector.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "EGamma/EGammaAnalysisTools/interface/ElectronEffectiveArea.h"
#include "TMath.h"

class eSelector{
   
 public:
  eSelector(const edm::ParameterSet ps);  
  std::map<std::string, Bool_t> CutRecord(const pat::Electron& e);
  
  void SetRho(Double_t rho){rho_ = rho;}
  void SetVertex(reco::Vertex pv){pv_ = pv;}
  void SetData(Bool_t isData){isData_ = isData;}

  // remove the contribution of nearby-tau
  Double_t GetTauCorrElecPfIso(const pat::Electron & e)
  {
    Double_t iso1 = e.userIsolation(pat::PfChargedHadronIso);
    Double_t iso2 = e.userIsolation(pat::PfNeutralHadronIso);
    Double_t iso3 = e.userIsolation(pat::PfGammaIso);
    Double_t iso4 = iso1 + iso2 + iso3;
    return iso4;
  }

  // remove the contribution of pileups using the official recipe
  Double_t GetCorrElecPfIso(const pat::Electron &e)
  {
    Double_t iso1 = 999.;
    Double_t iso2 = 999.;
    Double_t iso3 = 999.;
    Double_t iso4 = 999.;

    iso1  =  e.chargedHadronIso();                                                                                                                                     
    iso2  =  e.neutralHadronIso();                                                                                                                                     
    iso3  =  e.photonIso();                                                                                                                                            

    ElectronEffectiveArea::ElectronEffectiveAreaTarget effAreaTarget_ = 
      ElectronEffectiveArea::kEleEAData2012;
      
    ElectronEffectiveArea::ElectronEffectiveAreaType effAreaType_ =
      ElectronEffectiveArea::kEleGammaAndNeutralHadronIso03;      
    
    Double_t eta = e.superCluster()->eta();
    Double_t AEff = ElectronEffectiveArea::GetElectronEffectiveArea
      (effAreaType_, fabs(eta), effAreaTarget_);
    iso4 = iso1 + TMath::Max((Double_t)0.0, iso2+iso3-rho_*AEff);
    
    //Calculate combined and beta corrected Iso                                                                                                   
    
    return iso4;
  }  
  ~eSelector(){}
  
  Double_t ptX_;
  Double_t etaX_;

  Double_t detainBrlX_;
  Double_t delphiBrlX_;  
  Double_t sieieBrlX_;
  Double_t hoeBrlX_;
  Double_t d0vtxBrlX_;
  Double_t dzvtxBrlX_;
  Double_t ooemoopBrlX_;
  Double_t passConvBrlX_;
  Double_t nmisHitBrlX_;

  Double_t detainEcpX_;
  Double_t delphiEcpX_;  
  Double_t sieieEcpX_;
  Double_t hoeEcpX_;
  Double_t d0vtxEcpX_;
  Double_t dzvtxEcpX_;
  Double_t ooemoopEcpX_;
  Double_t passConvEcpX_;
  Double_t nmisHitEcpX_;

  Double_t isoRelBrlX_;
  Double_t isoRelEcpX_;
  
  
  //some pset declaration
  edm::ParameterSet idBrl_;
  edm::ParameterSet idEcp_;

  Double_t rho_;
  Double_t isData_;
  reco::Vertex pv_;

};

#endif
