#define MCTree_cxx
#include "MCTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

float MCTree::getRatioError( float num, float denom, float numErr, float denomErr ) {

  return sqrt( numErr*numErr/(denom*denom) + denomErr*denomErr*num*num/(denom*denom*denom*denom) );

}



void MCTree::addHisto(TString name, int nBins, float XLow, float XUp, TString XLabel){

  TH1F* histo = new TH1F(name,name,nBins,XLow,XUp);
  histo->GetXaxis()->SetTitle(XLabel);
  histos_[name]=histo;
}

void MCTree::drawHistos(){


  TCanvas c1;
   for(std::map<TString,TH1F*>::const_iterator it = histos_.begin();it!=histos_.end(); it++){
     std::cout<<outDir_+it->first+".png"<<std::endl;
     it->second->Draw();

     c1.SaveAs(outDir_+it->first+".png");
     c1.SaveAs(outDir_+it->first+".pdf");
     it->second->Write();
     c1.Clear();
   }



}

void MCTree::createHistos(){
  addHisto("Energy_singleChannel",10000,0,250,"Energy [GeV]");
  addHisto("Energy_3x3",10000,0,250,"Energy [GeV]");

}

std::pair<float,float> MCTree::fitCB(TH1F* histo){
  //    histo->Rebin(2);
  double peakpos = histo->GetBinCenter(histo->GetMaximumBin());
  double sigma = histo->GetRMS();

  double fitmin;
  double fitmax;


  fitmin = peakpos-4*sigma;
  fitmax = peakpos+4*sigma;


  RooRealVar x("x","deltaT", fitmin, fitmax);
  RooDataHist data("data","dataset with x",x,RooFit::Import(*histo) );

  RooRealVar meanr("meanr","Mean",peakpos-sigma,peakpos-3*sigma, peakpos+3*sigma);
  RooRealVar width("width","#sigma",sigma , 0, 5.*sigma);
  RooRealVar A("A","Dist",1.5, 0.0, 7.0);
  RooRealVar N("N","Deg",5, 0.0, 10);

  RooCBShape fit_fct("fit_fct","fit_fct",x,meanr,width,A,N);
  fit_fct.fitTo(data);



  RooPlot* frame;
  frame = x.frame("Title");
  frame->SetXTitle("Charge Integrated");
  std::string ytitle = Form("Events");
  frame->SetYTitle(ytitle.c_str());

  data.plotOn(frame);  //this will show histogram data points on canvas 
  fit_fct.plotOn(frame);//this will show fit overlay on canvas             

  double rms,rmsErr;

  rms = width.getVal();
  rmsErr = width.getError();
  TCanvas* cans = new TCanvas();
  cans->cd();
  frame->Draw();
  TLegend* lego = new TLegend(0.55, 0.8, 0.8, 0.9);
  lego->SetTextSize(0.031);
  lego->AddEntry(  (TObject*)0 ,Form("#sigma = %.1f #pm %.1f", rms, rmsErr), "");
  lego->AddEntry(  (TObject*)0 ,Form("mean = %.1f #pm %.1f", meanr.getVal(), meanr.getError()), "");
  float mean = meanr.getVal();
  float error=getRatioError(rms,meanr.getVal(),rmsErr,meanr.getError());
  lego->AddEntry(  (TObject*)0 ,Form("res = (%.1f  #pm %.1f)%% ",rms/mean*100 ,error*100), "");

  lego->SetFillColor(0);
  lego->Draw("same");

  TString name = outDir_+"reso_"+TString(histo->GetName())+"_"+energy_+"_"+setup_;
  
  cans->SaveAs(name+".png");
  cans->SaveAs(name+".pdf");

  return std::make_pair(rms/mean*100.,error*100);
}

float addPhotostatistics(float energy){
  //we measured 0.4 p.e. per MeV with PMTs. Now we have APDs. In simulation LY =1000 gamma/MeV 
  // quantum efficiency pmt qe=8%,  quantum efficiency apd (qe_apd)=0.70
  // #pe/gamma=(0.4/LY)*(qe_apd/qe)=0.0035 pe/gamma 
  //so for energy E deposited in the crystals we have a contribution of:
  // E* 1000 gamma/MeV*0.0035 pe/gamma= N pe with a resolution (in %) of
  // width_ps = 1/sqrt(N)

  return 1./sqrt(energy*1000*0.0035);

}

