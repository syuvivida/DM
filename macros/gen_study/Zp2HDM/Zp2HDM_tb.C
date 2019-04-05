void Zp2HDM_tb(const double mzp, const double ma0, const double tb1, const double tb2, const double xs1, const double xs2)
{

  const double ymt=172;
  const double ymb=4.7;
  const double ymtau=1.777;
  bool noTTbar=ma0<ymt*2;
  // if(ma0<ymt*2){
  //   cout << "This program can only be run for ma0 > " << ymt*2 << " GeV !! " << endl;
  //   return;
  // }

  double ratio1 = ((tb1*tb1)/pow(1+tb1*tb1,2))/((tb2*tb2)/pow(1+tb2*tb2,2));
  cout << "ratio of Gamma(Z'->A0h) = " << ratio1 << endl;

  const double gdm=1;
  const double mdm=100;
  double gamma_chichi = (gdm*gdm*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> chi chi = " << gamma_chichi << " GeV" << endl;

  const double aEW = 1.0/127.9;
  const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
  const double MZ = 91.1876;
  const double Gf = 0.0000116637;
  const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
  const double sw = sqrt(1-MW*MW/MZ/MZ);
  const double gw = ee/sw;
  const double cw = sqrt(1-sw*sw);


  const double vu=(2*MW*sw)/ee;
  cout << "vu = " << vu << endl;
  const double vd=(2*MW*sw)/ee;
  cout << "vd = " << vd << endl;
  double yt=sqrt(2)*ymt/vu;
  double gamma_tt1 = noTTbar? 0: (3./pow(tb1,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> tt for tanbeta = " << tb1 << " is " << gamma_tt1 << " GeV" << endl;


  double yb=sqrt(2)*ymb/vd;
  double gamma_bb1 = (3.*pow(tb1,2)*pow(ma0*yb,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymb,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> bb for tanbeta = " << tb1 << " is " << gamma_bb1 << " GeV" << endl;

  double ytau=sqrt(2)*ymtau/vd;
  double gamma_tautau1 = (pow(ma0*ytau,2)*pow(tb1,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymtau,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> tau tau for tanbeta = " << tb1 << " is " << gamma_tautau1 << " GeV" << endl;

  cout << "BR(A0->chi chi) for tanbeta = " << tb1 << " is " << gamma_chichi/(gamma_tt1+gamma_bb1+gamma_chichi) << endl;

  double gamma_tt2 = noTTbar? 0 : (3./pow(tb2,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> tt for tanbeta = " << tb2 << " is " << gamma_tt2 << " GeV" << endl;
  double gamma_bb2 = (3.*pow(tb2,2)*pow(ma0*yb,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymb,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> bb for tanbeta = " << tb2 << " is " << gamma_bb2 << " GeV" << endl;

  double gamma_tautau2 = (pow(ma0*ytau,2)*pow(tb2,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymtau,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> tau tau for tanbeta = " << tb2 << " is " << gamma_tautau2 << " GeV" << endl;


  cout << "BR(A0->chi chi) for tanbeta = " << tb2 << " is " << gamma_chichi/(gamma_tt2+gamma_bb2+gamma_chichi) << endl;

  double ratio2 = (gamma_bb2+gamma_tt2+gamma_chichi)/(gamma_bb1+gamma_tt1+gamma_chichi);

  cout << "ratio of BR(A0->chi chi) = " << ratio2 << endl;
  cout << endl;
  cout << "For Z'-2HDM cross sections at MZp = " << mzp << " GeV and MA0 = " <<  ma0 << " GeV: " << endl;
  double dr = xs1/xs2/ratio1/ratio2;
  cout << "ratio of Madgraph estimation divided by Eiko's prediction = " << dr << endl;
}
