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
  addHisto("timeArrival_avg",150,0,75,"time [ns]");
  addHisto("ZTimeArrival_avg",420,0,420,"Z [mm]");
  addHisto("processTimeArrival_avg",3,0.5,3.5,"process");
 
  addHisto("timeArrival_avg_wls",150,0,75,"time [ns]");
  addHisto("ZTimeArrival_avg_wls",420,0,420,"Z [mm]");
  
  addHisto("timeArrival_avg_scint",150,0,75,"time [ns]");
  addHisto("ZTimeArrival_avg_scint",420,0,420,"Z [mm]");
  
  addHisto("timeArrival_avg_cher",150,0,75,"time [ns]");
  addHisto("ZTimeArrival_avg_cher",420,0,420,"Z [mm]");

  addHisto("timeArrival",150,0,75,"time [ns]");
  addHisto("ZTimeArrival",420,0,420,"Z [mm]");
  addHisto("processTimeArrival",3,0.5,3.5,"process");

  addHisto("timeArrival_scint",150,0,75,"time [ns]");
  addHisto("ZTimeArrival_scint",420,0,420,"Z [mm]");

  addHisto("timeArrival_cher",150,0,75,"time [ns]");
  addHisto("ZTimeArrival_cher",420,0,420,"Z [mm]");

  addHisto("timeArrival_wls",150,0,75,"time [ns]");
  addHisto("ZTimeArrival_wls",420,0,420,"Z [mm]");

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
   //   nentries = 10;

   outDir_="outFiles/plots/"+setup+"_"+energy;
   system(Form("mkdir -p %s", outDir_.Data()));
   TString fileName = "outFiles/plotterTiming_"+setup+"_"+energy+".root";

  TFile* outFile = TFile::Open(fileName,"recreate");

  createHistos();

  int nPhotTiming=100;


   Long64_t nbytes = 0, nb = 0;
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

      for(int j=0;j<Time_deposit->size();++j){
	histos_["timeArrival"]->Fill(Time_deposit->at(j));
	histos_["ZTimeArrival"]->Fill(Z_deposit->at(j));
	histos_["processTimeArrival"]->Fill(Process_deposit->at(j));
      }

      for(int i=0;i<nPhotTiming;++i){
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


   drawHistos();

   outFile->Write();
   outFile->Close();

}
