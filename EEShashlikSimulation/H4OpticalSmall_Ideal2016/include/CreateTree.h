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

  float  Eabs;
  float  Eact;
  int    nLayers;

  std::vector<float> Eact_xtal;
  std::vector<float> Eabs_xtal;

  float  xPosition;
  float  yPosition;
  

    
};
