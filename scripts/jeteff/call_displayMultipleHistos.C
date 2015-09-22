#include <string>
#include <vector>
#include "displayMultipleHistos.C"

using namespace std;

void call_displayMultipleHistos()
{
  std::string dirname="/afs/cern.ch/work/s/syu/13tev/scripts/";
  vector<string> filenames;
  vector<string> legnames;
  vector<string> histonames;
  ////////////////////////////////  
  filenames.clear();
  filenames.push_back(dirname+"jeteff_old/jeteff_myDefinition_ZprimeToZhToZlephbb.root");
  filenames.push_back(dirname+"jeteff_mass/jeteff_myDefinition_ZprimeToZhToZlephbb_25ns.root");
  filenames.push_back(dirname+"jeteff_old/jeteff_myDefinition_ZprimeToZhToZlephbb.root");
  filenames.push_back(dirname+"jeteff_mass/jeteff_myDefinition_ZprimeToZhToZlephbb_25ns.root");

  legnames.clear();
  legnames.push_back("miniAOD M_{bb}");
  legnames.push_back("corrected M_{bb}");
  legnames.push_back("miniAOD Pruned mass");
  legnames.push_back("corrected Pruned mass");

  histonames.clear();
  histonames.push_back("h_bbmass");
  histonames.push_back("h_bbmass");
  histonames.push_back("h_fatjetmass");
  histonames.push_back("h_fatjetmass");
  
  
  displayMultipleHistos(filenames,legnames,histonames,"Z'#rightarrow Zh#rightarrow llbb");


  ////////////////////////////////  
  filenames.clear();
  filenames.push_back(dirname+"jeteff_monoHiggs/jeteff_myDefinition_MonoHSignalTreesMerged_20150919.root");
  filenames.push_back(dirname+"jeteff_monoHiggs/jeteff_myDefinition_MonoHSignalTreesMerged_20150919.root");

  legnames.clear();
  legnames.push_back("miniAOD M_{bb}");
  legnames.push_back("miniAOD Pruned mass");

  histonames.clear();
  histonames.push_back("h_bbmass");
  histonames.push_back("h_fatjetmass");
  
  
  displayMultipleHistos(filenames,legnames,histonames,"Z'#rightarrow A^{0}h#rightarrow bb #slash{E_{T}}");




}
