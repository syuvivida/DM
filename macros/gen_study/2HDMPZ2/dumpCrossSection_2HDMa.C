#define dumpCrossSection_2HDMa_cxx
#include "dumpCrossSection_2HDMa.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <algorithm>


const int nScales=6; 
const int nPDFs=100;
const int nPDFs_NNLO=100;

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

void dumpCrossSection_2HDMa::Loop()
{
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;


   Double_t totalXs_central=0;
   Double_t totalXs_central_263000=0;
   Double_t totalXs_central_306000=0;

   std::vector<double> scaleWeights;
   for(int i=0; i<nScales; i++) scaleWeights.push_back(0.0);

   std::vector<double> pdfWeights_263000;
   for(int i=0; i<nPDFs; i++) pdfWeights_263000.push_back(0.0);
   std::vector<double> pdfWeights_306000;
   for(int i=0; i<nPDFs_NNLO; i++) pdfWeights_306000.push_back(0.0);


   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      // central 0
      // scale 1,2,3,4,6,8
      // 306000: central 9
      //         error 10-111
      // 263000: central 838
      //         error   839-938

      totalXs_central           += Rwgt_Weight[0];
      totalXs_central_263000    += Rwgt_Weight[838];
      totalXs_central_306000    += Rwgt_Weight[9];

      // scale
      scaleWeights[0]           += (Rwgt_Weight[1]-Rwgt_Weight[0]);
      scaleWeights[1]           += (Rwgt_Weight[2]-Rwgt_Weight[0]);
      scaleWeights[2]           += (Rwgt_Weight[3]-Rwgt_Weight[0]);
      scaleWeights[3]           += (Rwgt_Weight[4]-Rwgt_Weight[0]);
      scaleWeights[4]           += (Rwgt_Weight[6]-Rwgt_Weight[0]);
      scaleWeights[5]           += (Rwgt_Weight[8]-Rwgt_Weight[0]);

      
      // PDF
      for(int i=0; i<nPDFs; i++) pdfWeights_263000[i] += Rwgt_Weight[i+839];
      for(int i=0; i<nPDFs_NNLO; i++) pdfWeights_306000[i] += Rwgt_Weight[i+10];
      
	
   }

   // take the maximum difference

   for(unsigned int i=0; i<scaleWeights.size();i++)cout << "[" << i << "] = " << scaleWeights[i] << endl;
   std::vector<double> absScaleDiff;
   for(unsigned int i=0; i<scaleWeights.size();i++)absScaleDiff.push_back(fabs(scaleWeights[i]));
   auto it =std::max_element(std::begin(absScaleDiff), std::end(absScaleDiff));
   double maxScaleDiff = *it;

   double totalScaleDiff = sqrt(std::inner_product(scaleWeights.begin(), scaleWeights.end(),  scaleWeights.begin(), 0.0));
   double scaleDiffRMS = sqrt(std::inner_product(scaleWeights.begin(), scaleWeights.end(),  scaleWeights.begin(), 0.0)/(double)scaleWeights.size());

   double avg_263000, RMS_263000;
   vector_compute(pdfWeights_263000, avg_263000, RMS_263000);

   double avg_306000, RMS_306000;
   vector_hessian(totalXs_central_306000,pdfWeights_306000, avg_306000, RMS_306000);

   cout << "central cross section = " << totalXs_central << " pb" << endl;  
   cout << " =============================================================================================" << endl;
   cout << "scale maximum variation = " << maxScaleDiff << " pb " << endl;
   cout << "scale total variation = " << totalScaleDiff << " pb " << endl;
   cout << "scale RMS variation = " << scaleDiffRMS << " pb " << endl;
   cout << " =============================================================================================" << endl;
   cout << "263000 central cross section = " << totalXs_central_263000 << " pb" << endl;
   cout << "263000 central cross section 2 = " << avg_263000 << "+-" << RMS_263000 << " pb" << endl;
   cout << "306000 central cross section = " << totalXs_central_306000 << " pb" << endl;
   cout << "306000 central cross section 2 = " << avg_306000 << "+-" << RMS_306000 << " pb" << endl;


}
