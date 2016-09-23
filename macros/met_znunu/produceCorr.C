#include <TEfficiency.h>
#include <TH1.h>
#include <TH2.h>
#include <iostream>
#include <string>
#include <TFile.h>
#include <TMatrixD.h>
using namespace std;

void produceCorr(std::string znnfile, std::string zmmfile, std::string outputfile)
{

  TEfficiency* eff_trig_nunu;
  TEfficiency* eff_trig_ref;
  
  TH1F* eff_nunu; // full acceptance and trigger efficiency
  TH1F* eff_ref; // full acceptance and trigger efficiency
  TH1F* final_corr;
  
  TH1F* effinput_nunu[3];
  TH1F* effinput_ref[3];

  TH2F* genrec_nunu_deno;
  TH2F* genrec_nunu_numr;
  TH2F* genrec_ref_deno;
  TH2F* genrec_ref_numr;


  TFile *inf = TFile::Open(znnfile.data());
  genrec_nunu_deno = (TH2F*)inf->FindObjectAny("h_genrec_deno_split0_total");
  genrec_nunu_deno ->SetName("genrec_nunu_deno");
  genrec_nunu_numr = (TH2F*)inf->FindObjectAny("h_genrec_numr_split0_total");
  genrec_nunu_numr ->SetName("genrec_nunu_numr");

  effinput_nunu[0] = (TH1F*)inf->FindObjectAny("h_genmet_pre_split0_total");
  effinput_nunu[1] = (TH1F*)inf->FindObjectAny("h_trigmet_split0_total");
  effinput_nunu[2] = (TH1F*)inf->FindObjectAny("h_recmet_after_split0_total");

  eff_trig_nunu = new TEfficiency(*effinput_nunu[2], *effinput_nunu[1]);
  eff_trig_nunu->SetName("eff_trig_nunu");
  eff_trig_nunu->SetTitle("MET trigger efficiency");

  eff_nunu = (TH1F*)effinput_nunu[2]->Clone("eff_nunu");
  eff_nunu->Reset();
  eff_nunu->SetTitle("Z#rightarrow #nu #nu acceptance + efficiency");
  eff_nunu->SetXTitle("Generator-level #slash{E}_{T} [GeV]");
  eff_nunu->Divide(effinput_nunu[2],effinput_nunu[0],1,1,"B");

  TFile *inf2 = TFile::Open(zmmfile.data());

  genrec_ref_deno = (TH2F*)inf2->FindObjectAny("h_genrec_deno_split0_total");
  genrec_ref_deno->SetName("genrec_ref_deno");
  genrec_ref_numr = (TH2F*)inf2->FindObjectAny("h_genrec_numr_split0_total");
  genrec_ref_numr->SetName("genrec_ref_numr");

  effinput_ref[0] = (TH1F*)inf2->FindObjectAny("h_genmet_pre_split0_total");
  effinput_ref[1] = (TH1F*)inf2->FindObjectAny("h_trigmet_split0_total");
  effinput_ref[2] = (TH1F*)inf2->FindObjectAny("h_recmet_after_split0_total");

  eff_trig_ref = new TEfficiency(*effinput_ref[2], *effinput_ref[1]);
  eff_trig_ref->SetName("eff_trig_ref");
  eff_trig_ref->SetTitle("Single muon trigger efficiency");

  eff_ref = (TH1F*)effinput_ref[2]->Clone("eff_ref");
  eff_ref->Reset();
  eff_ref->SetTitle("Z/#gamma^* #righarrow #mu #mu acceptance + efficiency");
  eff_ref->SetXTitle("Generator-level #slash{E}_{T} [GeV]");
  eff_ref->Divide(effinput_ref[2],effinput_ref[0],1,1,"B");


  // branching ratio diffrences are not yet included
  final_corr =  (TH1F*)effinput_nunu[2]->Clone("final_corr");
  final_corr->Reset();
  final_corr->Sumw2();
  final_corr->Divide(effinput_nunu[2],effinput_ref[2]); // already include BR differencex

  const int histobins = genrec_ref_deno->GetNbinsX();
  const int nbins=histobins + 2;

  // Z-> mu mu unfolding matrix
  TMatrixD ref_GenToRec(nbins,nbins);

  for(int i=0; i<nbins; i++)
    for(int j=0; j<nbins; j++)
      {// normalize to the split0_total number of events in the same genmet bin
	float sum = genrec_ref_deno->Integral(j,j,0,histobins+1);
	ref_GenToRec(i,j)= sum >0?
	  genrec_ref_numr->GetBinContent(j,i)/sum:0;
      }

  //  ref_GenToRec.Print();

  // get the inverse
  TMatrixD ref_RecToGen=ref_GenToRec;
  double* det;
  ref_RecToGen.Invert(det);
  // ref_RecToGen.Print();


  TMatrixD nunu_GenToRec(nbins,nbins);

  for(int i=0; i<nbins; i++)
    for(int j=0; j<nbins; j++)
      {
	float sum = genrec_nunu_deno->Integral(j,j,0,histobins+1);
	nunu_GenToRec(i,j)=sum>0?
	  genrec_nunu_numr->GetBinContent(j,i)/sum:0;
      }
  // // nunu_GenToRec.Print();


  // // write output
  TFile* outFile = new TFile(outputfile.data(),"recreate");

   eff_trig_nunu->Write();
   eff_trig_ref->Write();

   eff_nunu->Write();
   eff_ref->Write();
   final_corr->Write();

   genrec_ref_deno->Write();
   genrec_ref_numr->Write();

   genrec_nunu_deno->Write();
   genrec_nunu_numr->Write();

   ref_RecToGen.Write("ref_RecToGen");
   ref_GenToRec.Write("ref_GenToRec");
   nunu_GenToRec.Write("nunu_GenToRec");

   outFile->Close();

}
