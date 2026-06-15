#include <TF1.h>
#include <TF2.h>
#include <TMath.h>
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include <iostream>
#include <cstdlib>

const Double_t mh=125;
const Double_t vev=246;
const Double_t mZ=91.1876;
const int mchi=10;
const Double_t ychi=1;
const Double_t mtau=1.777;
const Double_t mb=4.7;
const Double_t mt=172;
const Double_t penaltyFactor=1;
const Double_t fracWH_upper=0.05;


Double_t cot2beta(Double_t tb)
{
  return (1-tb*tb)/2.0/tb;
 
}

Double_t sin2beta(Double_t tb)
{
  return (2*tb)/(1+tb*tb);
}

Double_t beta(Double_t m1, Double_t m2)
{
  return m1<2*m2? 0: sqrt(1-4*m2*m2/m1/m1);
}

Double_t lambda(Double_t m1, Double_t m2, Double_t m3)
{
  return pow(m1*m1-m2*m2-m3*m3,2)-4*m2*m2*m3*m3;
}


Double_t gAah_value(Double_t sint, Double_t lam3, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 1.0/mA/vev*(mh*mh-2*mH*mH-mA*mA+4*mHc*mHc-ma*ma-2*lam3*vev*vev)*sint*cost;
  return value;
}


Double_t width_Achichi(Double_t sint, Int_t mA)
{

  Double_t value = 1.0/TMath::Pi()/8.0*mA*beta(mA,mchi)*sint*sint;
  return value;
}

Double_t width_Aff(Double_t sint, Double_t tb, Double_t mf, Int_t mA)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t Nc = mf>2.0? 3.0: 1.0;
  Double_t value = Nc/tb/tb/TMath::Pi()/8.0*mf*mf/vev/vev*mA*beta(mA,mf)*cost*cost;
  return value;
}

Double_t width_Aah(Double_t sint, Double_t lam3, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  Double_t value = 1.0/16.0/TMath::Pi()*sqrt(lambda(mA,mh,ma))/mA*pow(gAah_value(sint,lam3,mH,mHc,mA,ma),2);
  return value;
}


Double_t width_A(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  Double_t value = width_Aah(sint,lam3,mH,mHc,mA,ma)
    + width_Achichi(sint,mA)
    + width_Aff(sint,tb,mtau,mA)
    + width_Aff(sint,tb,mb,mA)
    + width_Aff(sint,tb,mt,mA);
  return value;
}



Double_t BRAtoah(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  Double_t w1 = width_Aah(sint,lam3,mH,mHc,mA,ma);
  Double_t value = w1/width_A(sint,lam3,tb,mH,mHc,mA,ma);
  return value;

}



Double_t gHAA_value(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 2.0*cot2beta(tb)/mH/vev*(mh*mh-2*mH*mH+2*mHc*mHc-lam3*vev*vev)*cost*cost;
  return value;
}

Double_t gHAa_value(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 2.0*cot2beta(tb)/mH/vev*(mh*mh-2*mH*mH+2*mHc*mHc-lam3*vev*vev)*sint*cost;
  return value;
}


Double_t gHaa_value(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 2.0*cot2beta(tb)/mH/vev*(mh*mh-2*mH*mH+2*mHc*mHc-lam3*vev*vev)*sint*sint;
  return value;
}


Double_t width_Hff(Double_t tb, Double_t mf, Int_t mH)
{
  Double_t Nc = mf>2.0? 3.0:1.0;
  Double_t value = Nc/tb/tb/TMath::Pi()/8.0*mf*mf/vev/vev*mH*pow(beta(mH,mf),3);
  return value;
}

Double_t width_HZA(Double_t sint, Int_t mH, Int_t mA)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 1.0/16.0/TMath::Pi()*pow(lambda(mH,mZ,mA),1.5)/pow(mH,3)/vev/vev*cost*cost;
  return value;
}

Double_t width_HZa(Double_t sint, Int_t mH, Int_t ma)
{
  Double_t value = 1.0/16.0/TMath::Pi()*pow(lambda(mH,mZ,ma),1.5)/pow(mH,3)/vev/vev*sint*sint;
  return value;
}


