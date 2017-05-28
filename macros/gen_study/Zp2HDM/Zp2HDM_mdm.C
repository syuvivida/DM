Double_t Zp2HDM_mdm(Double_t* x, Double_t* par)
{
  float mdm=x[0];
  const float ymt=172;
  const float ymb=4.7;
  float ma0=par[0];
  const float tb1=1;
  bool noTTbar=ma0<ymt*2;

  const float gdm=1;
  float gamma_chichi = (gdm*gdm*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);
  //  cout << "width of A0-> chi chi = " << gamma_chichi << " GeV" << endl;

  const double aEW = 1.0/127.9;
  const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
  const double MZ = 91.1876;
  const double Gf = 0.0000116637;
  const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
  const double sw = sqrt(1-MW*MW/MZ/MZ);
  const double gw = ee/sw;
  const double cw = sqrt(1-sw*sw);


  const float vu=(2*MW*sw)/ee;
//   cout << "vu = " << vu << endl;
  const float vd=(2*MW*sw)/ee;
//   cout << "vd = " << vd << endl;
  float yt=sqrt(2)*ymt/vu;
  float gamma_tt1 = noTTbar? 0: (3./pow(tb1,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);
//   cout << "width of A0-> tt for tanbeta = " << tb1 << " is " << gamma_tt1 << " GeV" << endl;


  float yb=sqrt(2)*ymb/vd;
  float gamma_bb1 = (3.*pow(tb1,2)*pow(ma0*yb,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymb,2)))/16.0/TMath::Pi()/pow(ma0,3);
  float br = gamma_chichi/(gamma_tt1+gamma_bb1+gamma_chichi);
  return br;
//   cout << "width of A0-> bb for tanbeta = " << tb1 << " is " << gamma_bb1 << " GeV" << endl;

//   cout << "BR(A0->chi chi) for tanbeta = " << tb1 << " is " << gamma_chichi/(gamma_tt1+gamma_bb1+gamma_chichi) << endl;

}
