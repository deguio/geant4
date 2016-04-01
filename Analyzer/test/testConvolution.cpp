#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooGenericPdf.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "RooTFnBinding.h"
#include "RooTFnPdfBinding.h"
#include  "RooNumConvPdf.h"


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
    std::cout << "./testConvolution ([setup]) " << std::endl;
    exit(12345);
  }

  std::cout<<"plotting for setup:"<<setup<<std::endl;

  std::map<TString,float> energies;
  energies["100GeV"]=100;
  TH1F* waveform;

  for(std::map<TString,float>::const_iterator it = energies.begin();it!=energies.end(); it++){
    TFile* file = TFile::Open("outFiles/plotterTiming_"+setup+"_"+it->first+".root");
    waveform = (TH1F*) file->Get("timeArrival");
    waveform->Sumw2();
    waveform->Scale(1./waveform->Integral());
  }




  RooRealVar t("t","t", 0, 200);
  RooDataHist wave("wave","dataset with t",t,RooFit::Import(*waveform) );

  RooHistPdf histpdf1("histpdf1","histpdf1",t,wave);


  TF1 *f = new  TF1("f", "pow(exp(1.)*[1]*(x)/[0],[0])*exp(-[1]*(x))",0.,200.);
  f->SetParameters(3.0, 0.8);

  RooAbsPdf* rfa1 = RooFit::bindPdf(f,t);

  //  RooRealVar tau("tau","tau",1.22451e+7,1.e+5,pow(10,30)) ;
  //  RooRealVar timeshift("timeshift","timeshift",0.03e-6,0,0.05e-6) ;

  //  RooGenericPdf crrc("crrc","(@0-0.05e-6)*@1*exp(-(@0-0.05e-6)*@1)",RooArgSet(t,tau)) ;


  //  TFile* outFile = TFile::Open("outFiles/testConvolution_"+setup+".root");

  std::string fileName(Form("outFiles/testConvolution_%s",setup.c_str()));
  fileName+=".root";
  TFile* outFile = TFile::Open(fileName.c_str(),"recreate");

  RooNumConvPdf convolutionShaping("convolutionShaping","exp (X) crrc",t,histpdf1,*rfa1) ;
  RooRealVar center("center","center",50,0,400);
  RooRealVar width("width","width",40,0,400);

  convolutionShaping.setConvolutionWindow(center,width,1);
  convolutionShaping.verboseEval(0);
  convolutionShaping.convIntConfig().setEpsAbs(0.001);//absolute precision
  convolutionShaping.convIntConfig().setEpsRel(0.001);//relative precision

  RooPlot* frame1 = t.frame(RooFit::Title("histogram pdf")) ;
  //  rfa1->plotOn(frame1);
  wave.plotOn(frame1,RooFit::LineColor(kBlack),RooFit::DrawOption("L"),RooFit::LineWidth(2));
  convolutionShaping.plotOn(frame1,RooFit::LineColor(kRed)) ;

  TH1* histo_shaped =  convolutionShaping.createHistogram("t",400);
  histo_shaped->SetName("histo_shaped");


  TCanvas c1("c_histpdf");
  frame1->Draw();
  c1.SaveAs("histpdf.png");

  c1.Write();
  outFile->Write();
  histo_shaped->Write();
  outFile->Close();

  return 0;

}
