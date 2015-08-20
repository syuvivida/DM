// example code to produce MET from Z->nunu

#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TString.h>
#include <map>
#include <TH1D.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TSystemDirectory.h>
#include <TList.h>

using namespace std;
void xAna_metznn(std::string inputFile){

  //get TTree from file ...
  TString outputFile;
  std::vector<string> infiles;
  TString prefix;

  if(inputFile.find(".root")!= std::string::npos)
    { 
      prefix=gSystem->GetFromPipe(Form("file=%s; test=${file##*ZJetsNuNu/}; test2=${test%%/crab*}; echo \"${test2}\"",inputFile.data()));
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*NCUGlobalTuples_}; echo \"histo_%s_${test}\"",inputFile.data(),prefix.Data()));
      cout << "output file name = " << outputFile.Data() << endl;
      infiles.push_back(inputFile.data());
    }
  else
    {
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*ZJetsNuNu/}; test2=${test%%/crab*}; echo \"histo_${test2}_all.root\"",inputFile.data()));
      cout << "output file name = " << outputFile.Data() << endl;      
      TSystemDirectory *base = new TSystemDirectory("root","root");

      base->SetDirectory(inputFile.data());
      TList *listOfFiles = base->GetListOfFiles();
      TIter fileIt(listOfFiles);
      TFile *fileH = new TFile();
      int nfile=0;
      while(fileH = (TFile*)fileIt()) {
	std::string fileN = fileH->GetName();
	if( fileH->IsFolder())  continue;
	if(fileN.find("NCUGlobalTuples") == std::string::npos)continue;
	fileN = inputFile + "/" + fileN;
	cout << fileN.data() << endl;
	nfile++;
	infiles.push_back(fileN);
      }

      std::cout << "Opened " << nfile << " files" << std::endl;

    }
  
  TreeReader data(infiles);


  Long64_t nTotal=0;
  Long64_t nPass[20]={0};

  TH1F* h_met = new TH1F("h_met","",100,0,1000);

  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;
    float met        = data.GetFloat("pfMetRawPt");
    
    h_met->Fill(met);
  } // end of loop over entries

  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;

  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  h_met->Write();

  outFile->Close();

}
