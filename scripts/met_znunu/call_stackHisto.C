#include "stackHisto.C"
#include <vector>
#include <string>

using namespace std;

void call_stackHisto()
{
  vector<string> files;
  files.push_back("met_znunu/gencut100recocut150_24bins/singlemuon.root");
  files.push_back("met_znunu/gencut100recocut150_24bins/gencut100recocut150_met_zmm.root");
  files.push_back("met_znunu/gencut100recocut150_24bins/gencut100recocut150_met_tt.root");
  
  vector<string> legends;
  legends.push_back("Data");
  legends.push_back("Z/#gamma^{*}(#rightarrow#mu#mu)+jets");
  legends.push_back("t#bar{t}");

  stackHisto("h_mz_data_total",files,legends,"M_{#mu#mu} [GeV]");

}