Double_t width_HAA(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t mA)
{
  Double_t value = 1.0/32.0/TMath::Pi()*pow(gHAA_value(sint,lam3,tb,mH,mHc),2)*mH*beta(mH,mA);
  return value;
}

Double_t width_HAa(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  Double_t value = 1.0/16.0/TMath::Pi()*sqrt(lambda(mH,mA,ma))/mH*pow(gHAa_value(sint,lam3,tb,mH,mHc),2);
  return value;
}

Double_t width_Haa(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t ma)
{
  Double_t value = 1.0/32.0/TMath::Pi()*pow(gHaa_value(sint,lam3,tb,mH,mHc),2)*mH*beta(mH,ma);
  return value;
}


Double_t width_H(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  Double_t value = width_Hff(tb,mtau,mH)
    + width_Hff(tb,mb,mH)
    + width_Hff(tb,mt,mH)
    + width_HZA(sint,mH,mA)
    + width_HZa(sint,mH,ma)
    + width_HAA(sint,lam3,tb,mH,mHc,mA)
    + width_HAa(sint,lam3,tb,mH,mHc,mA,ma)
    + width_Haa(sint,lam3,tb,mH,mHc,ma);
  return value;
}


Double_t BRHtoAA(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  Double_t w1 = width_HAA(sint,lam3,tb,mH,mHc,mA);
  Double_t value = w1/width_H(sint,lam3, tb,mH,mHc,mA,ma);
  return value;

}


Double_t width_achichi(Double_t sint, Double_t tb, Int_t ma)
{
  Double_t value = ychi*ychi/8.0/TMath::Pi()*ma*beta(ma,mchi)*(1-sint*sint);
  return value;
}

Double_t width_aff(Double_t sint, Double_t tb, Double_t mf, Int_t ma)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t Nc = mf>2.0? 3.0: 1.0;
  Double_t value = Nc/tb/tb/TMath::Pi()/8.0*mf*mf/vev/vev*ma*beta(ma,mf)*sint*sint;
  return value;
}

Double_t width_a(Double_t sint, Double_t tb, Int_t ma)
{
  Double_t value = width_achichi(sint,tb,ma)+
    width_aff(sint,tb,mtau,ma)+
    width_aff(sint,tb,mb,ma)+
    width_aff(sint,tb,mt,ma);
  return value;
}

Double_t BRatochichi(Double_t sint, Double_t tb, Int_t ma)
{
  Double_t w1 = width_achichi(sint,tb,ma);
  Double_t value = w1/width_a(sint,tb,ma);
  return value;
}


Double_t effXSec(Double_t sint, Double_t lam3, Double_t tb, Int_t mH, Int_t mHc, Int_t mA, Int_t ma)
{
  return 1.0/tb/tb*BRHtoAA(sint,lam3,tb,mH,mHc,mA,ma)*pow(BRAtoah(sint,lam3,tb,mH,mHc,mA,ma),2)*pow(BRatochichi(sint,tb,ma),2);
}


Double_t functionForMinimize(const Double_t *par)
{
   Double_t sint =par[0];
   Double_t lam3 =par[1];
   Double_t tb   =par[2];
   Int_t    mH   =par[3];
   Int_t    mHc  =par[4];
   Int_t    mA   =par[5];
   Int_t    ma   =par[6];
   Double_t value = - effXSec(sint,lam3,tb, mH, mHc, mA, ma);
   Double_t constraint = penaltyFactor*(width_H(sint,lam3,tb, mH, mHc, mA, ma)/mH-fracWH_upper);
   return (value+constraint);
  
   /*
   Double_t constraint = (width_H(sint,lam3,tb,mH,mHc,mA,ma)/mH-fracWH_upper);

   if (constraint > 0) {
        // Apply a severe, continuous penalty if the constraint is violated.
        // Squaring the violation helps the minimizer calculate a gradient back to the valid region.
     //     std::cout << "Applying constraint " << std::endl;
     double penalty = penaltyFactor * std::pow(constraint, 2);
     return value + penalty;
    }
   return (value);
   */
}



