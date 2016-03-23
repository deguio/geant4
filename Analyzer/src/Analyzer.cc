#define Analyzer_cxx
#include "Analyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>


void Analyzer::addHisto(TString name, int nBins, float XLow, float XUp, TString XLabel){

  TH1F* histo = new TH1F(name,name,nBins,XLow,XUp);
  histo->GetXaxis()->SetTitle(XLabel);
  histos_[name]=histo;
}

void Analyzer::drawHistos(){


  TCanvas c1;
   for(std::map<TString,TH1F*>::const_iterator it = histos_.begin();it!=histos_.end(); it++){
     it->second->Draw();
     std::cout<<outDir_+it->first+".png"<<std::endl;
     c1.SaveAs(outDir_+it->first+".png");
     c1.SaveAs(outDir_+it->first+".pdf");
     it->second->Write();
     c1.Clear();
   }


}

void Analyzer::createHistos(){
  if(setup_=="SingleFibre")  addHisto("timeArrival_avg",500,5,10,"time [ns]");
  else addHisto("timeArrival_avg",500,10,20,"time [ns]");
  addHisto("ZTimeArrival_avg",420,0,420,"Z [mm]");
  addHisto("processTimeArrival_avg",3,0.5,3.5,"process");
 
  addHisto("nPhotons",200,0,1000,"WLS Photons");

  if(setup_=="SingleFibre")   addHisto("timeArrival_avg_wls",500,5,10,"time [ns]");
  else   addHisto("timeArrival_avg_wls",500,10,20,"time [ns]");
  addHisto("ZTimeArrival_avg_wls",420,0,420,"Z [mm]");
  
  if(setup_=="SingleFibre")   addHisto("timeArrival_avg_scint",500,5,10,"time [ns]");
  else   addHisto("timeArrival_avg_scint",500,10,20,"time [ns]");
  addHisto("ZTimeArrival_avg_scint",420,0,420,"Z [mm]");
  
  if(setup_=="SingleFibre")   addHisto("timeArrival_avg_cher",500,5,10,"time [ns]");
  else   addHisto("timeArrival_avg_cher",500,10,20,"time [ns]");
  addHisto("ZTimeArrival_avg_cher",420,0,420,"Z [mm]");

  if(setup_=="SingleFibre")   addHisto("timeArrival",500,5,10,"time [ns]");
  else   addHisto("timeArrival",500,10,200,"time [ns]");
  addHisto("ZTimeArrival",420,0,420,"Z [mm]");
  addHisto("processTimeArrival",3,0.5,3.5,"process");

  if(setup_=="SingleFibre")   addHisto("timeArrival_scint",500,5,10,"time [ns]");
  else   addHisto("timeArrival_scint",500,10,20,"time [ns]");
  addHisto("ZTimeArrival_scint",420,0,420,"Z [mm]");

  if(setup_=="SingleFibre")   addHisto("timeArrival_cher",500,5,10,"time [ns]");
  else   addHisto("timeArrival_cher",500,10,20,"time [ns]");
  addHisto("ZTimeArrival_cher",420,0,420,"Z [mm]");

  if(setup_=="SingleFibre")   addHisto("timeArrival_wls",500,5,10,"time [ns]");
  else   addHisto("timeArrival_wls",500,10,20,"time [ns]");
  addHisto("ZTimeArrival_wls",420,0,420,"Z [mm]");

 addHisto("waveform_total",200,0,100,"time [ns]");
 addHisto("waveform_scint",200,0,100,"time [ns]");
 addHisto("waveform_cher",200,0,100,"time [ns]");
 addHisto("waveform_wls",200,0,100,"time [ns]");

 addHisto("time_at_max",100,0,50,"time [ns]");

}


