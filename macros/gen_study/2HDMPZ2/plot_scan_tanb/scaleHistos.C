#include <TROOT.h>
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include <string>
#include <iostream>
#include <TSystem.h>


using namespace std;

void scaleHistos(std::string inputFile_, double scale)
{


  //Create directory, output file
  // dump histogram to a root file
  std::string outputFile_ = "scaled_" + inputFile_;
  gSystem->mkdir("scaledHistos");
  Char_t strFullPath[500];
  sprintf(strFullPath,"scaledHistos/%s",outputFile_.data());

  TFile* fOutFile=new TFile(strFullPath,"RECREATE");

  cout << "Target path: " << fOutFile->GetPath() << endl;
  TString path( (char*)strstr( fOutFile->GetPath(), ":" ) );
  path.Remove( 0, 2 );
  cout << path << endl;
  TFile *_file = TFile::Open(inputFile_.data());
  _file->cd(path);

  TDirectory *current_sourcedir = gDirectory;

  // loop over all keys in this directory
  TIter nextkey( current_sourcedir->GetListOfKeys() );
  TKey *key;
  while (key = (TKey*)nextkey() ) {

    TObject *obj = key->ReadObj();
    if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
      // descendant of TH1 -> scale it

      cout << "Scaling histogram " << obj->GetName() << endl;
      TH1 *h1 = (TH1*)obj;
      double norm=scale/h1->GetEntries();
      h1->Sumw2();
      h1->Scale(norm);
      
    } // if the object is a histogram
    fOutFile->cd();
    obj->Write(key->GetName() );
  } // loop over keys


  //  fOutFile->Write();
  fOutFile->Close();

}
