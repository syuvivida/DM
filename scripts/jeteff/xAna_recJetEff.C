// example code to run Bulk Graviton->ZZ->ZlepZhad selections on electron-channel

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <TString.h>
#include <map>
#include <TH1D.h>
#include <TFile.h>
#include "untuplizer.h"
#include <TClonesArray.h>
#include <TLorentzVector.h>
#include <TEfficiency.h>
#include <TSystemDirectory.h>
#include <TList.h>

using namespace std;
void xAna_recJetEff(std::string inputFile, bool myDefinition=true){

  std::vector<string> infiles;
  TString outputFile;
  string myName=myDefinition? "myDefinition":"JETMET";

  if(inputFile.find(".root")!= std::string::npos)
    { 
      //outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*ZprimeToZhToZlephbb_25ns/}; echo \"jeteff_%s_${test}\"",inputFile.data(),
      // outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*ZprimeToZhToZlephbb/}; echo \"jeteff_%s_${test}\"",inputFile.data(),
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*MonoHSignalTreesMerged_20150919/}; echo \"jeteff_%s_${test}\"",inputFile.data(),
					   myName.data()));
      cout << "output file name = " << outputFile.Data() << endl;
      infiles.push_back(inputFile.data());
    }
  else
    {
      //outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*jet_CMSSW747/}; echo \"jeteff_%s_${test}.root\"",inputFile.data(),
      //outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*signalMC/}; echo \"jeteff_%s_${test}.root\"",inputFile.data(),
      outputFile=gSystem->GetFromPipe(Form("file=%s; test=${file##*SPRING15/}; echo \"jeteff_%s_${test}.root\"",inputFile.data(),
					   myName.data()));
      cout << "output file name = " << outputFile.Data() << endl;      
      TSystemDirectory *base = new TSystemDirectory("root","root");

      base->SetDirectory(inputFile.data());
      TList *listOfFiles = base->GetListOfFiles();
      TIter fileIt(listOfFiles);
      TFile *fileH = new TFile();
      int nfile=0;
      while(fileH = (TFile*)fileIt()) {
	std::string fileN = fileH->GetName();
	if( fileH->IsFolder())  continue;
	if(fileN.find("root") == std::string::npos)continue;
	fileN = inputFile + "/" + fileN;
	cout << fileN.data() << endl;
	nfile++;
	infiles.push_back(fileN);
      }

      std::cout << "Opened " << nfile << " files" << std::endl;

    }

  

  cout << "output file name = " << outputFile.Data() << endl;
  //get TTree from file ...
  TreeReader data(infiles);
  
  Long64_t nTotal=0;
  Long64_t nPass[20]={0};

  TH1F* h_mass=new TH1F("h_mass","",60,60,180);

  TH1F* h_fatjetmass=(TH1F*)h_mass->Clone("h_fatjetmass");
  h_fatjetmass->SetXTitle("Fatjet Pruned mass [GeV]");

  TH1F* h_bbmass=(TH1F*)h_mass->Clone("h_bbmass");
  h_bbmass->SetXTitle("M_{bb} [GeV]");
  

  // TH1F* h_pt=new TH1F("h_pt","",125,0,2500);
  TH1F* h_pt=new TH1F("h_pt","",75,0,1500);
  h_pt->SetXTitle("Higgs p_{T} [GeV]");

  TH1F* h_deno=(TH1F*)h_pt->Clone("h_deno"); 
  TH1F* h_numrNoMatch=(TH1F*)h_pt->Clone("h_numrNoMatch");
  TH1F* h_numrFAT=(TH1F*)h_pt->Clone("h_numrFAT");
  TH1F* h_numrTHIN=(TH1F*)h_pt->Clone("h_numrTHIN");

  TH1F* h_numrFATMass=(TH1F*)h_pt->Clone("h_numrFATMass");
  TH1F* h_numrTHINMass=(TH1F*)h_pt->Clone("h_numrTHINMass");

  TH1F* h_numrFATCSV=(TH1F*)h_pt->Clone("h_numrFATCSV");
  TH1F* h_numrSubCSV1=(TH1F*)h_pt->Clone("h_numrSubCSV1");
  TH1F* h_numrSubCSV12=(TH1F*)h_pt->Clone("h_numrSubCSV12");
  TH1F* h_numrSubCSV2=(TH1F*)h_pt->Clone("h_numrSubCSV2");
  TH1F* h_numrTHINCSV=(TH1F*)h_pt->Clone("h_numrTHINCSV");


  TH1F* h_dR=new TH1F("h_dR","",120,0,6);
  TH1F* h_dR_H  = (TH1F*)h_dR->Clone("h_dR_H");
  TH1F* h_dR_bb_fat = (TH1F*)h_dR->Clone("h_dR_bb_fat");
  TH1F* h_dR_bb_thin = (TH1F*)h_dR->Clone("h_dR_bb_thin");

  //Event loop
  for(Long64_t jEntry=0; jEntry<data.GetEntriesFast() ;jEntry++){

    if (jEntry % 50000 == 0)
      fprintf(stderr, "Processing event %lli of %lli\n", jEntry + 1, data.GetEntriesFast());

    data.GetEntry(jEntry);
    nTotal ++;

    // 0. check the generator-level information and make sure there is a Z->e+e-
    Int_t nGenPar        = data.GetInt("nGenPar");
    Int_t* genParId      = data.GetPtrInt("genParId");
    Int_t* genDa1      = data.GetPtrInt("genDa1");
    Int_t* genDa2      = data.GetPtrInt("genDa2");

    TClonesArray* genParP4 = (TClonesArray*) data.GetPtrTObject("genParP4");


    // 1. make sure there is a H->bb
    bool hasHadron=false;
    int bindex[2]={-1,-1};
    int higgsIndex=-1;

    for(int ig=0; ig < nGenPar; ig++){

      if(genParId[ig]!=25)continue;
      int da1=genDa1[ig];
      int da2=genDa2[ig];

      if(da1<0 || da2<0)continue;
      int da1pdg = genParId[da1];
      int da2pdg = genParId[da2];

      if(abs(da1pdg)!=5)
	continue;
      if(abs(da2pdg)!=5)
	continue;
      
      higgsIndex=ig;
      bindex[0]=da1;
      bindex[1]=da2;

      hasHadron=true;
      if(hasHadron)break;

    }
    
    if(!hasHadron)continue;
    nPass[0]++;


    TLorentzVector* higgs_p4 = (TLorentzVector*)genParP4->At(higgsIndex);    
    h_deno->Fill(higgs_p4->Pt());

    TLorentzVector* b_p4[2];
    for(int i=0;i<2;i++)
      b_p4[i]= (TLorentzVector*)genParP4->At(bindex[i]);    

    // for(int i=0;i<2;i++)
    //   std::cout << "b index " << i <<  " = " << bindex[i] << endl;

    //// FAT Jet
     
    Int_t nFATJet         = data.GetInt("FATnJet");
    TClonesArray* fatjetP4 = (TClonesArray*) data.GetPtrTObject("FATjetP4");
    Float_t*  fatjetCISVV2 = data.GetPtrFloat("FATjetCISVV2");
    Float_t*  fatjetPRmass = data.GetPtrFloat("FATjetPRmass");
    // Float_t*  fatjetPRmassCorr = data.GetPtrFloat("FATjetPRmassL2L3Corr");
    Int_t*   nSubSoftDropJet = data.GetPtrInt("FATnSubSDJet");
    vector<float>   *subjetSDCSV =  data.GetPtrVectorFloat("FATsubjetSDCSV", nFATJet);
    vector<float>   *subjetSDPx  =  data.GetPtrVectorFloat("FATsubjetSDPx", nFATJet);
    vector<float>   *subjetSDPy  =  data.GetPtrVectorFloat("FATsubjetSDPy", nFATJet);
    vector<float>   *subjetSDPz  =  data.GetPtrVectorFloat("FATsubjetSDPz", nFATJet);
    vector<float>   *subjetSDE   =  data.GetPtrVectorFloat("FATsubjetSDE", nFATJet);

    const float dRFATMax=myDefinition? 0.8: 0.1;
    bool findAFATJet=false;
    bool findAFATJetMass=false;
    bool findAFATB=false;
    bool findASubB1=false;
    bool findASubB12=false;
    bool findASubB2=false;

    for(int ij=0; ij<nFATJet; ij++)
      {
    	
     	TLorentzVector* thisJet = (TLorentzVector*)fatjetP4->At(ij);
	if(thisJet->Pt()<200)continue;
	if(fabs(thisJet->Eta())>2.5)continue;

	h_dR_H->Fill(thisJet->DeltaR(*higgs_p4));
	h_dR_bb_fat->Fill(thisJet->DeltaR(*b_p4[0]));
	h_dR_bb_fat->Fill(thisJet->DeltaR(*b_p4[1]));

	
	if(thisJet->DeltaR(*b_p4[0]) > dRFATMax && myDefinition)continue;
	if(thisJet->DeltaR(*b_p4[1]) > dRFATMax && myDefinition)continue;

	if(thisJet->DeltaR(*higgs_p4) > dRFATMax && !myDefinition)continue;

	findAFATJet=true;


	h_fatjetmass->Fill(fatjetPRmass[ij]);
	if(fatjetPRmass[ij]<70 || fatjetPRmass[ij]>140)continue;

	// h_fatjetmass->Fill(fatjetPRmassCorr[ij]);	
	// if(fatjetPRmassCorr[ij]<80 || fatjetPRmassCorr[ij]>150)continue;

	findAFATJetMass=true;


	if(fatjetCISVV2[ij] > 0.605 && fatjetCISVV2[ij] < 1)	  
	  findAFATB=true;

	int nSubBTag=0;
	if( nSubSoftDropJet[ij]!=2)
	  continue;

	TLorentzVector sub[2];
	for(int is=0; is < nSubSoftDropJet[ij]; is++)
	  {
	    if(subjetSDCSV[ij][is] < 0.605 || subjetSDCSV[ij][is] >1)continue;
	    nSubBTag++;

	    sub[is].SetPxPyPzE(
			       subjetSDPx[ij][is],
			       subjetSDPy[ij][is],
			       subjetSDPz[ij][is],
			       subjetSDE[ij][is]
			       );
	    
	  }

	float subjetdR = sub[0].DeltaR(sub[1]);

	if(nSubBTag>=1)findASubB1=true;
	if(nSubBTag==2)findASubB2=true;
	if( (subjetdR<0.3  && nSubBTag==1) || 
	    (subjetdR>0.3  && nSubBTag==2))
	  findASubB12=true;

	
      }

    if(findAFATJet)
      {
	h_numrFAT->Fill(higgs_p4->Pt());
	nPass[1]++;

	if(findAFATJetMass){

	  h_numrFATMass->Fill(higgs_p4->Pt());
	  nPass[2]++;

	  if(findAFATB)
	    {
	      h_numrFATCSV->Fill(higgs_p4->Pt());
	      nPass[3]++;
	    }

	  if(findASubB1)
	    {
	      h_numrSubCSV1->Fill(higgs_p4->Pt());
	      nPass[4]++;
	    }
	  if(findASubB12)
	    {
	      h_numrSubCSV12->Fill(higgs_p4->Pt());
	      nPass[5]++;
	    }
	  if(findASubB2)
	    {
	      h_numrSubCSV2->Fill(higgs_p4->Pt());
	      nPass[6]++;
	    }


	} // pass mass window
	
      } // find a fatjet

    //// THIN Jet
    Int_t nTHINJet         = data.GetInt("THINnJet");
    TClonesArray* thinjetP4 = (TClonesArray*) data.GetPtrTObject("THINjetP4");
    Float_t*  thinjetCISVV2 = data.GetPtrFloat("THINjetCISVV2");

    bool findATHINJetPair=false;
    bool findATHINMassPair=false;
    bool findATHINBPair=false;
    const float dRTHINMax=myDefinition? 0.4 : 0.1;
    int bjetindex[2]={-1,-1};
    for(int ij=0; ij<nTHINJet; ij++)
      {
    	
     	TLorentzVector* thisJet = (TLorentzVector*)thinjetP4->At(ij);
	if(thisJet->Pt()<30)continue;
	if(fabs(thisJet->Eta())>2.5)continue;

	h_dR_bb_thin->Fill(thisJet->DeltaR(*b_p4[0]));
	h_dR_bb_thin->Fill(thisJet->DeltaR(*b_p4[1]));

	
	// exclude when the jet is matched to both
	if(thisJet->DeltaR(*b_p4[0]) < dRTHINMax && thisJet->DeltaR(*b_p4[1]) < dRTHINMax)continue;

	// exclude when the jet is not matched to either
	if(thisJet->DeltaR(*b_p4[0]) > dRTHINMax && thisJet->DeltaR(*b_p4[1]) > dRTHINMax)continue;


	for(int jj=0; jj<ij; jj++)
	  {

	    TLorentzVector* thatJet = (TLorentzVector*)thinjetP4->At(jj);
	    if(thatJet->Pt()<30)continue;
	    if(fabs(thatJet->Eta())>2.5)continue;

	    // exclude when the jet is matched to both
	    if(thatJet->DeltaR(*b_p4[0]) < dRTHINMax && thatJet->DeltaR(*b_p4[1]) < dRTHINMax)continue;

	    // exclude when the jet is not matched to either
	    if(thatJet->DeltaR(*b_p4[0]) > dRTHINMax && thatJet->DeltaR(*b_p4[1]) > dRTHINMax)continue;
	    
	    if( (thisJet->DeltaR(*b_p4[0]) < dRTHINMax && thisJet->DeltaR(*b_p4[1]) > dRTHINMax &&
		 thatJet->DeltaR(*b_p4[0]) > dRTHINMax && thatJet->DeltaR(*b_p4[1]) < dRTHINMax) || 
		(thisJet->DeltaR(*b_p4[0]) > dRTHINMax && thisJet->DeltaR(*b_p4[1]) < dRTHINMax &&
		 thatJet->DeltaR(*b_p4[0]) < dRTHINMax && thatJet->DeltaR(*b_p4[1]) > dRTHINMax)		 
		)

	      {
		findATHINJetPair=true;


		float bbmass = (*thisJet+*thatJet).M();

		h_bbmass->Fill(bbmass);

		if(bbmass< 80 || bbmass>150)continue;

		findATHINMassPair=true;

		if(thinjetCISVV2[ij]>0.605 && thinjetCISVV2[ij]<1 && 
		   thinjetCISVV2[jj]>0.605 && thinjetCISVV2[jj]<1)   
		  {
		    findATHINBPair=true;
		    bjetindex[0] = ij;
		    bjetindex[1] = jj;
		  }
	      }
	    
	    if(findATHINJetPair && findATHINMassPair && findATHINBPair)
	      break;

	  }
	
      } //end of double loop


    
    if(findATHINJetPair)
      {

	// for(int i=0;i<2;i++)
	//   std::cout << "b jet index " << i <<  " = " << bjetindex[i] << endl;

	h_numrTHIN->Fill(higgs_p4->Pt());
	nPass[7]++;

	if(findATHINMassPair){

	  h_numrTHINMass->Fill(higgs_p4->Pt());
	  nPass[8]++;
	  
	  if(findATHINBPair)
	    {
	      // for(int i=0;i<2;i++)
	      //   std::cout << "real b jet index " << i <<  " = " << bjetindex[i] << endl;
	      
	      h_numrTHINCSV->Fill(higgs_p4->Pt());
	      nPass[9]++;
	    }
	  }
      }
    

    if( !findAFATJet && !findATHINJetPair)
      h_numrNoMatch->Fill(higgs_p4->Pt());

   

  } // end of loop over entries

  TEfficiency* nEff = new TEfficiency(*h_numrNoMatch,*h_deno);
  nEff->SetName("noJetEff");
  nEff->SetTitle("no match efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEff = new TEfficiency(*h_numrFAT,*h_deno);
  fEff->SetName("fatJetEff");
  fEff->SetTitle("AK8 jet efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* tEff = new TEfficiency(*h_numrTHIN,*h_deno);
  tEff->SetName("thinJetEff");
  tEff->SetTitle("Two AK4 jet efficiency; Higgs p_{T} [GeV]; Efficiency");


  TEfficiency* fEff2 = new TEfficiency(*h_numrFATMass,*h_deno);
  fEff2->SetName("fatJetEff2");
  fEff2->SetTitle("AK8 jet reconstruction + mass cut efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* tEff2 = new TEfficiency(*h_numrTHINMass,*h_deno);
  tEff2->SetName("thinJetEff2");
  tEff2->SetTitle("Two AK4 jet reconstruction + mass cut efficiency; Higgs p_{T} [GeV]; Efficiency");


  TEfficiency* fEffMass = new TEfficiency(*h_numrFATMass, *h_numrFAT);
  fEffMass->SetName("fatJetMassEff");
  fEffMass->SetTitle("Higgs mass cut efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* tEffMass = new TEfficiency(*h_numrTHINMass, *h_numrTHIN);
  tEffMass->SetName("thinJetMassEff");
  tEffMass->SetTitle("Higgs mass cut efficiency; Higgs p_{T} [GeV]; Efficiency");


  TEfficiency* fEffCSV = new TEfficiency(*h_numrFATCSV,*h_numrFATMass);
  fEffCSV->SetName("fatJetCSVEff");
  fEffCSV->SetTitle("AK8 Fatjet CSV efficiency; Higgs p_{T} [GeV]; Efficiency");


  TEfficiency* fEffCSV1 = new TEfficiency(*h_numrSubCSV1,*h_numrFATMass);
  fEffCSV1->SetName("subJetCSVEff1");
  fEffCSV1->SetTitle("AK8 Subjet CSV efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEffCSV12 = new TEfficiency(*h_numrSubCSV12,*h_numrFATMass);
  fEffCSV12->SetName("subJetCSVEff12");
  fEffCSV12->SetTitle("AK8 Subjet CSV efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEffCSV2 = new TEfficiency(*h_numrSubCSV2,*h_numrFATMass);
  fEffCSV2->SetName("subJetCSVEff2");
  fEffCSV2->SetTitle("AK8 Subjet CSV efficiency; Higgs p_{T} [GeV]; Efficiency");



  TEfficiency* tEffCSV = new TEfficiency(*h_numrTHINCSV,*h_numrTHINMass);
  tEffCSV->SetName("thinJetCSVEff");
  tEffCSV->SetTitle("Two AK4 CSV efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEffTotal = new TEfficiency(*h_numrFATCSV,*h_deno);
  fEffTotal->SetName("fatJetTotalEff");
  fEffTotal->SetTitle("AK8 total efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* tEffTotal = new TEfficiency(*h_numrTHINCSV,*h_deno);
  tEffTotal->SetName("thinJetTotalEff");
  tEffTotal->SetTitle("Two AK4 total efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEffTotal1 = new TEfficiency(*h_numrSubCSV1,*h_deno);
  fEffTotal1->SetName("fatJetTotalEff1");
  fEffTotal1->SetTitle("AK8 total efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEffTotal12 = new TEfficiency(*h_numrSubCSV12,*h_deno);
  fEffTotal12->SetName("fatJetTotalEff12");
  fEffTotal12->SetTitle("AK8 total efficiency; Higgs p_{T} [GeV]; Efficiency");

  TEfficiency* fEffTotal2 = new TEfficiency(*h_numrSubCSV2,*h_deno);
  fEffTotal2->SetName("fatJetTotalEff2");
  fEffTotal2->SetTitle("AK8 total efficiency; Higgs p_{T} [GeV]; Efficiency");


  TFile* outFile = new TFile(outputFile.Data(),"recreate");

  h_dR_H       ->Write();
  h_dR_bb_fat  ->Write();
  h_dR_bb_thin ->Write();

  h_deno->Write();
  h_numrNoMatch->Write();
  h_numrFAT->Write();
  h_numrTHIN->Write();
  h_numrFATMass->Write();
  h_numrTHINMass->Write();
  h_numrFATCSV->Write();
  h_numrTHINCSV->Write();

  h_numrSubCSV1->Write();
  h_numrSubCSV12->Write();
  h_numrSubCSV2->Write();


  h_fatjetmass->Write();
  h_bbmass->Write();


  nEff->Write();

  fEff->Write();
  tEff->Write(); 

  fEff2->Write();
  tEff2->Write(); 

  fEffTotal->Write();
  tEffTotal->Write(); 

  fEffTotal1->Write();
  fEffTotal12->Write();
  fEffTotal2->Write();

  fEffMass->Write();
  tEffMass->Write(); 

  fEffCSV->Write();
  tEffCSV->Write(); 


  fEffCSV1->Write();
  fEffCSV12->Write();
  fEffCSV2->Write();


  outFile->Close();
  std::cout << "nTotal    = " << nTotal << std::endl;
  for(int i=0;i<20;i++)
    if(nPass[i]>0)
      std::cout << "nPass[" << i << "]= " << nPass[i] << std::endl;
    
  
  

}
