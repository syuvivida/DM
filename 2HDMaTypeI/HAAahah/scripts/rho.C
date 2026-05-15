#include <TF1.h>

const Double_t mh=125;
const Double_t lam3=6.0;
const Double_t vev=246;
const Double_t sint=0.2;
const Double_t cost=sqrt(1-sint*sint);


Double_t f3(Double_t m1, Double_t m2, Double_t m3){

  Double_t value = m1*m1 - (fabs(m1-m2)<1e-6?m1*m1: m1*m1*m2*m2/(m1*m1-m2*m2)*TMath::Log(m1*m1/m2/m2))-
    (fabs(m1-m3)<1e-6?m1*m1: m1*m1*m3*m3/(m1*m1-m3*m3)*TMath::Log(m1*m1/m3/m3)) +
    (fabs(m2-m3)<1e-6?m2*m2: m2*m2*m3*m3/(m2*m2-m3*m3)*TMath::Log(m2*m2/m3/m3));
  return value; 

}

Double_t rho(Double_t mA, Double_t mH, Double_t mHc, Double_t ma){

  Double_t drho = 1.0/pow(TMath::Pi()*4,2)/vev/vev*(
						cost*cost*f3(mHc,mA,mH)+
						sint*sint*f3(mHc,ma,mH)
						);
  return drho;
  
}
