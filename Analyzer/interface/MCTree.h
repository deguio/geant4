//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Sep 29 17:08:22 2016 by ROOT version 5.34/18
// from TTree tree/tree
// found on file: ../EEShashlikSimulation/H4OpticalSmall_Ideal2016/cmake/runEEShashlik.root
//////////////////////////////////////////////////////////

#ifndef MCTree_h
#define MCTree_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

#include "TF1.h"
#include "TGraphErrors.h"
#include "TString.h"
#include "TProfile.h"
#include "TRandom3.h"

#include "RooDataHist.h"
#include "RooRealVar.h"
#include "RooDataSet.h"
#include "RooGaussian.h"
#include "RooLandau.h"
#include "RooFFTConvPdf.h"
#include "RooPlot.h"
#include "RooCBShape.h"
#include "RooCruijff.h"
#include "RooGenericPdf.h"
#include "RooTFnBinding.h"
#include "RooTFnPdfBinding.h"
#include  "RooNumConvPdf.h"
#include "TLegend.h"

#include <TVectorD.h>

// Header file for the classes stored in the TTree if any.
#include <vector>
#include <string>
#include <map>
#include <algorithm>



// Header file for the classes stored in the TTree if any.

// Fixed size dimensions of array or collections stored in the TTree if any.

class MCTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   Int_t           Event;
   Float_t         Eabs;
   Float_t         Eact_3x3;
   Float_t         Eact_1x3;
   Float_t         Eabs_CentralXtal;
   Float_t         Eact_CentralXtal;
   Int_t           nLayers;
   Float_t         xPosition;
   Float_t         yPosition;

   std::string outDir_;
   std::map<TString,TH1F*> histos_;
   std::string energy_;
   std::string setup_;

   // List of branches
   TBranch        *b_Event;   //!
   TBranch        *b_Eabs;   //!
   TBranch        *b_Eact_3x3;   //!
   TBranch        *b_Eact_1x3;   //!
   TBranch        *b_Eabs_CentralXtal;   //!
   TBranch        *b_Eact_CentralXtal;   //!
   TBranch        *b_nLayers;   //!
   TBranch        *b_xPosition;   //!
   TBranch        *b_yPosition;   //!

   MCTree(TTree *tree=0);
   virtual ~MCTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(std::string setup, std::string energy);
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);

   float getRatioError( float num, float denom, float numErr, float denomErr );
   void addHisto(TString name, int nBins, float XLow, float XUp, TString XLabel);
   void drawHistos();
   void createHistos();
   std::pair<float,float> fitCB(TH1F* histo);
   std::pair<float,float> fitGausROOT(TH1F* histo);


};

#endif

#ifdef MCTree_cxx
MCTree::MCTree(TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("../EEShashlikSimulation/H4OpticalSmall_Ideal2016/cmake/runEEShashlik.root");
      if (!f || !f->IsOpen()) {
         f = new TFile("../EEShashlikSimulation/H4OpticalSmall_Ideal2016/cmake/runEEShashlik.root");
      }
      f->GetObject("tree",tree);

   }
   Init(tree);
}

MCTree::~MCTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t MCTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t MCTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void MCTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("Event", &Event, &b_Event);
   fChain->SetBranchAddress("Eabs", &Eabs, &b_Eabs);
   fChain->SetBranchAddress("Eact_3x3", &Eact_3x3, &b_Eact_3x3);
   fChain->SetBranchAddress("Eact_1x3", &Eact_1x3, &b_Eact_1x3);
   fChain->SetBranchAddress("Eabs_CentralXtal", &Eabs_CentralXtal, &b_Eabs_CentralXtal);
   fChain->SetBranchAddress("Eact_CentralXtal", &Eact_CentralXtal, &b_Eact_CentralXtal);
   fChain->SetBranchAddress("nLayers", &nLayers, &b_nLayers);
   fChain->SetBranchAddress("xPosition", &xPosition, &b_xPosition);
   fChain->SetBranchAddress("yPosition", &yPosition, &b_yPosition);
   Notify();
}

Bool_t MCTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void MCTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t MCTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef MCTree_cxx
