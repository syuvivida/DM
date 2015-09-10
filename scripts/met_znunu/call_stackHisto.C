#include "stackHisto.C"
#include <vector>
#include <string>

using namespace std;

void call_stackHisto()
{
  vector<string> files;
  vector<string> legends;
  vector<string> histos;
  files.clear();
  legends.clear();
  histos.clear();

  // compare data and MC
  files.push_back("met_znunu/gencut100recocut150_24bins/singlemuon.root");
  files.push_back("met_znunu/gencut100recocut150_24bins/gencut100recocut150_met_zmm.root");
  files.push_back("met_znunu/gencut100recocut150_24bins/gencut100recocut150_met_tt.root");
  
  legends.push_back("Data");
  legends.push_back("Z/#gamma^{*}(#rightarrow#mu#mu)+jets");
  legends.push_back("t#bar{t}");


  stackHisto("h_mz_data_total",files,legends,histos,"M_{#mu#mu} [GeV]");
  stackHisto("h_recmet_after_data_total",files,legends,histos,"#slash{E}_{T} [GeV]");
  stackHisto("h_lpt_data0_total",files,legends,histos,"p_{T}^{\ell 1} [GeV]");
  stackHisto("h_lpt_data1_total",files,legends,histos,"p_{T}^{\ell 2} [GeV]");

  // compare test sample and corrected met 1D
  files.clear();
  legends.clear();
  histos.clear();

  // files.push_back("met_znunu/gencut100recocut150_24bins/final_Znn.root");
  // files.push_back("met_znunu/gencut100recocut150_24bins/final_Znn.root");
  files.push_back("final_Znn.root");
  files.push_back("final_Znn.root");

  legends.push_back("Z#rightarrow #nu #nu + jets Test sample");
  legends.push_back("1-D Corrected Z#rightarrow #mu #mu MC");

  histos.push_back("testmet_mc");
  histos.push_back("outputmet1D");
  stackHisto("1DmetMC",files,legends,histos,"#slash{E}_{T} [GeV]");

  legends.clear();
  histos.clear();

  legends.push_back("Z#rightarrow #nu #nu + jets Test sample");
  legends.push_back("2-D Corrected Z#rightarrow #mu #mu MC");

  histos.push_back("testmet_mc");
  histos.push_back("outputmet2D");
  stackHisto("2DmetMC",files,legends,histos,"#slash{E}_{T} [GeV]");

  legends.clear();
  histos.clear();

  legends.push_back("Z#rightarrow #nu #nu + jets Test sample");
  legends.push_back("1-D Corrected Z#rightarrow #mu #mu data");

  histos.push_back("testmet_data");
  histos.push_back("outputmetdata");
  stackHisto("1Dmetdata",files,legends,histos,"#slash{E}_{T} [GeV]");


}
