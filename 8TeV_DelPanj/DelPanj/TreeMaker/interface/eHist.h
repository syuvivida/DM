#ifndef __EHIST_H__
#define __EHIST_H__
#include "DelPanj/TreeMaker/interface/eSelector.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "TH1F.h"
class eHist{

public:
  eHist(std::string cutLevel,  edm::Service<TFileService>& theFile);
  ~eHist();
  void Fill(pat::Electron& e);
  void FillAfterCuts(pat::Electron& e, std::map<std::string, bool>& ewp);

 private:
  eHist();//dont allow the default constructor.
  TH1F* InitHist( std::string qty,  std::string cutLevel, std::string xlabel,
		  std::string title,int nbins, double rlow,
		  double rhigh,  edm::Service<TFileService>& theFile);

  TH1F* IniHist( TH1F* h, std::string qty,  std::string cutLevel, std::string xlabel,
                  std::string title,int nbins, double rlow,
                  double rhigh,  edm::Service<TFileService>& File);

  TH1F* hpt_; 
  TH1F* het_;
  TH1F* hphi_;
  TH1F* heta_; 
  TH1F* hsieiebarrel_; 
  TH1F* hdetainbarrel_;
  TH1F* hdphiinbarrel_;
  TH1F* hecalisobarrel_;
  TH1F* hhcalisobarrel_;
  TH1F* htrkisobarrel_; 
  TH1F* hdistbarrel_;   
  TH1F* hdcotbarrel_;   
  TH1F* hhoebarrel_;    
  TH1F* hnmishitsbarrel_; 
  
  
  TH1F* hsieieendcap_;  
  TH1F* hdetainendcap_; 
  TH1F* hdphiinendcap_; 
  TH1F* hecalisoendcap_;
  TH1F* hhcalisoendcap_;
  TH1F* htrkisoendcap_; 
  TH1F* hdistendcap_;   
  TH1F* hdcotendcap_;   
  TH1F* hhoeendcap_;    
  TH1F* hnmishitsendcap_; 
  TH1F* hisoendcap_;
  TH1F* hisobarrel_;
  
};


#endif


