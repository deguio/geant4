#include "CreateTree.h"



CreateTree* CreateTree::fInstance = NULL;



CreateTree::CreateTree(TString name)
{
  if( fInstance )
    {
      return;
    }

  this -> fInstance = this;
  this -> fname     = name;
  this -> ftree     = new TTree(name,name);

  this->GetTree()->Branch("Event",&this->Event,"Event/I");

  this->GetTree()->Branch("Eabs",&this->Eabs,"Eabs/F");
  this->GetTree()->Branch("Eact",&this->Eact,"Eact/F");

  this->GetTree()->Branch("nLayers",&this->nLayers,"nLayers/I");

  this->GetTree()->Branch("xPosition",&this->xPosition,"xPosition/F");
  this->GetTree()->Branch("yPosition",&this->yPosition,"yPosition/F");




}

bool CreateTree::Write()
{
  TString filename = this->GetName();
  filename+=".root";
  TFile* file = new TFile(filename,"RECREATE");
  this->GetTree()->Write();
  file->Write();
  file->Close();
  return true;
}


void CreateTree::Clear()
{
  Event = 0;

  Eabs=0;
  Eact=0;
  nLayers=0;
  
  
  xPosition=0;
  yPosition=0;
  

}
