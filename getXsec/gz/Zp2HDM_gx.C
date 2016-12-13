void Zp2HDM_gx(const float mzp, const float ma0, const float gx1, const float gx2, const float xs1, const float xs2)
{

  const float ymt=172;
  const float ymb=4.7;
  bool noTTbar=ma0<ymt*2;

  const float tb=1;
  const float mdm=100;


  const double aEW = 1.0/127.9;
  const double ee = 2*sqrt(aEW)*sqrt(TMath::Pi());
  const double MZ = 91.1876;
  const double Gf = 0.0000116637;
  const double MW = sqrt(pow(MZ,2)/2. + sqrt(pow(MZ,4)/4. - (aEW*TMath::Pi()*pow(MZ,2))/(Gf*sqrt(2))));
  const double sw = sqrt(1-MW*MW/MZ/MZ);
  const double gw = ee/sw;
  const double cw = sqrt(1-sw*sw);

  const float vu=(2*MW*sw)/ee;
  cout << "vu = " << vu << endl;
  const float vd=(2*MW*sw)/ee;
  cout << "vd = " << vd << endl;
  float yt=sqrt(2)*ymt/vu;
  float gamma_tt = noTTbar? 0: (3./pow(tb,2)*pow(ma0*yt,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymt,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> tt for tanbeta = " << tb << " is " << gamma_tt << " GeV" << endl;


  float yb=sqrt(2)*ymb/vd;
  float gamma_bb = (3.*pow(tb,2)*pow(ma0*yb,2)*sqrt(pow(ma0,4)-4*pow(ma0*ymb,2)))/16.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> bb for tanbeta = " << tb << " is " << gamma_bb << " GeV" << endl;


  float gamma_chichi1 = (gx1*gx1*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> chi chi for gdm = " << gx1 << " is " << 
    gamma_chichi1 << " GeV" << endl;

  cout << "BR(A0->chi chi) for gdm = " << gx1 << " is " << gamma_chichi1/(gamma_tt+gamma_bb+gamma_chichi1) << endl;

  float gamma_chichi2 = (gx2*gx2*pow(ma0,2)*sqrt(pow(ma0,4)-4*pow(ma0*mdm,2)))/8.0/TMath::Pi()/pow(ma0,3);
  cout << "width of A0-> chi chi for gdm = " << gx2 << " is " << 
    gamma_chichi2 << " GeV" << endl;

  cout << "BR(A0->chi chi) for gdm = " << gx2 << " is " << gamma_chichi2/(gamma_tt+gamma_bb+gamma_chichi2) << endl;



  float ratio = gamma_chichi1*(gamma_bb+gamma_tt+gamma_chichi2)/(gamma_bb+gamma_tt+gamma_chichi1)/gamma_chichi2;

  cout << "ratio of BR(A0->chi chi) = " << ratio << endl;

  float dr = xs1/xs2/ratio;
  cout << "For Z'-2HDM cross sections at MZp = " << mzp << " GeV and MA0 = " <<  ma0 << " GeV: " << endl;
  cout << "ratio of Madgraph estimation divided by Eiko's prediction = " << dr << endl;
}
