#ifndef __BASE_TREE_H__
#define __BASE_TREE_H__

/*
Anil Singh
Panjab University

Log: This is a base class to define the common members 
for all the individual object fillers. 

*/

#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include "TTree.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Framework/interface/Event.h"

typedef edm::ParameterSet pset;

class baseTree{
 public:
  baseTree(std::string identifier, TTree* tree);
 ~baseTree(){delete tree_;};

 protected:
  baseTree(){}
  TTree* tree_;
  std::string identifier_; //eg. "pat"/"reco"

  void AddBranch(Float_t* x, std::string name);
  void AddBranch(Int_t* x, std::string name);
  void AddBranch(std::vector<Float_t>* vec, std::string name);
  void AddBranch(std::vector<Int_t>* vec, std::string name);
  void AddBranch(std::vector<std::string>* vec, std::string name);
  //vectors of vectors
  void AddBranch(std::vector<std::vector<Float_t>>* vec, std::string name);
  void AddBranch(std::vector<std::vector<Int_t>>* vec, std::string name);

  };


#endif