void minimize_3d(const int mH, const int mA)
{
  const int mHc=mH;
  const int ma=100;
  ROOT::Math::Minimizer* minimizer = 
    ROOT::Math::Factory::CreateMinimizer("Minuit2", "Migrad");

  if (!minimizer) {
    std::cerr << "Error: Could not create minimizer!" << std::endl;
    return;
  }

  // 3. Configure minimizer settings
  minimizer->SetMaxFunctionCalls(1000000); // Upper limit on function evaluations
  minimizer->SetTolerance(0.001);          // Convergence tolerance
  minimizer->SetPrintLevel(1);             // 1 = print results, 0 = silent, 2 = verbose

  // 4. Wrap our function in a ROOT Functor (specifying 3 dimensions)
  ROOT::Math::Functor f(&functionForMinimize, 3);
  minimizer->SetFunction(f);

// 5. Set the 3 variables
    // Arguments: (index, name, start_value, step_size)
  //    minimizer->SetVariable(0, "sint", 0.0, 0.1);
  //    minimizer->SetVariable(1, "lam3", 1.0, 0.1);
  //    minimizer->SetVariable(2, "tb", 1.0, 0.1);

    // Optional: If you want to set boundaries, use SetLimitedVariable instead:
  /*
  minimizer->SetLimitedVariable(0, "sint", 0.0665, 0.01, 0.0, sqrt(2)/2.0);
  minimizer->SetLimitedVariable(1, "lam3", 1.72098, 0.01, 1.0, 10.0);
  minimizer->SetLimitedVariable(2, "tb", 1.918, 0.01, 1.0, 10.0);
  */
  minimizer->SetLimitedVariable(0, "sint", 0.064, 0.01, 0.0, sqrt(2)/2.0);
  minimizer->SetLimitedVariable(1, "lam3", 1.0, 0.01, 1.0, 10.0);
  minimizer->SetLimitedVariable(2, "tb", 1.5, 0.01, 1.0, 10.0);
 
  minimizer->SetVariable(3, "mH",  mH,0.01);
  minimizer->SetVariable(4, "mHc", mHc,0.01);
  minimizer->SetVariable(5, "mA",  mA,0.01);
  minimizer->SetVariable(6, "ma", ma,0.01);  
  minimizer->FixVariable(3);
  minimizer->FixVariable(4);
  minimizer->FixVariable(5);
  minimizer->FixVariable(6);
  
  // 6. Run the minimization
  // fix sinTheta first
  //  minimizer->FixVariable(1);
  minimizer->FixVariable(0);
  minimizer->Minimize();
  minimizer->ReleaseVariable(0);
  minimizer->Minimize();
  
  // 7. Retrieve and print results
  const double *xs = minimizer->X();
  const double *errors = minimizer->Errors();

  std::cout << "\n=== Minimization Results ===" << std::endl;
  std::cout << "Minimum value found: " << minimizer->MinValue() << std::endl;
  std::cout << "Effective cross section: "
	    << effXSec(xs[0],xs[1],xs[2],mH,mHc,mA,ma) << std::endl;
  std::cout << "Fractional width: " << width_H(xs[0],xs[1],xs[2],mH,mHc,mA,ma)/mH << std::endl;
  std::cout << "sint = " << xs[0] << " +/- " << errors[0] << std::endl;
  std::cout << "lam3 = " << xs[1] << " +/- " << errors[1] << std::endl;
  std::cout << "tb = " << xs[2] << " +/- " << errors[2] << std::endl;

    // Clean up memory
    delete minimizer;
}

int main(int argc, char* argv[]) {

  if(argc !=3){
    std::cout << "Usage: ./minimize_3d mH mA" << std::endl;
    return 1;
  }

  const int mH=std::atoi(argv[1]);
  const int mA=std::atoi(argv[2]);
  
  minimize_3d(mH,mA);
  return 0;
}
  
  
 
