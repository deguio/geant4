#include "MCTree.h"
#include "DrawTools.h"
#include "TGaxis.h"


int main( int argc, char* argv[] ) {

  DrawTools::setStyle();

  gStyle->SetLabelSize(0.040, "XYZ");
  
  std::string setup = "Ideal2016";
  std::string energy = "100GeV";

  if( argc>1 ) {
    std::string setup_str(argv[1]);
    setup = setup_str;
    if( argc>2 ) {
      std::string energy_str(argv[2]);
      energy=energy_str;
    }
  } else {
    std::cout << "Usage:" << std::endl;
    std::cout << "./checkEnergyResolution ([setup]) ([energy])" << std::endl;
    exit(12345);
  }

  std::cout<<"plotting for setup:"<<setup<<" energy:"<<energy<<std::endl;

  TString fileName = "files/totalOut_"+setup+"_smeared_"+energy+".root";

  TFile* inFile = TFile::Open(fileName);
  TTree* tree = (TTree*) inFile->Get("tree");
  MCTree t(tree);
  t.createHistos();
  t.Loop(setup, energy);
  t.drawHistos();

  return 0;


}
