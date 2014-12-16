#ifndef __UTILS_H_
#define __UTILS_H_
#include "TLorentzVector.h"
#include "DataFormats/RecoCandidate/interface/RecoCandidate.h"
#include <vector>
#include <string>
#include <map>
class PtGreater {
  public:
  template <typename T> bool operator () (const T& i, const T& j) {
    return (i.pt() > j.pt());
  }
};


TLorentzVector Part2LorVec(reco::Candidate& cand);
int PassAll(std::map<std::string, Bool_t> cutrecd);
int PassAllBut(std::string, std::map<std::string, Bool_t>);

#endif

