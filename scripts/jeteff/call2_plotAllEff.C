#include "plotAllEff.C"
#include <string>
#include <vector>

using namespace std;

void call2_plotAllEff(string filename)
{

  bool JETMET=false;
  if(filename.find("JETMET")!= std::string::npos)
    JETMET=true;
  std::string prefix;
  if(JETMET)
    prefix= "_JEMET";
  else
    prefix= "_myDefinition";

  vector<vector<string>>legends;
  legends.clear();
  vector<string> temp_strings;
  temp_strings.clear();
  vector<string> effnames;
  effnames.clear();
  vector<string> cuts;
  cuts.clear();
  temp_strings.push_back("single Anti-Kt R=0.8 jet");
  if(JETMET)
    temp_strings.push_back("#Delta R(H,jet)<0.1");
  else
    temp_strings.push_back("#Delta R(b,jet)<0.8");
  legends.push_back(temp_strings);

  temp_strings.clear();
  temp_strings.push_back("two Anti-Kt R=0.4 jets");
  if(JETMET)
    temp_strings.push_back("#Delta R(b,jet)<0.1");
  else
    temp_strings.push_back("#Delta R(b,jet)<0.4");
  legends.push_back(temp_strings);

  ///////////////////////////////////////////////////////////////////////////////
  ///  Basic kinematic selection and reconstruction efficiency
  effnames.clear();
  effnames.push_back("fatJetEff");
  effnames.push_back("thinJetEff");

  cuts.push_back("p_{T}>30 GeV, |#eta|<2.5");
  
  plotAllEff(filename,effnames,
	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
      	     legends,
	     cuts,
	     Form("jetrec%s",prefix.data()),
	     1.2,
    	     0.447,
    	     0.551,
    	     0.796,
    	     0.643,
	     0.429,
	     0.226,
	     0.780,
	     0.565	     
	     );




  // // ///////////////////////////////////////////////////////////////////////////////
  // // // /// Basic kinematic selection + mass cuts
  
  effnames.clear();
  effnames.push_back("fatJetEff2");
  effnames.push_back("thinJetEff2");
  cuts.push_back("70<M_{pruned}<140 GeV");
  cuts.push_back("80<M_{bb}<150 GeV");
  plotAllEff(filename,effnames,
    	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
	     legends,
	     cuts,
	     Form("jetrecmass%s",prefix.data()),
       	     1.20,
      	     0.445,
      	     0.703,
      	     0.796,
      	     0.882,
      	     0.427,
      	     0.198,
      	     0.778,
      	     0.538	     
      	     );



  // // // ///////////////////////////////////////////////////////////////////////////////
  // // // /// mass cuts only
  
  effnames.clear();
  effnames.push_back("fatJetMassEff");
  effnames.push_back("thinJetMassEff");

  legends.clear();
  temp_strings.clear();
  temp_strings.push_back("AK8 jet");
  legends.push_back(temp_strings);

  temp_strings.clear();
  temp_strings.push_back("AK4 jet");
  legends.push_back(temp_strings);

  cuts.clear();
  cuts.push_back("70<M_{pruned}<140 GeV");
  cuts.push_back("80<M_{bb}<150 GeV");


  plotAllEff(filename,effnames,
    	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
	     legends,
	     cuts,
	     Form("jetmass%s",prefix.data()),
	     1.20,
       	     0.252,
       	     0.340,
       	     0.403,
    	     0.432,
      	     0.246,
      	     0.221,
       	     0.597,
       	     0.319

    	     );
	     



  // // ///////////////////////////////////////////////////////////////////////////////
  // // /// Basic kinematic selection + mass cuts + Fatjet CSV or subjet CSV
  effnames.clear();
  effnames.push_back("fatJetTotalEff");
  effnames.push_back("thinJetTotalEff");
  effnames.push_back("fatJetTotalEff1");
  effnames.push_back("fatJetTotalEff12");
  effnames.push_back("fatJetTotalEff2");


  legends.clear();
  temp_strings.clear();
  temp_strings.push_back("AK8 jet");
  legends.push_back(temp_strings);

  temp_strings.clear();
  temp_strings.push_back("AK4 jet");
  legends.push_back(temp_strings);

  temp_strings.clear();
  temp_strings.push_back("#geq 1 subjet");
  legends.push_back(temp_strings);

  temp_strings.clear();
  temp_strings.push_back("1 or 2 subjets");
  temp_strings.push_back("based on #Delta R_{subjet}");
  legends.push_back(temp_strings);

  temp_strings.clear();
  temp_strings.push_back("=2 subjets");
  legends.push_back(temp_strings);


  cuts.clear();
  cuts.push_back("p_{T}>30 GeV, |#eta|<2.5");
  cuts.push_back("70<M_{pruned}<140 GeV");
  cuts.push_back("80<M_{bb}<150 GeV");
  cuts.push_back("CISVV2 > 0.605");
  cuts.push_back("");

  
  plotAllEff(filename,effnames,
       	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
     	     legends,
      	     cuts,
      	     Form("jettotal%s",prefix.data()),
  	     1.80,
  	     0.531,
  	     0.417,
  	     0.849,
  	     0.887,
  	     0.129,
  	     0.440,
  	     0.580,
  	     0.790 )
    ;
	     


  // ///////////////////////////////////////////////////////////////////////////////
  // /// btagging cuts 


  effnames.clear();
  effnames.push_back("fatJetCSVEff");
  effnames.push_back("thinJetCSVEff");
  effnames.push_back("subJetCSVEff1");
  effnames.push_back("subJetCSVEff12");
  effnames.push_back("subJetCSVEff2");

  cuts.clear();
  cuts.push_back("CISVV2 > 0.605");

  plotAllEff(filename,effnames,
   	     ";Generator-level Higgs p_{T} [GeV];Efficiency;",
  	     legends,
  	     cuts,
  	     Form("jetcsv%s",prefix.data()),
  	     1.80,
 	     0.129,
 	     0.669,
  	     0.403,
 	     0.736,
 	     0.482,
 	     0.544,
 	     0.931,
 	     0.895 );





}
