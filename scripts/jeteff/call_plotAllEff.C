#include "plotAllEff.C"
#include <string>
#include <vector>

using namespace std;

void call_plotAllEff(string filename)
{

  vector<string> cuts;
  cuts.push_back("p_{T}>30 GeV, |#eta|<2.5");
  
  plotAllEff(filename,"fatJetEff","thinJetEff",
    	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
    	     cuts,
    	     "jetrec",
    	     1.3,
    	     0.358,
    	     0.337,
    	     0.862,
    	     0.662
    	     );
  cuts.push_back("90<M_{pruned}<130 GeV");
  cuts.push_back("100<M_{bb}<140 GeV");

  plotAllEff(filename,"fatJetEff2","thinJetEff2",
    	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
   	     cuts,
   	     "jetrecmass");

  cuts.push_back("CISVV2 > 0.605");
  cuts.push_back("");

  plotAllEff(filename,"fatJetTotalEff","thinJetTotalEff",
    	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
   	     cuts,
   	     "jettotal",1.35,
   	     0.280,
   	     0.417,
   	     0.797,
   	     0.887);
  	     

  cuts.clear();
  cuts.push_back("90 < M_{pruned} < 130 GeV");
  cuts.push_back("100 < M_{bb} < 140 GeV");


  plotAllEff(filename,"fatJetMassEff","thinJetMassEff",
    	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
   	     cuts,
   	     "jetmass",1.4,
   	     0.353,
   	     0.525,
   	     0.871,
   	     0.895);
	     

  cuts.clear();
  cuts.push_back("CISVV2 > 0.605");

  plotAllEff(filename,"fatJetCSVEff","thinJetCSVEff",
   	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
	     cuts,
	     "jetcsv",1.55,
	     0.381,
	     0.538,
	     0.897,
	     0.907
	     );





}
