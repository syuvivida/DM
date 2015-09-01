
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TString.h>
#include <TH1D.h>
#include <TFile.h>

using namespace std;
void errmc(float nsig,float ntotal,float factor=1)
{
  float eff = nsig/ntotal;
  float err = sqrt( (1-eff)*eff/ntotal);
  cout << "efficiency = " << eff*factor << " +- " << err*factor << endl;

}

void xAna_eff(std::string inputFile){

 TH1F* h1;
 TH1F* h2;

 TH1F* h1_2;
 TH1F* h2_2;

 TString inputFile2=gSystem->GetFromPipe(Form("file=%s; test=${file##*histo_}; echo \"histo_2jets_${test}\"",inputFile.data()));

 TFile *f1 = TFile::Open(inputFile.data());
 TFile *f2 = TFile::Open(inputFile2.Data());

 h1  = (TH1F*)(f1->Get("h_njet"));
 h2  = (TH1F*)(f2->Get("h_njet"));
 h1_2= (TH1F*)(f1->Get("h_njet30"));
 h2_2= (TH1F*)(f2->Get("h_njet30"));

 // efficiency for jet pt  > 20 GeV
 cout << "Efficiency for jet pt > 20 GeV " << endl;

 cout << "0-jet ";
 errmc(h1->Integral(0,1),h1->Integral());
 cout << "extra-jet ";
 errmc(h2->Integral(0,1),h2->Integral());

 cout << endl;

 cout << "Allowing one extra jet" << endl;
 cout << "0-jet ";
 errmc(h1->Integral(0,2),h1->Integral());
 cout << "extra-jet ";
 errmc(h2->Integral(0,2),h2->Integral());
 
 cout << endl << endl;
 cout << "Efficiency for jet pt > 30 GeV " << endl;

 cout << "0-jet ";
 errmc(h1_2->Integral(0,1),h1_2->Integral());
 cout << "extra-jet ";
 errmc(h2_2->Integral(0,1),h2_2->Integral());

 cout << endl;

 cout << "Allowing one extra jet" << endl;
 cout << "0-jet ";
 errmc(h1_2->Integral(0,2),h1_2->Integral());
 cout << "extra-jet ";
 errmc(h2_2->Integral(0,2),h2_2->Integral());


}
