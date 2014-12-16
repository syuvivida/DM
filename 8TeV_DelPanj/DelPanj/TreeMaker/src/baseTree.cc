#include "DelPanj/TreeMaker/interface/baseTree.h"

baseTree::baseTree(std::string identifier, TTree* tree){
  identifier_  = identifier; 
  tree_ = tree;
}

void baseTree::AddBranch(std::vector<Float_t>* vec, std::string name){
  std::string brName = identifier_+name;
  tree_->Branch(brName.c_str(),vec);
}


void baseTree::AddBranch(std::vector<Int_t>* vec, std::string name){
  std::string brName = identifier_+name;
  tree_->Branch(brName.c_str(),vec);
}

void baseTree::AddBranch(std::vector<std::string>* vec, std::string name){
  std::string brName = identifier_+name;
  tree_->Branch(brName.c_str(),vec);
}

void baseTree::AddBranch(Float_t* x, std::string name){
  std::string brName = identifier_+name;
  tree_->Branch(brName.c_str(),x,(brName+"/F").c_str());
}

void baseTree::AddBranch(Int_t* x, std::string name){
  std::string brName = identifier_+name;
  tree_->Branch(brName.c_str(),x,(brName+"/I").c_str());
}


void baseTree::AddBranch(std::vector<std::vector<Float_t>>* vec, std::string name){
  std::string brName = identifier_+name;
  tree_->Branch(brName.c_str(),vec);
}

void baseTree::AddBranch(std::vector<std::vector<Int_t>>* vec, std::string name){
  std::string brName = identifier_+name;
  tree_->Branch(brName.c_str(),vec);
}

