#include "stackHisto.C"
#include <vector>
#include <string>

using namespace std;

void call_stackHisto(std::string dirname)
{
  vector<string> files;
  vector<string> legends;
  vector<string> histos;
  files.clear();
  legends.clear();

  // compare data and MC
  files.push_back(Form("%s/singlemuon.root",dirname.data()));
  files.push_back(Form("%s/gencut100recocut150_met_zmm.root",dirname.data()));
  files.push_back(Form("%s/gencut100recocut150_met_tt.root",dirname.data()));
  
  legends.push_back("Data");
  legends.push_back("Z/#gamma^{*}(#rightarrow#mu#mu)+jets");
  legends.push_back("t#bar{t}");

  histos.clear();
  histos.push_back("h_mz_data_total");
  stackHisto(files,histos,legends,"M_{#mu#mu} [GeV]");

  histos.clear();
  histos.push_back("h_recmet_after_data_total");
  stackHisto(files,histos,legends,"#slash{E}_{T} [GeV]");

  histos.clear();
  histos.push_back("h_lpt_data0_total");
  stackHisto(files,histos,legends,"p_{T}^{#mu1} [GeV]");

  histos.clear();
  histos.push_back("h_lpt_data1_total");
  stackHisto(files,histos,legends,"p_{T}^{#mu2} [GeV]");

  // compare test sample and corrected met 1D
  files.clear();
  legends.clear();
  histos.clear();

  files.push_back(Form("%s/final_Znn.root",dirname.data()));
  files.push_back(Form("%s/final_Znn.root",dirname.data()));

  legends.push_back("Z#rightarrow #nu #nu + jets Test sample");
  legends.push_back("1-D Corrected Z#rightarrow #mu #mu MC");

  histos.push_back("testmet_mc");
  histos.push_back("outputmet1D");
  stackHisto(files,histos,legends,"#slash{E}_{T} [GeV]","1DmetMC");

  legends.clear();
  histos.clear();

  legends.push_back("Z#rightarrow #nu #nu + jets Test sample");
  legends.push_back("2-D Corrected Z#rightarrow #mu #mu MC");

  histos.push_back("testmet_mc");
  histos.push_back("outputmet2D");
  stackHisto(files,histos,legends,"#slash{E}_{T} [GeV]","2DmetMC");

  legends.clear();
  histos.clear();

  legends.push_back("Z#rightarrow #nu #nu + jets Test sample");
  legends.push_back("1-D Corrected Z#rightarrow #mu #mu data");

  histos.push_back("testmet_data");
  histos.push_back("outputmetdata");
  stackHisto(files,histos,legends,"#slash{E}_{T} [GeV]","1Dmetdata");


}
