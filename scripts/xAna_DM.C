//root -q -b -l juwu.C++\(\"inputFile\"\,\"outputFile\"\)


#include <vector>
#include <iostream>
#include <algorithm>
#include <map>
#include <TH1D.h>
#include <TRandom.h>
#include <TLorentzVector.h>
#include <TString.h>
#include <TFile.h>
#include "untuplizer.h"
#include <standalone_LumiReWeighting.cc>


using namespace std;
void xAna_DM(std::string inputFile, bool applyCut=false){


  standalone_LumiReWeighting LumiWeights_central(0);
  //get TTree from file ...
  TreeReader data(inputFile.data());

  bool isData=(data.GetInt("info_isData")==1);

  TH1F* hmass = new TH1F("hmass","", 50,0,1000);
  hmass->Sumw2();

  TH1F* hmzp= (TH1F*)hmass->Clone("hmzp");
  hmzp->SetXTitle("M_{T}(#slash{p}_{T},b#bar{b}) [GeV]");

  TH1F* hptzp = (TH1F*)hmass->Clone("hptzp");
  hptzp->SetXTitle("p_{T}(X) [GeV]");

  TH1F* hjetht= (TH1F*)hmass->Clone("hjetht");
  hjetht->SetXTitle("#Sigma p_{T}(jet) [GeV]");

  TH1F* hht= (TH1F*)hmass->Clone("hht");
  hht->SetXTitle("#Sigma p_{T}^{jet} + #slash{p}_{T}, [GeV]");

  TH1F* hpth = (TH1F*)hmass->Clone("hpth");
  hpth->SetXTitle("p_{T}(b#bar{b}) [GeV]");


  TH1F* hmbb= new TH1F("hmbb","", 50,60,260);
  hmbb->SetXTitle("M(b,#bar{b}) [GeV]");
  hmbb->Sumw2();
  TH1F* hmet= new TH1F("hmet","",  50,0,500);
  hmet->SetXTitle("#slash{p}_{T}");
  hmet->Sumw2();

  TH1F* hdphi = new TH1F("hdphi","",50,0,TMath::Pi());
  hdphi->Sumw2();
  TH1F* hdphibb = (TH1F*)hdphi->Clone("hdphibb");
  hdphibb->SetXTitle("#Delta#phi(#slash{p}_{T},b#bar{b})");
  TH1F* hdphij  = (TH1F*)hdphi->Clone("hdphij");
  hdphij->SetXTitle("#Delta#phi(#slash{p}_{T},jet^{closest})");

  TH1F* hn = new TH1F("hn","",6,-0.5,5.5);
  hn->Sumw2();
  TH1F* hnele = (TH1F*)hn->Clone("hnele");
  hnele->SetXTitle("Number of extra electrons");
  TH1F* hnmuo = (TH1F*)hn->Clone("hmuo");
  hnele->SetXTitle("Number of extra muons");
  TH1F* hnjet = (TH1F*)hn->Clone("hnjet");
  hnjet->SetXTitle("Number of extra jets");

  
  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);

    double PU_weight_central =1;
    if(!isData){
      Float_t ntrue= data.GetFloat("pu_nTrueInt");
      PU_weight_central = LumiWeights_central.weight(ntrue);
    }
    


    std::string* trigName = data.GetPtrString("hlt_trigName");
    Int_t* trigResult = data.GetPtrInt("hlt_trigResult");
    const Int_t nsize = data.GetPtrStringSize();

    bool passTrigger=false;
    for(int it=0; it< nsize; it++)
      {
 	std::string thisTrig= trigName[it];
 	int results = trigResult[it];
	  
 	if(thisTrig.find("HLT_DoubleEle33")!= std::string::npos && results==1)
 	  {
 	    passTrigger=true;
 	    break;
 	  }


      }

    if(isData && !passTrigger)continue;



    // check if there are extra leptons
    Int_t    nEle        = data.GetInt("nEle"); 
    Int_t*   elePassID   = data.GetPtrInt("elePassID");
    Float_t* eleScEta    = data.GetPtrFloat("eleScEta");
    Float_t* elePt       = data.GetPtrFloat("elePt");
    Float_t* eleEta      = data.GetPtrFloat("eleEta");
    Float_t* elePhi      = data.GetPtrFloat("elePhi");
    Float_t* eleM        = data.GetPtrFloat("eleM");


    std::vector<int> myEles;
    for(int ie=0; ie<nEle; ie++){
      if(elePt[ie]<10)continue;
      if(fabs(eleScEta[ie])>2.5)continue;
      if(elePassID[ie]==0)continue;
      myEles.push_back(ie);
      // break;
    }


    Int_t    nMu      = data.GetInt("nMu"); 
    Int_t*   muPassID = data.GetPtrInt("muPassID");
    Float_t* muPt     = data.GetPtrFloat("muPt");
    Float_t* muEta    = data.GetPtrFloat("muEta");
    Float_t* muPhi    = data.GetPtrFloat("muPhi");
    Float_t* muM      = data.GetPtrFloat("muM");

    std::vector<int> myMuos;
    for(int im=0; im < nMu; im++){
      if(muPt[im]<10)continue;
      if(fabs(muEta[im])>2.4)continue;
      if(muPassID[im]==0)continue;
      myMuos.push_back(im);
      // break;
    }


    Float_t met     = data.GetFloat("pfMetCorrPt");
    Float_t metphi  = data.GetFloat("pfMetCorrPhi");

    if(met<150 && applyCut)continue;

    TLorentzVector met_l4(0,0,0,0);
    met_l4.SetPtEtaPhiE(met,
			0,
			metphi,
			met);


    // require a jet 

    Int_t    nAK5jet     = data.GetInt("AK5nJet");
    Float_t* AK5jetPt    = data.GetPtrFloat("AK5jetPt");
    Float_t* AK5jetEta   = data.GetPtrFloat("AK5jetEta");
    Float_t* AK5jetPhi   = data.GetPtrFloat("AK5jetPhi");
    Float_t* AK5jetE     = data.GetPtrFloat("AK5jetEn");
    Float_t* AK5jetCSV  = data.GetPtrFloat("AK5jetCSV");
    Int_t*   AK5jetID    = data.GetPtrInt("AK5jetPassID");

    if(nAK5jet<2)continue;


    std::vector<int> myBs;
    Float_t dphiMin = 9999999;
    int closestJetIndex=-1;
    Float_t sumJetPt=0;
    unsigned int extraJets=0;

    for(int ij=0; ij < nAK5jet; ij++){
      
      if(AK5jetPt[ij]<30)continue;
      if(fabs(AK5jetEta[ij])>2.5)continue;
      if(AK5jetID[ij]==0)continue;

      // check overlaps with muons or electrons
      TLorentzVector jet_l4(0,0,0,0);
      jet_l4.SetPtEtaPhiE(AK5jetPt[ij],
			  AK5jetEta[ij],
			  AK5jetPhi[ij],
			  AK5jetE[ij]);
      bool overlap=false;

      for(unsigned int il=0; il < myEles.size(); il++){
	
	TLorentzVector lep_l4(0,0,0,0);
	int index = myEles[il];
	lep_l4.SetPtEtaPhiM(elePt[index],
			    eleEta[index],
			    elePhi[index],
			    eleM[index]);

	float dR=lep_l4.DeltaR(jet_l4);
	if(dR<0.5)
	  {
	    overlap=true;
	    break;
	  }

      }

      if(overlap)continue;
      for(unsigned int il=0; il < myMuos.size(); il++){
	
	TLorentzVector lep_l4(0,0,0,0);
	int index = myMuos[il];
	lep_l4.SetPtEtaPhiM(muPt[index],
			    muEta[index],
			    muPhi[index],
			    muM[index]);

	float dR=lep_l4.DeltaR(jet_l4);
	if(dR<0.5)
	  {
	    overlap=true;
	    break;
	  }

      }
      if(overlap)continue;


      extraJets++;
      sumJetPt += AK5jetPt[ij];

      float thisdphi=fabs(met_l4.DeltaPhi(jet_l4));
      if(thisdphi < dphiMin)
	{
	  dphiMin = thisdphi;
	  closestJetIndex = ij;
	}


      if(AK5jetCSV[ij]<0.244)continue;
      myBs.push_back(ij);
    }
    if(myBs.size()<2)continue;

    // no extra objects
    extraJets -= 2;

    if(extraJets>1 && applyCut)continue;
    hnjet->Fill(extraJets,PU_weight_central);
    hjetht->Fill(sumJetPt,PU_weight_central);
    hht->Fill(met+sumJetPt,PU_weight_central);

    if(myEles.size()>0 && applyCut)continue;
    hnele->Fill(myEles.size(),PU_weight_central);

    if(myMuos.size()>0 && applyCut)continue;
    hnmuo->Fill(myMuos.size(),PU_weight_central);

    hmet->Fill(met,PU_weight_central);

    if(dphiMin<1.0 && applyCut)continue;
    if(closestJetIndex>=0)
      hdphij->Fill(dphiMin,PU_weight_central);
      


    // look into bjets
    for(unsigned int ij=0; ij < myBs.size(); ij++){      

      TLorentzVector jet1_l4(0,0,0,0);      
      int index1 = myBs[ij];
      jet1_l4.SetPtEtaPhiE(AK5jetPt[index1],
			   AK5jetEta[index1],
			   AK5jetPhi[index1],
			   AK5jetE[index1]);


      for(unsigned int jj=0; jj < ij; jj++){
	
	TLorentzVector jet2_l4(0,0,0,0);      
	int index2 = myBs[jj];
	jet2_l4.SetPtEtaPhiE(AK5jetPt[index2],
			     AK5jetEta[index2],
			     AK5jetPhi[index2],
			     AK5jetE[index2]);

	TLorentzVector h_l4 = jet1_l4+jet2_l4;

	Float_t dphi= fabs(met_l4.DeltaPhi(h_l4));

	if(dphi<2.5 && applyCut)continue;
	hdphibb->Fill(dphi,PU_weight_central);	

	if((h_l4.M()<100 || h_l4.M()>150) && applyCut)continue;
	hmbb->Fill(h_l4.M(),PU_weight_central);

	hpth->Fill(h_l4.Pt());

	TLorentzVector h_l4_tran(0,0,0,0);
	h_l4_tran.SetPtEtaPhiM(
			       h_l4.Pt(),
			       0,
			       h_l4.Phi(),
			       125.0);
	Float_t mT = (h_l4_tran+met_l4).M();
	hmzp->Fill(mT);

	Float_t xpt = (h_l4_tran+met_l4).Pt();
	hptzp->Fill(xpt);

      } // end loop of 2nd bjets

    } // end loop over 1st bjets

  
  } // end of loop over entries


  //save output
  TString endfix=gSystem->GetFromPipe(Form("file=%s; echo \"${file##*/}\"",inputFile.data()));
  TString outputFile = applyCut? ("cutDMHisto_" + endfix): ("DMHisto_" + endfix);

  TFile* outFile = new TFile(outputFile.Data(),"recreate");
  hmzp     ->Write();
  hptzp    ->Write();
  hjetht   ->Write();
  hht      ->Write();
  hpth     ->Write();
  hmbb     ->Write();
  hmet     ->Write();
  hdphibb  ->Write();
  hdphij   ->Write();
  hnele    ->Write();
  hnmuo    ->Write();
  hnjet    ->Write();

  outFile->Close();

}
