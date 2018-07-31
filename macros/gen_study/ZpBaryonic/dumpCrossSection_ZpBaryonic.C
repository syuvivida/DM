#define dumpCrossSection_ZpBaryonic_cxx
#include "dumpCrossSection_ZpBaryonic.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>


const int nScales=6; 
const int nPDFs=100;


void vector_compute(const std::vector<double>& ref, double& avg, double& RMS)
{

  double sum  = std::accumulate(ref.begin(), ref.end(), 0.0);
  avg         = sum /(double)ref.size();
  double sum2 = std::inner_product(ref.begin(), ref.end(),  ref.begin(), 0.0);
  RMS         = sqrt(sum2/(double)ref.size() - avg*avg);

  return;

}


void vector_hessian(const double central, const std::vector<double>& ref, double& avg, double& RMS)
{

  double sum  = std::accumulate(ref.begin(), ref.end(), 0.0);
  avg         = sum /(double)ref.size();
  std::vector<double> diff;
  for(unsigned int i=0; i<ref.size(); i++)diff.push_back(ref[i]-central);
  RMS = sqrt(std::inner_product(diff.begin(), diff.end(),  diff.begin(), 0.0));
  return;

}

void dumpCrossSection_ZpBaryonic::Loop()
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;

   Double_t totalXs_central=0;
   Double_t totalXs_central_263400=0;
   Double_t totalXs_central_320900=0;

   std::vector<double> scaleWeights;
   for(int i=0; i<nScales; i++) scaleWeights.push_back(0.0);

   std::vector<double> pdfWeights_263400;
   for(int i=0; i<nPDFs; i++) pdfWeights_263400.push_back(0.0);
   std::vector<double> pdfWeights_320900;
   for(int i=0; i<nPDFs; i++) pdfWeights_320900.push_back(0.0);


   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      // central 0
      // scale 1,2,3,4,6,8
      // 320900: central 161
      //         error 162-261
      // 263400: central 465
      //         error   466-565

      totalXs_central           += Rwgt_Weight[0];
      totalXs_central_263400    += Rwgt_Weight[465];
      totalXs_central_320900    += Rwgt_Weight[161];

      // scale
      scaleWeights[0]           += (Rwgt_Weight[1]-Rwgt_Weight[0]);
      scaleWeights[1]           += (Rwgt_Weight[2]-Rwgt_Weight[0]);
      scaleWeights[2]           += (Rwgt_Weight[3]-Rwgt_Weight[0]);
      scaleWeights[3]           += (Rwgt_Weight[4]-Rwgt_Weight[0]);
      scaleWeights[4]           += (Rwgt_Weight[6]-Rwgt_Weight[0]);
      scaleWeights[5]           += (Rwgt_Weight[8]-Rwgt_Weight[0]);

      
      // PDF
      for(int i=0; i<nPDFs; i++) pdfWeights_263400[i] += Rwgt_Weight[i+466];
      for(int i=0; i<nPDFs; i++) pdfWeights_320900[i] += Rwgt_Weight[i+162];
      
	
   }

   // take the maximum difference

   for(unsigned int i=0; i<scaleWeights.size();i++)cout << "[" << i << "] = " << scaleWeights[i] << endl;
   std::vector<double> absScaleDiff;
   for(unsigned int i=0; i<scaleWeights.size();i++)absScaleDiff.push_back(fabs(scaleWeights[i]));
   auto it =std::max_element(std::begin(absScaleDiff), std::end(absScaleDiff));
   double maxScaleDiff = *it;

   double totalScaleDiff = sqrt(std::inner_product(scaleWeights.begin(), scaleWeights.end(),  scaleWeights.begin(), 0.0));
   double scaleDiffRMS = sqrt(std::inner_product(scaleWeights.begin(), scaleWeights.end(),  scaleWeights.begin(), 0.0)/(double)scaleWeights.size());

   double avg_263400, RMS_263400;
   vector_compute(pdfWeights_263400, avg_263400, RMS_263400);

   double avg_320900, RMS_320900;
   vector_compute(pdfWeights_320900, avg_320900, RMS_320900);

   cout << "central cross section = " << totalXs_central << " pb" << endl;  
   cout << " =============================================================================================" << endl;
   cout << "scale maximum variation = " << maxScaleDiff << " pb " << endl;
   cout << "scale total variation = " << totalScaleDiff << " pb " << endl;
   cout << "scale RMS variation = " << scaleDiffRMS << " pb " << endl;
   cout << " =============================================================================================" << endl;
   cout << "263400 central cross section = " << totalXs_central_263400 << " pb" << endl;
   cout << "263400 central cross section 2 = " << avg_263400 << "+-" << RMS_263400 << " pb" << endl;
   cout << "320900 central cross section = " << totalXs_central_320900 << " pb" << endl;
   cout << "320900 central cross section 2 = " << avg_320900 << "+-" << RMS_320900 << " pb" << endl;

}
