
#include "DelPanj/TreeMaker/interface/eHist.h"
#include "DelPanj/TreeMaker/interface/utils.h"

eHist::eHist(std::string cutLevel,  edm::Service<TFileService>& theFile){
 
  hpt_  =  InitHist( "PTELE",cutLevel,"p_{T} [GeV]","ElectronTransverseMomentum",60,0,150.,theFile);
  het_  =  InitHist( "ETELE",cutLevel,"sc E_{T} [GeV]","ElectronSuperClusterEt", 60, 0, 150, theFile);
  hphi_ =  InitHist( "PHIELE",cutLevel,"#Phi","ElectronPhi",40,-4,4, theFile);
  heta_ =  InitHist( "ETAELE",cutLevel,"#eta","ElectronEta",40,-4,4, theFile);
  hsieiebarrel_  =InitHist("SIEIEBARREL",cutLevel,  "#sigma_{i#etai#eta} (barrel)","ElectronSigma_iEtaiEta_Barrel", 70,0.0,0.07, theFile);
  hdetainbarrel_ =InitHist("DETAINBARREL",cutLevel, "#Delta#eta_{in} (barrel)","ElectronDeltaEta_in_Barrel",100,-0.04,0.04, theFile);
  hdphiinbarrel_ =InitHist("DPHIINBARREL",cutLevel, "#Delta#phi_{in} (barrel)","ElectronDeltaPhi_in_Barrel",100,-0.5,0.5, theFile);
  hecalisobarrel_=InitHist("CHARHADISOBARREL",cutLevel,"IsoDeposit/p_{T} (barrel)", "RelativeCHARHAD_IsoDeposit_Barrel" ,100,0.0,0.5,theFile);
  hhcalisobarrel_=InitHist("NEUHADISOBARREL",cutLevel,"IsoDeposit/p_{T} (barrel)", "RelativeNEUTHAD_IsoDeposit_Barrel" ,100,0.0,0.5, theFile);
  htrkisobarrel_ =InitHist("PHOTISOBARREL",cutLevel, "IsoDeposit/p_{T} (barrel)", "RelativePHOTSumPt_Barrel",100,0.0,0.5, theFile);
  hdistbarrel_   =InitHist("DISTBARREL",cutLevel,   "Dist (barrel)", "ElectronDist_Barrel",50,-0.04,0.04, theFile);
  hdcotbarrel_   =IniHist(hdcotbarrel_,"DCOTBARREL",cutLevel,   "#DeltaCot(#theta) (barrel)", "ElectronDeltaCot_Theta_Barrel",50,-0.04,0.04, theFile);
  hhoebarrel_    =InitHist("HOEBARREL",cutLevel,    "H/E (barrel)", "ElectronHadOverEmFrac_Barrel",200,0.,1., theFile);
  hnmishitsbarrel_ = InitHist("NHITSMISBARREL",cutLevel,"nHits_{mis} (barrel)", "ElectronMissingHits_Barrel",5,0.,5., theFile);
  hisobarrel_=InitHist("COMBPFISOBARREL",cutLevel,"IsoDeposit/p_{T} (barrel)", "RelativeCOMBINEDPF_IsoDeposit_Barrel" ,100,0.0,0.5, theFile);


  hsieieendcap_  =InitHist("SIEIEENDCAP",cutLevel,  "#sigma_{i#etai#eta} (endcap)","ElectronSigma_iEtaiEta_Endcap", 70,0.0,0.07, theFile);
  hdetainendcap_ =InitHist("DETAINENDCAP",cutLevel, "#Delta#eta_{in} (endcap)","ElectronDeltaEta_in_Endcap",100,-0.04,0.04, theFile);
  hdphiinendcap_ =InitHist("DPHIINENDCAP",cutLevel, "#Delta#phi_{in} (endcap)","ElectronDeltaPhi_in_Endcap",100,-0.5,0.5, theFile);
  hecalisoendcap_=InitHist("CHARHADISOENDCAP",cutLevel,"IsoDeposit/p_{T} (endcap)", "RelativeCHARHAD_IsoDeposit_Endcap" ,100,0.0,0.5,theFile);
  hhcalisoendcap_=InitHist("NEUHADISOENDCAP",cutLevel,"IsoDeposit/p_{T} (endcap)", "RelativeNEUTHAD_IsoDeposit_Endcap" ,100,0.0,0.5, theFile);
  htrkisoendcap_ =InitHist("PHOTISOENDCAP",cutLevel, "IsoDeposit/p_{T} (endcap)", "RelativePHOTSumPt_Endcap",100,0.0,0.5, theFile);
  hdistendcap_   =InitHist("DISTENDCAP",cutLevel,   "Dist (endcap)", "ElectronDist_Endcap",50,-0.04,0.04, theFile);
  hdcotendcap_   =IniHist(hdcotendcap_,"DCOTENDCAP",cutLevel,   "#DeltaCot(#theta) (endcap)", "ElectronDeltaCot_Theta_Endcap",50,-0.04,0.04, theFile);
  hhoeendcap_    =InitHist("HOEENDCAP",cutLevel,    "H/E (endcap)", "ElectronHadOverEmFrac_Endcap",200,0.,1., theFile);
  hnmishitsendcap_ = InitHist("NHITSMISENDCAP",cutLevel,"nHits_{mis} (endcap)", "ElectronMissingHits_Endcap",5,0.,5., theFile);

  hisoendcap_=InitHist("COMBPFISOENDCAP",cutLevel,"IsoDeposit/p_{T} (endcap)", "RelativeCOMBINEDPF_IsoDeposit_Endcap" ,100,0.0,0.5, theFile);



}   
 

