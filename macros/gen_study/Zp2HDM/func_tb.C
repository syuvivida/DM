// BR(A0->bb), note the decay width of A0->Zh is ignored
Double_t A0bb_tb(Double_t* x, Double_t* par)
{
  const double ma0=par[0];
  const double tb1=x[0];
  const double ymt=172;
  const double ymb=4.7;
  const double ymtau=1.777;
  bool noTTbar=ma0<ymt*2;
  const double gdm=1;
  const double mdm=100;
  double gamma_chichi = (gdm*gdm*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);

  const double aEW = 1.0/127.9;
  const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
  const double MZ = 91.1876;
  const double Gf = 0.0000116637;
  const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
  const double sw = sqrt(1-MW*MW/MZ/MZ);
  const double gw = ee/sw;
  const double cw = sqrt(1-sw*sw);


  const double vu=(2*MW*sw)/ee;
  const double vd=(2*MW*sw)/ee;
  double yt=sqrt(2)*ymt/vu;
  double gamma_tt1 = noTTbar? 0: (3./pow(tb1,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);

  double yb=sqrt(2)*ymb/vd;
  double gamma_bb1 = (3.*pow(tb1,2)*pow(ma0*yb,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymb,2)))/16.0/TMath::Pi()/pow(ma0,3);

  double ytau=sqrt(2)*ymtau/vd;
  double gamma_tautau = (pow(ma0*ytau,2)*pow(tb1,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymtau,2)))/16.0/TMath::Pi()/pow(ma0,3);


  double br_bb = gamma_bb1/(gamma_chichi + gamma_bb1 + gamma_tautau + gamma_tt1);

  return br_bb;

}


// BR(A0->tt), note the decay width of A0->Zh is ignored
Double_t A0tt_tb(Double_t* x, Double_t* par)
{
  const double ma0=par[0];
  const double tb1=x[0];
  const double ymt=172;
  const double ymb=4.7;
  const double ymtau=1.777;
  bool noTTbar=ma0<ymt*2;
  const double gdm=1;
  const double mdm=100;
  double gamma_chichi = (gdm*gdm*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);

  const double aEW = 1.0/127.9;
  const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
  const double MZ = 91.1876;
  const double Gf = 0.0000116637;
  const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
  const double sw = sqrt(1-MW*MW/MZ/MZ);
  const double gw = ee/sw;
  const double cw = sqrt(1-sw*sw);


  const double vu=(2*MW*sw)/ee;
  const double vd=(2*MW*sw)/ee;
  double yt=sqrt(2)*ymt/vu;
  double gamma_tt1 = noTTbar? 0: (3./pow(tb1,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);

  double yb=sqrt(2)*ymb/vd;
  double gamma_bb1 = (3.*pow(tb1,2)*pow(ma0*yb,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymb,2)))/16.0/TMath::Pi()/pow(ma0,3);

  double ytau=sqrt(2)*ymtau/vd;
  double gamma_tautau = (pow(ma0*ytau,2)*pow(tb1,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymtau,2)))/16.0/TMath::Pi()/pow(ma0,3);


  double br_tt = gamma_tt1/(gamma_chichi + gamma_bb1 + gamma_tautau + gamma_tt1);

  return br_tt;

}

// BR(A0->chichi), note the decay width of A0->Zh is ignored
Double_t A0DM_tb(Double_t* x, Double_t* par)
{
  const double ma0=par[0];
  const double tb1=x[0];
  const double ymt=172;
  const double ymb=4.7;
  const double ymtau=1.777;
  bool noTTbar=ma0<ymt*2;
  const double gdm=1;
  const double mdm=100;
  double gamma_chichi = (gdm*gdm*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);

  const double aEW = 1.0/127.9;
  const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
  const double MZ = 91.1876;
  const double Gf = 0.0000116637;
  const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
  const double sw = sqrt(1-MW*MW/MZ/MZ);
  const double gw = ee/sw;
  const double cw = sqrt(1-sw*sw);


  const double vu=(2*MW*sw)/ee;
  const double vd=(2*MW*sw)/ee;
  double yt=sqrt(2)*ymt/vu;
  double gamma_tt1 = noTTbar? 0: (3./pow(tb1,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);

  double yb=sqrt(2)*ymb/vd;
  double gamma_bb1 = (3.*pow(tb1,2)*pow(ma0*yb,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymb,2)))/16.0/TMath::Pi()/pow(ma0,3);

  double ytau=sqrt(2)*ymtau/vd;
  double gamma_tautau = (pow(ma0*ytau,2)*pow(tb1,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymtau,2)))/16.0/TMath::Pi()/pow(ma0,3);


  double br_DM = gamma_chichi/(gamma_chichi + gamma_bb1 + gamma_tautau + gamma_tt1);

  return br_DM;

}
