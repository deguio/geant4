//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: exampleEEShash.cc 75215 2013-10-29 16:07:06Z gcosmo $
//
/// \file exampleEEShash.cc
/// \brief Main program of the EEShash example


// Number of layers, BGOs and fibres
// Better to set these here to prevent problems with ROOT branch creation and filling
#include <vector>
int nLayers = 12; 
int nBGOs = 0;
int nFibres = 0;

int nPhotonsForTiming=0;
std::vector<float> time_vector;

#include "EEShashDetectorConstruction.hh"
#include "EEShashActionInitialization.hh"
#include "SteppingAction.hh"
#include "TrackingAction.hh"


#include "G4EmStandardPhysics.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include "G4EmUserPhysics.hh"

#include "CreateTree.h"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "FTFP_BERT.hh"

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "TROOT.h"

using namespace CLHEP;


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " exampleEEShash [-m macro ] [-u UIsession] [-t nThreads]" << G4endl;
    G4cerr << "   note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

long int CreateSeed();

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

int main(int argc,char** argv)
{
  // Evaluate arguments
  //
  if ( argc > 7 ) {
    PrintUsage();
    return 1;
  }
  
  G4String macro;
  G4String session;
  G4double rotation = 0.;
  G4double zTras = 0.;
  G4String jobid = "notbatched";
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-m" ) macro = argv[i+1];
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
    else if ( G4String(argv[i]) == "-r" ) rotation = atof(argv[i+1]);
    else if ( G4String(argv[i]) == "-z" ) zTras = atof(argv[i+1]);
    else if ( G4String(argv[i]) == "-b" ) jobid = argv[i+1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
#endif
    else {
      PrintUsage();
      return 1;
    }
  }  
  
  // Choose the Random engine
  //
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
  //
#ifdef G4MULTITHREADED
  G4MTRunManager * runManager = new G4MTRunManager;
  if ( nThreads > 0 ) { 
    runManager->SetNumberOfThreads(nThreads);
  }  
#else
  G4RunManager * runManager = new G4RunManager;
#endif



  // Set mandatory initialization classes                                                                                                                                                                                              
  //                                                                                                                                                                                                                                   
  gROOT->ProcessLine("#include <vector>");//otherwise branch with vector does not work                                   
  std::string filename;

  // If this run is part of a job, store it in a separate ROOT file
  if ((std::getenv("JOB_ID")) && (std::getenv("JOB_OUTDIR"))) {
    G4String JOBID = std::getenv("JOB_ID");
    G4String JOBOUTDIR = std::getenv("JOB_OUTDIR");
    filename = JOBOUTDIR + "out.root";
    }
  else if(std::getenv("JOBNAME") ){//for lxpl
    G4String JOBOUTDIR = std::getenv("JOB_OUTDIR");
    filename = JOBOUTDIR + "/out.root";
    }
  else filename = "runEEShashlik.root";
  // Else, just store as it as the default output file

  std::cout << "Using fileName: " << filename << G4endl;
                                                                                       
  TFile* outfile=new TFile(filename.c_str(),"recreate");
  CreateTree* mytree = new CreateTree("tree");


  // Set mandatory initialization classes
  //


  // Initialize DetectorConstruction
  EEShashDetectorConstruction* detConstruction = new EEShashDetectorConstruction(rotation, zTras);
  runManager->SetUserInitialization(detConstruction);

  // Switch on relevant physics
  G4int switchOnScintillation = 1;
  G4int switchOnCerenkov = 0;
  //  G4int switchOnCerenkov = 1;
  G4int propagateScintillation = 1;
  G4int propagateCerenkov = 0;


  

  std::string physName("");
  G4PhysListFactory factory;
  const std::vector<G4String>& names = factory.AvailablePhysLists();
  for(unsigned n = 0; n != names.size(); n++)
    G4cout << "PhysicsList: " << names[n] << G4endl;

  if( physName == "")
    {
      char* path = getenv("PHYSLIST");
      if( path ) physName = G4String(path);
    }
  if ( physName == "" || factory.IsReferencePhysList(physName))
    {
      physName = "FTFP_BERT";
    }

  G4cout << "Using physics list: " << physName << G4endl;
  G4cout << "" << G4endl;
  G4cout << ">>> Define physics list::begin <<<" << G4endl;
  G4VModularPhysicsList* physics = factory.GetReferencePhysList(physName);

  physics->RegisterPhysics(new G4EmUserPhysics(switchOnScintillation,switchOnCerenkov));

  runManager-> SetUserInitialization(physics);
  G4cout << ">>> Define physics list::end <<<" << G4endl; 
  



  EEShashActionInitialization* actionInitialization
     = new EEShashActionInitialization();
  runManager->SetUserInitialization(actionInitialization);

  
  G4cout << ">>> Define TrackingAction::begin <<<" << G4endl;
  TrackingAction* tracking_action = new TrackingAction;
  runManager->SetUserAction(tracking_action);
  G4cout << ">>> Define TrackingAction::end <<<" << G4endl;

  G4cout << ">>> Define SteppingAction::begin <<<" << G4endl;
  SteppingAction* stepping_action = new SteppingAction(detConstruction,propagateScintillation,propagateCerenkov);
  runManager->SetUserAction(stepping_action);
  G4cout << ">>> Define SteppingAction::end <<<" << G4endl;


  // Initialize G4 kernel
  //
  runManager->Initialize();
  
#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  if ( macro.size() ) {
    // batch mode
    G4String command = "/control/execute ";
    UImanager->ApplyCommand(command+macro);
  }
  else  {  
    // interactive mode : define UI session
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv, session);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute init_vis.mac"); 
#else
    UImanager->ApplyCommand("/control/execute init.mac"); 
#endif
    //  if (ui->IsGUI())
    // UImanager->ApplyCommand("/control/execute gui.mac");
    ui->SessionStart();
    delete ui;
#endif
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted 
  // in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  mytree -> GetTree() -> Write();
  outfile -> Close();


  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
