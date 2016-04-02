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
  this->GetTree()->Branch("Time_deposit",&this->Time_deposit);
  this->GetTree()->Branch("Process_deposit",&this->Process_deposit);
  this->GetTree()->Branch("Z_deposit",&this->Z_deposit);
  this->GetTree()->Branch("Theta_deposit",&this->Theta_deposit);
  this->GetTree()->Branch("opPhoton_time",&this->opPhoton_time);    
  this->GetTree()->Branch("opPhoton_process",&this->opPhoton_process);    
  this->GetTree()->Branch("Eabs",&this->Eabs,"Eabs/F");
  this->GetTree()->Branch("Eact",&this->Eact,"Eact/F");
  this->GetTree()->Branch("NPhot_Act",&this->NPhot_Act,"NPhot_Act/I");
  this->GetTree()->Branch("EfibrCore",&this->EfibrCore,"EfibrCore/F");
  this->GetTree()->Branch("EfibrClad",&this->EfibrClad,"EfibrClad/F");
  this->GetTree()->Branch("nLayers",&this->nLayers,"nLayers/I");

  this->GetTree()->Branch("Fibre_start_0",&this->Fibre_start_0,"Fibre_start_0/I");

  this->GetTree()->Branch("Fibre_0",&this->Fibre_0,"Fibre_0/I");
  this->GetTree()->Branch("Fibre_1",&this->Fibre_1,"Fibre_1/I");
  this->GetTree()->Branch("Fibre_2",&this->Fibre_2,"Fibre_2/I");
  this->GetTree()->Branch("Fibre_3",&this->Fibre_3,"Fibre_3/I");

  this->GetTree()->Branch("xPosition",&this->xPosition,"xPosition/F");
  this->GetTree()->Branch("yPosition",&this->yPosition,"yPosition/F");

  this->GetTree()->Branch("EOpt_0",&this->EOpt_0,"EOpt_0/F");
  this->GetTree()->Branch("EOpt_1",&this->EOpt_1,"EOpt_1/F");
  this->GetTree()->Branch("EOpt_2",&this->EOpt_2,"EOpt_2/F");
  this->GetTree()->Branch("EOpt_3",&this->EOpt_3,"EOpt_3/F");


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
  Process_deposit.clear();
  Z_deposit.clear();
  Theta_deposit.clear();
  opPhoton_time.clear();
  opPhoton_process.clear();
  Eabs=0;
  Eact=0;
  NPhot_Act=0;
  EfibrCore=0;
  EfibrClad=0;
  nLayers=0;
  
  Fibre_start_0=0;

  Fibre_0=0;
  Fibre_1=0;
  Fibre_2=0;
  Fibre_3=0;
  
  xPosition=0;
  yPosition=0;
  
  EOpt_0=0;
  EOpt_1=0;
  EOpt_2=0;
  EOpt_3=0;  

}
