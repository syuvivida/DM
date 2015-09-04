#include <TROOT.h>
#include "TFile.h"
#include "TH1.h"
#include "TTree.h"
#include "TKey.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;
void scaleHistos(float datalumi,std::string input, std::string output, bool isData=false)
{
  FILE *fTable = fopen(input.data(),"r");
  int flag=1;
  int nfile=0;

  std::vector<string> infiles;
  std::vector<float> xsecs;
  std::vector<float> neve;
  while (flag!=-1){
    char filename[300];
    flag=fscanf(fTable,"%s",filename);
    std::string tempFile = filename;    

    char tmp[300];
    flag=fscanf(fTable,"%s",tmp);
    float xsec_temp=atof(tmp);

    flag=fscanf(fTable,"%s",tmp);
    float neve_temp=atof(tmp);

    // first reading input file
    if(flag!=-1){
      infiles.push_back(tempFile);
      xsecs.push_back(xsec_temp);
      neve.push_back(neve_temp);
      nfile++;
    }
  }

  cout << "There are " << nfile << " files " << endl;
  const int MAXNHISTOS=100;
  TH1* htotal[MAXNHISTOS];
  unsigned int nPage=0;

  for(unsigned i=0; i < infiles.size(); i++){

    float scale = datalumi*(xsecs[i]/neve[i]);
    cout << "scale " << i << " = " << scale << endl;

    TFile *_file = TFile::Open(infiles[i].data());
    _file->cd();
    nPage=0;
    TDirectory *current_sourcedir = gDirectory;
  // loop over all keys in this directory
     TIter nextkey( current_sourcedir->GetListOfKeys() );
     TKey *key;
     while (key = (TKey*)nextkey()) {
       if(nPage>=MAXNHISTOS)break;
       TObject *obj = key->ReadObj();
       if ( obj->IsA()->InheritsFrom( "TH1" ) ) {
	 // descendant of TH1 -> scale it

	 cout << "outputing histogram " << obj->GetName() << endl;

	 TH1 *h1 = (TH1*)obj;
	 h1->SetMarkerStyle(8);
	 h1->SetMarkerSize(1);
	 h1->SetLineWidth(3);
	 h1->SetLineColor(4);
	 h1->SetMinimum(0);
	 h1->Sumw2();

	 if(i==0){
	   htotal[nPage] = (TH1*)h1->Clone(Form("%s_total",(TH1*)obj->GetName()));
	   htotal[nPage]->Reset();
	   htotal[nPage]->SetMarkerStyle(8);
	   htotal[nPage]->SetMarkerSize(1);
	   htotal[nPage]->SetLineWidth(3);
	   htotal[nPage]->SetLineColor(4);
	   htotal[nPage]->SetMinimum(0);
	   htotal[nPage]->Sumw2();
	 }
	 if(!isData)h1->Scale(scale);
	 htotal[nPage]->Add(h1);
	 cout << htotal[nPage]->Integral() << endl;
	 nPage++;
       } // if the object is a histogram
     } // loop over keys
  } // end of loop over files

  cout << "nPage = " << nPage << endl;

  TFile* outFile = new TFile(output.data(),"recreate");

  for(unsigned int i=0;i<nPage;i++)
    htotal[i]->Write();
  
  outFile->Close();


}
