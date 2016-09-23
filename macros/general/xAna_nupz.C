
#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include "untuplizer.h"
#include <vector>
#include <iostream>

using namespace std;
const double MWPDG = 80.385;


void xAna_nupz(std::string inputFile)
{
  
  TreeReader data(inputFile.data());
  TH1F* hdR = new TH1F("hdR","#Delta R",500,0,5);
  TH1F* hdTheta = new TH1F("hdTheta","#theta",500,-TMath::Pi(),TMath::Pi());
  TH1F* hcosTheta = new TH1F("hcosTheta","cos(|#theta|)",500,0,1);
  TH1F* hdZ = new TH1F("hdZ","#Delta P_{z}",400,-100,100);
  TH1F* hMW = new TH1F("hMW","M_{W}",500,50,150);

  TH1F* hdR_guess[2];
  TH1F* hdTheta_guess[2];
  TH1F* hcosTheta_guess[2];
  TH1F* hMW_guess[2];
  TH1F* hdZ_guess[2];
  
  for(int i=0; i<2; i++)
    {
      hdR_guess[i] = (TH1F*)hdR->Clone(Form("hdR_guess%d",i));
      hdTheta_guess[i] = (TH1F*)hdTheta->Clone(Form("hdTheta_guess%d",i));
      hcosTheta_guess[i] = (TH1F*)hcosTheta->Clone(Form("hcosTheta_guess%d",i));
      hMW_guess[i] = (TH1F*)hMW->Clone(Form("hMW_guess%d",i));
      hdZ_guess[i] = (TH1F*)hdZ->Clone(Form("hdZ_guess%d",i));
    }

  Long64_t ntotal=0;
  Long64_t ncorrect=0;
  Long64_t nwrong=0;
  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    // if(jEntry>100)break;
    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);

    Int_t nGenPar = data.GetInt("nGenPar");

    Int_t* genParId = data.GetPtrInt("genParId");
    Int_t* genParSt = data.GetPtrInt("genParSt");
    Int_t* genDa1   = data.GetPtrInt("genDa1");
    Int_t* genDa2   = data.GetPtrInt("genDa2");
    Float_t* genParPt = data.GetPtrFloat("genParPt");
    Float_t* genParEta= data.GetPtrFloat("genParEta");
    Float_t* genParPhi= data.GetPtrFloat("genParPhi");
    Float_t* genParM  = data.GetPtrFloat("genParM");

    // first look for a status=3 muon from W
    std::vector<unsigned int> Ws;
    std::vector<unsigned int> neutrinos;
    std::vector<unsigned int> muons;
    
    

    for(unsigned int i=0; i < nGenPar;i++)
      {
	  
	int PID = abs(genParId[i]);
	int status = genParSt[i];

	if(abs(PID)!=24)continue;
	if(status!=3)continue;
	if(genDa1[i] < 0 || genDa2[i] < 0)continue;
	if(genParPt[i]<300)continue;
 
	int Da1 = abs(genParId[genDa1[i]]);
	int St1 = genParSt[genDa1[i]];
	int Da2 = abs(genParId[genDa2[i]]);
	int St2 = genParSt[genDa2[i]];

	if(Da1==13 && St1==3 && Da2==14 && St2==3)
	  {
	    Ws.push_back(i);
	    muons.push_back(genDa1[i]);
	    neutrinos.push_back(genDa2[i]);
	  } 
	else
	  if(Da1==14 && St1==3 && Da2==13 && St2==3)
	    {
	      Ws.push_back(i);
	      muons.push_back(genDa2[i]);
	      neutrinos.push_back(genDa1[i]);
	    } 
	  
      } // end of loop over genparticles
      
    if(muons.size()==0  || neutrinos.size()==0 || 
       Ws.size() == 0 || 
       muons.size() != neutrinos.size() || 
       Ws.size()  != muons.size()
       )continue;

    for(unsigned int i=0; i < Ws.size(); i++){
	
      unsigned iW = Ws[i];
      double MW = genParM[iW];
      unsigned int imu = muons[i];
      TLorentzVector mu_p4(0,0,0,0);
      double Mmu = genParM[imu];
	  
      mu_p4.SetPtEtaPhiM(
			 genParPt[imu],
			 genParEta[imu],
			 genParPhi[imu],
			 genParM[imu]
			 );

      unsigned int inu = neutrinos[i];
      TLorentzVector nu_p4(0,0,0,0);
      nu_p4.SetPtEtaPhiM(
			 genParPt[inu],
			 genParEta[inu],
			 genParPhi[inu],
			 genParM[inu]
			 );

      double alpha = MW*MW - Mmu*Mmu
	+ 2*mu_p4.Px()*nu_p4.Px() + 2*mu_p4.Py()*nu_p4.Py();
	
      double beta = 2*mu_p4.Pz();
	
      double gamma = Mmu*Mmu + mu_p4.Pt()*mu_p4.Pt();

      double upsilon = 4*mu_p4.E()*mu_p4.E()*nu_p4.Pt()*nu_p4.Pt()
	- alpha*alpha;
	
      double zetta = alpha*alpha*beta*beta - 4*gamma*upsilon;
      double nupz1 = gamma>1e-6 && zetta>=0?
	(alpha*beta + sqrt(zetta))*0.25/gamma: -999999;

      double nupz2 = gamma>1e-6 && zetta>=0?
	(alpha*beta - sqrt(zetta))*0.25/gamma: -999999;

      // cout << "real neutrino pz = " << nu_p4.Pz() << " \t" 
      // 	   << "two solutions are: " << 
      // 	nupz1 << " and " << nupz2 << endl;



      // use the rapidity difference to compute the 
      // two solutions
      // m_W^2 = m_mu^2 + 2m_T^mu*m_T^nu*cosh(delta_y) - 2Pt_mu dot Pt_nu

      double mT_mu = sqrt(Mmu*Mmu + mu_p4.Px()*mu_p4.Px() + mu_p4.Py()*mu_p4.Py());
      double mT_nu = sqrt(nu_p4.Px()*nu_p4.Px() + nu_p4.Py()*nu_p4.Py());
      double dY = fabs(TMath::ACosH((MW*MW - Mmu*Mmu + 2*mu_p4.Px()*nu_p4.Px() + 2*mu_p4.Py()*nu_p4.Py())*0.5/mT_mu/mT_nu));
      
      double Y1 = mu_p4.Rapidity() + dY;
      double Pz1 = mT_nu*TMath::SinH(Y1);
      TLorentzVector nu1_p4(0,0,0,0);
      nu1_p4.SetPxPyPzE(nu_p4.Px(),
			nu_p4.Py(),
			Pz1,
			sqrt(mT_nu*mT_nu + Pz1*Pz1));
      double dtheta1 = fabs(nu1_p4.Vect().Theta() - mu_p4.Vect().Theta());
      
      double Y2 = mu_p4.Rapidity() - dY;
      double Pz2 = mT_nu*TMath::SinH(Y2);
      TLorentzVector nu2_p4(0,0,0,0);
      nu2_p4.SetPxPyPzE(nu_p4.Px(),
			nu_p4.Py(),
			Pz2,
			sqrt(mT_nu*mT_nu + Pz2*Pz2));
      double dtheta2 = fabs(nu2_p4.Vect().Theta() - mu_p4.Vect().Theta());

      TLorentzVector nu_l4[2];      

      // let neutrino1 always be the more correct solution
      if(dtheta1 > dtheta2)
	{
	  nu_l4[0] = nu2_p4;
	  nu_l4[1] = nu1_p4;
	}
      else
	{
	  nu_l4[0] = nu1_p4;
	  nu_l4[1] = nu2_p4;
	}

      double dz[2]={0};
      for(int k=0; k<2; k++)
	{
	  hdR_guess[k]->Fill( mu_p4.DeltaR(nu_l4[k]) );
	  hdTheta_guess[k]->Fill ( TVector2::Phi_mpi_pi(mu_p4.Theta()- nu_l4[k].Theta()) );
	  hcosTheta_guess[k]->Fill ( TMath::Cos(fabs(mu_p4.Theta()- nu_l4[k].Theta())) );
	  hMW_guess[k]->Fill( (mu_p4 + nu_l4[k]).M() );
	  dz[k] = nu_l4[k].Pz() - nu_p4.Pz() ;
	  hdZ_guess[k]->Fill( dz[k]);

	}
      ntotal++;

      if(fabs(dz[0])<fabs(dz[1]))
	ncorrect++;
      else
	nwrong++;

      // cout << "MW = " << MW << " and " << (nu1_p4+mu_p4).M() << " and " << (nu2_p4+mu_p4).M() << endl;
			
      // cout << "real neutrino pz = " << nu_p4.Pz() << " \t" 
      //  	   << "two solutions are: " << 
      //  	Pz1 << " and " << Pz2 << endl;
      
    } // end of loop over W
      
  }

  cout<< "ntotal = " << ntotal << " and the correction rate =   "
      << (double)ncorrect/(double)ntotal <<  endl;

 TFile* outFile = new TFile("nupz.root","recreate");       

  for(int i=0; i<2; i++)
    {
      hdR_guess[i] ->Write();
      hdTheta_guess[i] ->Write();
      hcosTheta_guess[i] ->Write();
      hMW_guess[i] ->Write();
      hdZ_guess[i] ->Write();
    }
 
  outFile->Close();
}
