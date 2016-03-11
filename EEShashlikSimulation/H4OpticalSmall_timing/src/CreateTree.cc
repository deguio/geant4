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
  this->GetTree()->Branch("Prova",&this->Prova,"Prova/I");
  this->GetTree()->Branch("Time_deposit",&this->Time_deposit);
  this->GetTree()->Branch("opPhoton_time",&opPhoton_time);    

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
  Time_deposit.clear();
  opPhoton_time.clear();

}
