#include <TLegend.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <TH1D.h>
#include <TRandom.h>
#include <TLorentzVector.h>
#include <TFile.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TH1F.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TROOT.h>
#include "TImage.h"
#include "TSystem.h"
#include "TStyle.h"
#include <TClonesArray.h>
#include <fstream>
#include <cmath>
#include <TSystem.h>
#include <string>
#include <sstream>
#include "setNCUStyle.C"
#include<TH2.h>
#include "TLine.h"
#include "TF1.h"
#include"TGraphAsymmErrors.h"
#include "TLatex.h"

void makeXsec(){
	TCanvas* c1,*c2;
	setNCUStyle();
	c1 = new TCanvas("c1","",1150,768);
	int massZ[8]={600,800,1000,1200,1400,1700,2000,2500};
	int inputZ[8]={2,4,6,8,10,13,16,21};
	int massA[6]={300,400,500,600,700,800};
	fstream file1("xsec.txt");
	TH2D* th2=new TH2D("xsec","xsec",8,0,8,6,0,6);
	double db[8][6]={0};
	for (int i=0;i<8;i++){
		for (int j=0;j<6;j++){
			file1>>db[i][j];
			cout<<db[i][j];
			th2->SetBinContent(i+1,j+1,db[i][j]);
		}
	}
	for(int i=0;i<8;i++){
		
		th2->GetXaxis()->SetBinLabel(i+1,Form("%d",massZ[i]));
		
	}
	for(int j=0;j<6;j++){
		
		th2->GetYaxis()->SetBinLabel(j+1,Form("%d",massA[j]));
	
	}

		th2->SetXTitle("m_{Zp}[GeV]");
		th2->SetYTitle("m_{A0}[GeV]");
		th2->SetMarkerSize(2);

	gStyle->SetPaintTextFormat(".2e ");
	
	th2->Draw("colz,text");
	c1->SaveAs("xsec.png");
	
	TFile* outFile ;
	outFile= new TFile("output.root","recreate");
	th2->Write();
	outFile->Close();
}


