#define Analyzer_cxx
#include "Analyzer.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

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

  TString fileName = "outFiles/plotterTiming_"+setup+"_"+energy+".root";

  TFile* outFile = TFile::Open(fileName,"recreate");

  int nPhotTiming=100;

  TH1F* timeArrival_avg = new TH1F("timeArrival_avg","timeArrival_avg",100,0,100);
  TH1F* ZTimeArrival_avg = new TH1F("ZTimeArrival_avg","ZTimeArrival_avg",420,0,420);
  TH1F* processTimeArrival_avg = new TH1F("processTimeArrival_avg","processTimeArrival_avg",3,1,4);

  TH1F* timeArrival_avg_wls = new TH1F("timeArrival_avg_wls","timeArrival_avg_wls",100,0,100);
  TH1F* ZTimeArrival_avg_wls = new TH1F("ZTimeArrival_avg_wls","ZTimeArrival_avg_wls",420,0,420);


  TH1F* timeArrival_avg_scint = new TH1F("timeArrival_avg_scint","timeArrival_avg_scint",100,0,100);
  TH1F* ZTimeArrival_avg_scint = new TH1F("ZTimeArrival_avg_scint","ZTimeArrival_avg_scint",420,0,420);

  TH1F* timeArrival_avg_cher = new TH1F("timeArrival_avg_cher","timeArrival_avg_cher",100,0,100);
  TH1F* ZTimeArrival_avg_cher = new TH1F("ZTimeArrival_avg_cher","ZTimeArrival_avg_cher",420,0,420);



  TH1F* timeArrival = new TH1F("timeArrival","timeArrival",100,0,100);
  TH1F* ZTimeArrival = new TH1F("ZTimeArrival","ZTimeArrival",420,0,420);
  TH1F* processTimeArrival = new TH1F("processTimeArrival","processTimeArrival",3,1,4);

  TH1F* timeArrival_scint = new TH1F("timeArrival_scint","timeArrival_scint",100,0,100);
  TH1F* ZTimeArrival_scint = new TH1F("ZTimeArrival_scint","ZTimeArrival_scint",420,0,420);
  TH1F* processTimeArrival_scint = new TH1F("processTimeArrival_scint","processTimeArrival_scint",3,1,4);


  TH1F* timeArrival_cher = new TH1F("timeArrival_cher","timeArrival_cher",100,0,100);
  TH1F* ZTimeArrival_cher = new TH1F("ZTimeArrival_cher","ZTimeArrival_cher",420,0,420);
  TH1F* processTimeArrival_cher = new TH1F("processTimeArrival_cher","processTimeArrival_cher",3,1,4);


  TH1F* timeArrival_wls = new TH1F("timeArrival_wls","timeArrival_wls",100,0,100);
  TH1F* ZTimeArrival_wls = new TH1F("ZTimeArrival_wls","ZTimeArrival_wls",420,0,420);
  TH1F* processTimeArrival_wls = new TH1F("processTimeArrival_wls","processTimeArrival_wls",3,1,4);



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
	timeArrival->Fill(Time_deposit->at(j));
	ZTimeArrival->Fill(Z_deposit->at(j));
	processTimeArrival->Fill(Process_deposit->at(j));
      }

      for(int i=0;i<nPhotTiming;++i){
	if(i<Time_deposit->size()){
	  averageTiming+=Time_deposit->at(i);
	  processTimeArrival_avg->Fill(Process_deposit->at(i));
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
      timeArrival_avg->Fill(averageTiming/nPhotTiming);
      ZTimeArrival_avg->Fill(averageZ/nPhotTiming);

      timeArrival_avg_wls->Fill(averageTiming_wls/nPhotTiming_wls);
      ZTimeArrival_avg_wls->Fill(averageZ_wls/nPhotTiming_wls);

      timeArrival_avg_scint->Fill(averageTiming_scint/nPhotTiming_scint);
      ZTimeArrival_avg_scint->Fill(averageZ_scint/nPhotTiming_scint);

      timeArrival_avg_cher->Fill(averageTiming_cher/nPhotTiming_cher);
      ZTimeArrival_avg_cher->Fill(averageZ_cher/nPhotTiming_cher);


   }

   timeArrival_avg->Write();
   processTimeArrival_avg->Write();
   ZTimeArrival_avg->Write();
   timeArrival->Write();
   ZTimeArrival->Write();
   outFile->Write();
   outFile->Close();

}
