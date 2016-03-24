#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "DrawTools.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLegend.h"
#include "TGaxis.h"
#include "TVectorD.h"
#include "TF1.h"
#include "TGraphErrors.h"
#include "TString.h"

#include <vector>
#include <string>
#include <map>
#include <algorithm>


int main( int argc, char* argv[] ) {

  DrawTools::setStyle();

  std::string setup = "WLS";
  std::string energy = "50GeV";

  if( argc>1 ) {
    std::string setup_str(argv[1]);
    setup = setup_str;
  }else{
    std::cout << "Usage:" << std::endl;
    std::cout << "./compareTimingEnergy ([setup]) " << std::endl;
    exit(12345);
  }

  std::cout<<"plotting for setup:"<<setup<<std::endl;

  std::map<TString,float> energies;
  energies["20GeV"]=20;
  energies["50GeV"]=50;
  energies["100GeV"]=100;
  energies["200GeV"]=200;
  
  TH2F * axes = new TH2F("axes","axes",200,0.,250.,300,0,400);
  TGraphErrors* resVsEnergy = new TGraphErrors(0);
  resVsEnergy->SetName("resVsEnergy");
  resVsEnergy->SetMarkerStyle(20);
  resVsEnergy->SetMarkerSize(1.6);
  resVsEnergy->SetMarkerColor(kBlue);

  axes->GetYaxis()->SetTitle("#sigma_{t} [ps]");
  axes->GetXaxis()->SetTitle("Energy [GeV]");


  int i=0;
  for(std::map<TString,float>::const_iterator it = energies.begin();it!=energies.end(); it++){
    TFile* file = TFile::Open("outFiles/plotterTiming_"+setup+"_"+it->first+".root");
    TVectorD* resValue = (TVectorD*)file->Get("resValueTime");
    TVectorD* resValueErr = (TVectorD*) file->Get("resValueTimeErr");

    std::cout<< (*resValue)[0]<<std::endl;
    resVsEnergy->SetPoint(i,it->second, (*resValue)[0]);
    resVsEnergy->SetPointError(i,0, (*resValueErr)[0]);
    ++i;  
 }

  TCanvas c1;
  TF1* f = new TF1("f","[0]/sqrt(x)",0,250);
  axes->Draw();
  resVsEnergy->Fit(f,"R",0,250);
  resVsEnergy->Draw("Psame");
  c1.SaveAs("outFiles/plots/resVsEnergy.pdf");
  c1.SaveAs("outFiles/plots/resVsEnergy.png");

  return 0;

}
