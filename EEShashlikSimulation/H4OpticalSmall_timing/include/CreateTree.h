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
  std::vector<float> Time_deposit;
  std::vector<float> Process_deposit;//0 is WLS
  std::vector<float> opPhoton_time;
  std::vector<int> opPhoton_process;
  float  Eabs;
  float  Eact;
  float  EfibrCore;
  float  EfibrClad;
  int    nLayers;

  int   Fibre_0;
  int   Fibre_1;
  int   Fibre_2;
  int   Fibre_3;

  float  xPosition;
  float  yPosition;
  
  float  EOpt_0;
  float  EOpt_1;
  float  EOpt_2;
  float  EOpt_3;  
    
};