void Analyzer::Loop(std::string setup, std::string energy)
{
//   In a ROOT session, you can do:
//      Root > .L Analyzer.C
//      Root > Analyzer t
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

   Long64_t nentries = fChain->GetEntries();
   //   nentries = 200;
   energy_=energy;
   setup_=setup;

   outDir_="outFiles/plots/"+energy+"/"+setup+"/";
   system(Form("mkdir -p %s", outDir_.Data()));
   TString fileName = "outFiles/plotterTiming_"+setup+"_"+energy+".root";

  TFile* outFile = TFile::Open(fileName,"recreate");

  createHistos();

  int nPhotTiming=200;
  LoadTree(0);
  fChain->GetEntry(0);
  //  nPhotTiming=(int)(Time_deposit->size()*0.01);
  std::cout<<"averaging time on "<<nPhotTiming<<" photons"<<std::endl;


   Long64_t nbytes = 0, nb = 0;
   TH1F* wave = new TH1F("wave","wave",200,0,100);
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      if(jentry%100 == 0)std::cout<<"Entry:"<<jentry<<"/"<<nentries<<std::endl;
      float averageTiming = 0;
      float averageZ = 0;
      float averageTiming_wls = 0;
      float averageZ_wls = 0;
      float averageTiming_scint = 0;
      float averageZ_scint = 0;
      float averageTiming_cher = 0;
      float averageZ_cher = 0;

      int nPhotTiming_wls=0;
      int nPhotTiming_scint=0;
      int nPhotTiming_cher=0;



      histos_["nPhotons"]->Fill(Fibre_0);

      for(int j=0;j<Time_deposit->size();++j){
	histos_["timeArrival"]->Fill(Time_deposit->at(j));
	histos_["ZTimeArrival"]->Fill(Z_deposit->at(j));
	histos_["processTimeArrival"]->Fill(Process_deposit->at(j));
	if(jentry<10)	histos_["waveform_total"]->Fill(Time_deposit->at(j));
	wave->Fill(Time_deposit->at(j));
	if(Process_deposit->at(j)==1){
	histos_["timeArrival_wls"]->Fill(Time_deposit->at(j));
	histos_["ZTimeArrival_wls"]->Fill(Z_deposit->at(j));
	if(jentry<10)	histos_["waveform_wls"]->Fill(Time_deposit->at(j));
	}else if(Process_deposit->at(j)==2){
	histos_["timeArrival_scint"]->Fill(Time_deposit->at(j));
	histos_["ZTimeArrival_scint"]->Fill(Z_deposit->at(j));
	if(jentry<10)	histos_["waveform_scint"]->Fill(Time_deposit->at(j));
	}else  if(Process_deposit->at(j)==3){
	histos_["timeArrival_cher"]->Fill(Time_deposit->at(j));
	histos_["ZTimeArrival_cher"]->Fill(Z_deposit->at(j));
	if(jentry<10)	histos_["waveform_cher"]->Fill(Time_deposit->at(j));
	}
      }


      histos_["time_at_max"]->Fill(wave->GetBinCenter(wave->GetMaximumBin()));
	///	std::cout<<wave->GetMaximumBin()<<std::endl;
	if(jentry==200){
	  TCanvas dummy;
	  TF1 f("f","pol2",0,50);
	  wave->Fit("f","R","",wave->GetBinCenter(wave->GetMaximumBin())-2*wave->GetBinWidth(0),wave->GetBinCenter(wave->GetMaximumBin())+2*wave->GetBinWidth(0));
	  wave->Draw();
	  f.SetLineColor(kRed);
	  f.Draw("same");
	  dummy.SaveAs("dummy.png");
	}
	  wave->Reset();	


      if(setup_=="SingleFibre")         nPhotTiming=(int)(Time_deposit->size()*0.01);
      else nPhotTiming=(int)(Time_deposit->size()*0.01);
      //      std::cout<<nPhotTiming<<std::endl;

      //      for (auto i: Analyzer::sort_indexes( *Time_deposit)) {
	//	std::cout << Time_deposit->at(i) << std::endl;
      //      } 


      int nPhot=0;      
      for (auto i: Analyzer::sort_indexes( *Time_deposit)) {
	nPhot++;
	if(nPhot>nPhotTiming)break;
	//	std::cout<<nPhot<<" "<< Time_deposit->at(i) << std::endl;
	if(i<Time_deposit->size()){
	  averageTiming+=Time_deposit->at(i);
	  histos_["processTimeArrival_avg"]->Fill(Process_deposit->at(i));
	  averageZ+=Z_deposit->at(i);
	  if(Process_deposit->at(i)==1){
	    nPhotTiming_wls++;
	    averageTiming_wls+=Time_deposit->at(i);
	    averageZ_wls+=Z_deposit->at(i);
	  }else if(Process_deposit->at(i)==2){
	    nPhotTiming_scint++;
	    averageTiming_scint+=Time_deposit->at(i);
	    averageZ_scint+=Z_deposit->at(i);
	  }else if(Process_deposit->at(i)==3){
	    nPhotTiming_cher++;
	    averageTiming_cher+=Time_deposit->at(i);
	    averageZ_cher+=Z_deposit->at(i);
	  }
	}
      }
      histos_["timeArrival_avg"]->Fill(averageTiming/nPhotTiming);
      histos_["ZTimeArrival_avg"]->Fill(averageZ/nPhotTiming);
      
      histos_["timeArrival_avg_wls"]->Fill(averageTiming_wls/nPhotTiming_wls);
      histos_["ZTimeArrival_avg_wls"]->Fill(averageZ_wls/nPhotTiming_wls);
      
      histos_["timeArrival_avg_scint"]->Fill(averageTiming_scint/nPhotTiming_scint);
      histos_["ZTimeArrival_avg_scint"]->Fill(averageZ_scint/nPhotTiming_scint);
      
      histos_["timeArrival_avg_cher"]->Fill(averageTiming_cher/nPhotTiming_cher);
      histos_["ZTimeArrival_avg_cher"]->Fill(averageZ_cher/nPhotTiming_cher);
   }


   fitHisto(histos_["timeArrival_avg"]);
   drawHistos();

   TCanvas wave_canvas;
   histos_["waveform_total"]->Draw();
   histos_["waveform_total"]->Print();
   histos_["waveform_scint"]->SetLineColor(kRed);
   histos_["waveform_scint"]->Draw("same");
   histos_["waveform_cher"]->SetLineColor(kBlue);
   histos_["waveform_wls"]->SetLineColor(kViolet);
   histos_["waveform_cher"]->Draw("same");
   histos_["waveform_wls"]->Draw("same");
   wave_canvas.SaveAs(outDir_+"waveform.png");
   wave_canvas.SaveAs(outDir_+"waveform.pdf");

   outFile->Write();
   outFile->Close();

}


