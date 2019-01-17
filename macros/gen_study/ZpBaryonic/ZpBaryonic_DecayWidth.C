
const Double_t gchi=1.0;
const Double_t   gq=0.25;
const Double_t   md=5.040000e-03;
const Double_t   mu=2.550000e-03;
const Double_t   ms=1.010000e-01;
const Double_t   mc=1.270000e+00;
const Double_t   mb=4.700000e+00;
const Double_t   mt=1.720000e+02;



Double_t ZpBaryonic_width(Double_t mq, Double_t* par)
{

  Double_t mmed =par[0];
  Double_t ncol =par[1];
  Double_t    g =par[2];
  if(mmed<1e-6)return 0;

  Double_t betaf= (mmed>2*mq)? sqrt(1-4*mq*mq/mmed/mmed):0;
  Double_t step = (mmed>2*mq)? 1: 0;

  Double_t width= ncol*(g*g/12.0/TMath::Pi())*(1+2*mq*mq/mmed/mmed)*betaf*step;

  return width;
  
}

Double_t ZpBaryonic_BR_dm(Double_t* x, Double_t* par)
{

  Double_t   mdm =x[0];
  Double_t  mmed =par[0];
  if(mmed<1e-6)return 0;
  Double_t ppar_dm[3]={mmed,1,gchi};
  

  
  Double_t width_dm    = ZpBaryonic_width(mdm,ppar_dm);
  Double_t mquark[6]={md,mu,ms,mc,mb,mt};
  Double_t ppar_q[3]={mmed,3,gq};

  Double_t width_quark_sum=0;
  for(int i=0;i<6;i++)
    width_quark_sum +=  ZpBaryonic_width(mquark[i],ppar_q);

  Double_t BR = width_dm+width_quark_sum<1e-12? 0: width_dm/(width_dm+width_quark_sum);
  return BR;
  
}



Double_t ZpBaryonic_BR_quark(Double_t* x, Double_t* par)
{

  Double_t   mdm =x[0];
  Double_t  mmed =par[0];
  if(mmed<1e-6)return 0;
  Int_t     quark_index = par[1]-1;
  if(quark_index <0 || quark_index>5)return 0;
  Double_t ppar_dm[3]={mmed,1,gchi};
  

  
  Double_t width_dm    = ZpBaryonic_width(mdm,ppar_dm);
  Double_t mquark[6]={md,mu,ms,mc,mb,mt};
  Double_t ppar_q[3]={mmed,3,gq};

  Double_t width_quark_sum=0;
  for(int i=0;i<6;i++)
    width_quark_sum +=  ZpBaryonic_width(mquark[i],ppar_q);

  Double_t width_thisquark = ZpBaryonic_width(mquark[quark_index],ppar_q);

  Double_t BR = width_dm+width_quark_sum<1e-12? 0: width_thisquark/(width_dm+width_quark_sum);
  return BR;
  
}
