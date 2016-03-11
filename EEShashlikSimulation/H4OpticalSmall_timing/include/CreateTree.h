#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TString.h"

class CreateTree
{
 private:
  
  TTree*  ftree;
  TString fname;
  
 public:
  
  
  CreateTree(TString name);
  TTree*             GetTree() const { return ftree; };
  TString            GetName() const { return fname; };
  int                Fill() { return this->GetTree()->Fill(); };
  bool               Write();
  void               Clear();
  static CreateTree* Instance() { return fInstance; };
  static CreateTree* fInstance;
  int Event;
  int Prova;
  std::vector<float> Time_deposit;
  std::vector<float> opPhoton_time;
  std::vector<int> opPhoton_process;
  
};