TH1F* eHist::InitHist(std::string qty,  std::string cutLevel, std::string xlabel ,
		      std::string title,int nbins, double rlow,
		      double rhigh,  edm::Service<TFileService>& theFile){
  TH1F* m;
  std::string name = "h_"+qty+"_"+cutLevel;
  title = title+"_"+cutLevel;
  m = theFile->make<TH1F>(name.c_str(),title.c_str(),nbins,rlow,rhigh);
  m->GetXaxis()->SetTitle(xlabel.c_str());
  m->GetXaxis()->SetRangeUser(rlow,rhigh);
 return m;
}

TH1F* eHist::IniHist(TH1F* m, std::string qty,  std::string cutLevel, std::string xlabel ,
                      std::string title,int nbins, double rlow,
                      double rhigh,  edm::Service<TFileService>& theFile){
  std::string name = "h_"+qty+"_"+cutLevel;
  title = title+"_"+cutLevel;
  m = theFile->make<TH1F>(name.c_str(),title.c_str(),nbins,rlow,rhigh);
  m->GetXaxis()->SetTitle(xlabel.c_str());
  m->GetXaxis()->SetRangeUser(rlow,rhigh);
  return m;
}

void eHist::Fill(pat::Electron& e){
}

void  eHist::FillAfterCuts(pat::Electron& e, std::map<std::string, bool>& ewp){

  if(PassAll(ewp))hpt_->Fill(e.pt());
  if(PassAll(ewp))heta_->Fill(e.eta());
  if(PassAll(ewp))het_->Fill(e.superCluster()->energy() * sin(2*atan(exp( e.superCluster()->eta() ) ) ));
  if(PassAll(ewp))hphi_ ->Fill(e.phi());

  if(e.isEB()){
    if(PassAllBut("sieie",ewp))hsieiebarrel_->Fill(e.scSigmaIEtaIEta());
    if(PassAllBut("detain",ewp))hdetainbarrel_->Fill(e.deltaEtaSuperClusterTrackAtVtx()); 
    if(PassAllBut("delphi",ewp))hdphiinbarrel_->Fill(e.deltaPhiSuperClusterTrackAtVtx());
    if(PassAllBut("dcot",ewp))hdcotbarrel_->Fill(e.convDcot());
    if(PassAllBut("dist",ewp))hdistbarrel_->Fill(e.convDist());
    if(PassAllBut("hoe",ewp))hhoebarrel_  ->Fill(e.hadronicOverEm());
    if(PassAllBut("nmshits",ewp))hnmishitsbarrel_->Fill( e.gsfTrack().get()->trackerExpectedHitsInner().numberOfHits());
    if(PassAllBut("iso4",ewp))hecalisobarrel_ ->Fill(e.chargedHadronIso()/e.pt());
    if(PassAllBut("iso4",ewp))hhcalisobarrel_ ->Fill(e.neutralHadronIso()/e.pt());
    if(PassAllBut("iso4",ewp))htrkisobarrel_  ->Fill(e.photonIso()/e.pt());
    if(PassAllBut("iso4",ewp))hisobarrel_->Fill((e.chargedHadronIso()+e.neutralHadronIso()+e.photonIso())/e.pt());
  }
  else if(e.isEE()){
    if(PassAllBut("sieie",ewp))hsieieendcap_->Fill(e.scSigmaIEtaIEta());
    if(PassAllBut("detain",ewp))hdetainendcap_->Fill(e.deltaEtaSuperClusterTrackAtVtx());
    if(PassAllBut("delphi",ewp))hdphiinendcap_->Fill(e.deltaPhiSuperClusterTrackAtVtx());
    if(PassAllBut("dcot",ewp))hdcotendcap_->Fill(e.convDcot());
    if(PassAllBut("dist",ewp))hdistendcap_->Fill(e.convDist());
    if(PassAllBut("hoe",ewp))hhoeendcap_  ->Fill(e.hadronicOverEm());
    if(PassAllBut("nmshits",ewp))hnmishitsendcap_->Fill( e.gsfTrack().get()->trackerExpectedHitsInner().numberOfHits());
    if(PassAllBut("iso4",ewp))hecalisoendcap_ ->Fill(e.chargedHadronIso()/e.pt());
    if(PassAllBut("iso4",ewp))hhcalisoendcap_ ->Fill(e.neutralHadronIso()/e.pt());
    if(PassAllBut("iso4",ewp))htrkisoendcap_  ->Fill(e.photonIso()/e.pt());
    if(PassAllBut("iso4",ewp))hisoendcap_->Fill((e.chargedHadronIso()+e.neutralHadronIso()+e.photonIso())/e.pt());
  }
}









