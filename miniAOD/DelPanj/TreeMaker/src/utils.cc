#include "DelPanj/TreeMaker/interface/utils.h"
//#include "TLorentzVector.h"
//#include <vector>
//#include <string>
//#include <map>

/*
class PtGreater {
  public:
  template <typename T> bool operator () (const T& i, const T& j) {
    return (i.pt() > j.pt());
  }
};
*/
TLorentzVector Part2LorVec(reco::Candidate& cand){
  TLorentzVector* l = new TLorentzVector();
  l->SetPtEtaPhiM(cand.pt(),cand.eta(),cand.phi(),cand.mass());
  return (*l);
}

//When Selectors are fully developed,this must move to baseSelector class.
Int_t PassAll(std::map<std::string, Bool_t> cutrecd){
  std::map<std::string, Bool_t>::iterator iter= cutrecd.begin();
  Bool_t decision =1 ;
  for(;iter!=cutrecd.end();iter++){
    //    std::cout<<"-->"<<iter->first<<"\t"<<iter->second<<std::endl;	   
    decision = decision&&iter->second;     
  }
  return (Int_t)decision;
}


Int_t PassAllBut(std::string tag, std::map<std::string, Bool_t> cutrecd){
  std::map<std::string, Bool_t>::iterator iter= cutrecd.begin();
  Bool_t decision =1 ;
  for(;iter!=cutrecd.end();iter++){
    if(iter->first==tag)continue;
    decision = decision&&iter->second;
  }
 return (Int_t)decision;  
}



