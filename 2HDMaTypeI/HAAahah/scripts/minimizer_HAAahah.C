#include <TF1.h>
#include <TF2.h>
#include <TMath.h>
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include <iostream>

const Double_t mh=125;
const Double_t vev=246;
const Double_t mZ=91.1876;
const int mA=230;
const int mH=700;
const int mHc=mH;
const int ma=100;
const int mchi=10;
const Double_t mtau=1.777;
const Double_t mb=4.7;
const Double_t mt=172;
const Double_t penaltyFactor=1e3;
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


Double_t gAah_value(Double_t sint, Double_t lam3)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 1.0/mA/vev*(mh*mh-2*mH*mH-mA*mA+4*mHc*mHc-ma*ma-2*lam3*vev*vev)*sint*cost;
  return value;
}


Double_t width_Achichi(Double_t sint)
{

  Double_t value = 1.0/TMath::Pi()/8.0*mA*beta(mA,mchi)*sint*sint;
  return value;
}

Double_t width_Aff(Double_t sint, Double_t mf, Double_t tb)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t Nc = mf>2.0? 3.0: 1.0;
  Double_t value = Nc/tb/tb/TMath::Pi()/8.0*mf*mf/vev/vev*mA*beta(mA,mf)*cost*cost;
  return value;
}

Double_t width_Aah(Double_t sint, Double_t lam3)
{
  Double_t value = 1.0/16.0/TMath::Pi()*sqrt(lambda(mA,mh,ma))/mA*pow(gAah_value(sint,lam3),2);
  return value;
}


Double_t width_A(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t value = width_Aah(sint,lam3) + width_Achichi(sint) + width_Aff(sint,mtau,tb)+ width_Aff(sint,mb,tb)+width_Aff(sint,mt,tb);
  return value;
}



Double_t BRAtoah(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t w1 = width_Aah(sint,lam3);
  Double_t value = w1/width_A(sint,lam3, tb);
  return value;

}



Double_t gHAA_value(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 2.0*cot2beta(tb)/mH/vev*(mh*mh-2*mH*mH+2*mHc*mHc-lam3*vev*vev)*cost*cost;
  return value;
}

Double_t gHAa_value(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 2.0*cot2beta(tb)/mH/vev*(mh*mh-2*mH*mH+2*mHc*mHc-lam3*vev*vev)*sint*cost;
  return value;
}


Double_t gHaa_value(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 2.0*cot2beta(tb)/mH/vev*(mh*mh-2*mH*mH+2*mHc*mHc-lam3*vev*vev)*sint*sint;
  return value;
}


Double_t width_Hff(Double_t mf, Double_t tb)
{
  Double_t Nc = mf>2.0? 3.0:1.0;
  Double_t value = Nc/tb/tb/TMath::Pi()/8.0*mf*mf/vev/vev*mH*pow(beta(mH,mf),3);
  return value;
}

Double_t width_HZA(Double_t sint)
{
  Double_t cost = sqrt(1-sint*sint);
  Double_t value = 1.0/16.0/TMath::Pi()*pow(lambda(mH,mZ,mA),1.5)/pow(mH,3)/vev/vev*cost*cost;
  return value;
}

Double_t width_HZa(Double_t sint)
{
  Double_t value = 1.0/16.0/TMath::Pi()*pow(lambda(mH,mZ,ma),1.5)/pow(mH,3)/vev/vev*sint*sint;
  return value;
}


Double_t width_HAA(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t value = 1.0/32.0/TMath::Pi()*pow(gHAA_value(sint,lam3,tb),2)*mH*beta(mH,mA);
  return value;
}

Double_t width_HAa(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t value = 1.0/16.0/TMath::Pi()*sqrt(lambda(mH,mA,ma))/mH*pow(gHAa_value(sint,lam3,tb),2);
  return value;
}

Double_t width_Haa(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t value = 1.0/32.0/TMath::Pi()*pow(gHaa_value(sint,lam3,tb),2)*mH*beta(mH,ma);
  return value;
}


Double_t width_H(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t value = width_Hff(mtau,tb)+ width_Hff(mb,tb)+width_Hff(mt,tb)+
    width_HZA(sint)+width_HZa(sint)+width_HAA(sint,lam3,tb)+width_HAa(sint,lam3,tb)+width_Haa(sint,lam3,tb);
  return value;
}


Double_t BRHtoAA(Double_t sint, Double_t lam3, Double_t tb)
{
  Double_t w1 = width_HAA(sint,lam3,tb);
  Double_t value = w1/width_H(sint,lam3, tb);
  return value;

}


Double_t effXSec(Double_t sint, Double_t lam3, Double_t tb)
{
  return 1.0/tb/tb*BRHtoAA(sint,lam3,tb)*pow(BRAtoah(sint,lam3,tb),2);
}


Double_t functionForMinimize(const Double_t *par)
{
   Double_t sint =par[0];
   Double_t lam3 =par[1];
   Double_t tb   =par[2];
   Double_t value = - effXSec(sint,lam3,tb);
   /*
   Double_t constraint = penaltyFactor*(width_H(sint,lam3,tb)/mH-fracWH_upper);
   return (value+constraint);
   */
   Double_t constraint = (width_H(sint,lam3,tb)/mH-fracWH_upper);
   //   std::cout << "widthH/mH = " << width_H(sint,lam3,tb)/mH << std::endl;
   if (constraint > 0) {
        // Apply a severe, continuous penalty if the constraint is violated.
        // Squaring the violation helps the minimizer calculate a gradient back to the valid region.
     //     std::cout << "Applying constraint " << std::endl;
     double penalty = penaltyFactor * std::pow(constraint, 2);
     return value + penalty;
    }
   return (value);

}



void minimize_3d()
{
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
  minimizer->SetLimitedVariable(0, "sint", 0.064, 0.01, 0.0, sqrt(2)/2.0);
  minimizer->SetLimitedVariable(1, "lam3", 1.0, 0.01, 1.0, 10.0);
  minimizer->SetLimitedVariable(2, "tb", 1.5, 0.01, 1.0, 10.0);
  // 6. Run the minimization
  minimizer->Minimize();

  // 7. Retrieve and print results
  const double *xs = minimizer->X();
  const double *errors = minimizer->Errors();

  std::cout << "\n=== Minimization Results ===" << std::endl;
  std::cout << "Minimum value found: " << minimizer->MinValue() << std::endl;
  std::cout << "Effective cross section: " << effXSec(xs[0],xs[1],xs[2]) << std::endl;
  std::cout << "Fractional width: " << width_H(xs[0],xs[1],xs[2])/mH << std::endl;
  std::cout << "sint = " << xs[0] << " +/- " << errors[0] << std::endl;
  std::cout << "lam3 = " << xs[1] << " +/- " << errors[1] << std::endl;
  std::cout << "tb = " << xs[2] << " +/- " << errors[2] << std::endl;

    // Clean up memory
    delete minimizer;
}

int main() {
    minimize_3d();
    return 0;
}
  
  
 