std::pair<float,float> MCTree::fitGausROOT(TH1F* histo)
{
  


  std::cout<<histo->GetName()<<std::endl;

  double peakpos = histo->GetBinCenter(histo->GetMaximumBin());
   double sigma = histo->GetRMS();

   double fitmin;
  double fitmax;


  fitmin = peakpos-3*sigma;
  fitmax = peakpos+3*sigma;



  TF1 f1("f1","gaus",fitmin,fitmax);
  f1.SetParameter(0,peakpos);
  f1.SetParameter(1,sigma);
  f1.SetLineColor(kBlue);
  f1.SetLineWidth(2);
  histo->GetXaxis()->SetRangeUser(fitmin,fitmax);
   histo->Fit("f1","","",fitmin,fitmax);
  histo->SetStats(kFALSE);
  //  RooPlot* frame;                            
  double rms,rmsErr;
  rms = f1.GetParameter(2);
  //  std::cout<<"rms:"<<rms<<" "<<sigma<<std::endl;                
  rms = sqrt(f1.GetParameter(2)*f1.GetParameter(2));
  rmsErr = f1.GetParError(2);
  TCanvas* cans = new TCanvas();
  cans->cd();
  //  frame->Draw();
  TLegend* lego = new TLegend(0.55, 0.8, 0.8, 0.9);
  lego->SetTextSize(0.031);
  lego->AddEntry(  (TObject*)0 ,Form("#sigma = %.2f #pm %.2f", rms, rmsErr), "");
  lego->AddEntry(  (TObject*)0 ,Form("mean = %.2f #pm %.2f", f1.GetParameter(1),f1.GetParError(1)), "");
  float mean = f1.GetParameter(1);
  float error=getRatioError(rms,f1.GetParameter(1),rmsErr,f1.GetParError(1));
  lego->AddEntry(  (TObject*)0 ,Form("res = (%.2f  #pm %.2f)%% ",rms/mean*100 ,error*100), "");

  lego->SetFillColor(0);

  //add photostatistic contribution
  float poissonError=addPhotostatistics(mean*1000);

  float   reso = sqrt(rms*rms/(mean*mean)+poissonError*poissonError)*100;
  float   errorTotal = reso*0.05;//5% error on these estimations

  histo->Draw("EP");
  lego->Draw("same");

  TString name = outDir_+"reso_"+TString(histo->GetName())+"_"+energy_+"_"+setup_;

  cans->SaveAs(name+".png");
  cans->SaveAs(name+".pdf");

  //  return std::make_pair(rms/mean*100.,error*100);
  return std::make_pair(reso,errorTotal);

}


void MCTree::Loop(std::string setup, std::string energy)
{
//   In a ROOT session, you can do:
//      Root > .L MCTree.C
//      Root > MCTree t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   energy_=energy;
   setup_=setup;


   outDir_="outFiles/plots/"+energy+"/"+setup+"/";
   system(Form("mkdir -p %s", outDir_.c_str()));
   TString fileName = "outFiles/resolution_"+setup+"_"+energy;
   fileName+=".root";

   TFile* outFile=TFile::Open(fileName,"recreate");

   TVectorD resolution(2);
   TVectorD resolutionErr(2);

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      histos_["Energy_singleChannel"]->Fill(Eact_CentralXtal/1000.);//we want it in GeV
      histos_["Energy_3x3"]->Fill((Eact_CentralXtal+Eact_3x3)/1000.);//we want it in GeV
   }

   //   std::pair<float,float> reso_xtal=  fitCB(histos_["Energy_singleChannel"]);
   std::pair<float,float> reso_xtal=  fitGausROOT(histos_["Energy_singleChannel"]);
   

   resolution [0] = reso_xtal.first;
   resolutionErr [0] = reso_xtal.second;

   std::pair<float,float> reso_matrix  =  fitGausROOT(histos_["Energy_3x3"]);
   
   
   resolution [1] = reso_matrix.first;
   resolutionErr [1] = reso_matrix.second;
   
   resolution.Write("resolution");
   resolutionErr.Write("resolutionErr");

}