void Analyzer::fitHisto(TH1F* histo){
//-----------------fit with cruijff ------------------------
  double peakpos = histo->GetBinCenter(histo->GetMaximumBin());
  double sigma = histo->GetRMS();

  double fitmin;
  double fitmax;
  
  
  fitmin = peakpos-4*sigma;
  fitmax = peakpos+4*sigma;
  
  RooRealVar x("x","deltaT", fitmin, fitmax);
  RooDataHist data("data","dataset with x",x,RooFit::Import(*histo) );

  RooRealVar meanr("meanr","Mean",peakpos,peakpos-3*sigma, peakpos+3*sigma);
  RooRealVar widthL("widthL","#sigmaL",sigma , 0, 5*sigma);
  RooRealVar widthR("widthR","#sigmaR",sigma , 0, 5*sigma);
  RooRealVar alphaL("alphaL","#alpha",5.08615e-02 , 0., 1.);
  RooRealVar alphaR("alphaR","#alpha",5.08615e-02, 0., 1.);
  int ndf;

  RooPlot* frame;

  RooCruijff fit_fct("fit_fct","fit_fct",x,meanr,widthL,widthR,alphaL,alphaR); ndf = 5;
  fit_fct.fitTo(data);
  
  frame = x.frame("Title");
  frame->SetXTitle("time [ns]");
  std::string ytitle = Form("Events");
  frame->SetYTitle(ytitle.c_str());
  
  data.plotOn(frame);  //this will show histogram data points on canvas 
  fit_fct.plotOn(frame);//this will show fit overlay on canvas  

  double rms,rmsErr;
  rms = (widthL.getVal()+widthR.getVal())/2;
  rmsErr = 0.5*sqrt(widthL.getError()*widthL.getError()+widthR.getError()*widthR.getError());

  TCanvas* cans = new TCanvas();
  cans->cd();
  frame->Draw();
  TLegend* lego = new TLegend(0.57, 0.75, 0.89, 0.9); 
  lego->SetTextAlign(32);
  lego->SetTextSize(0.036);
  lego->AddEntry(  (TObject*)0 ,Form("#mu = %.0f #pm %.0f ps", meanr.getVal()*1.e3, meanr.getError()*1.e3), "");
  lego->AddEntry(  (TObject*)0 ,Form("#sigma = %.0f #pm %.0f ps", rms*1.e3, rmsErr*1.e3), "");

  lego->SetFillColor(0);
  lego->Draw("same");
  
  TPaveText* pave = DrawTools::getLabelTop_expOnXaxis(energy_+" Electron Beam");
  pave->Draw("same");

  cans->SaveAs(outDir_+histo->GetName()+"_cruijff_fit.png");
  cans->SaveAs(outDir_+histo->GetName()+"_cruijff_fit.pdf");



}
