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
// $Id: EEShashActionInitialization.cc 68058 2013-03-13 14:47:43Z gcosmo $
//
/// \file EEShashActionInitialization.cc
/// \brief Implementation of the EEShashActionInitialization class

#include "EEShashActionInitialization.hh"
#include "EEShashPrimaryGeneratorAction.hh"
#include "EEShashRunAction.hh"
#include "EEShashEventAction.hh"
#include "TrackingAction.hh"
#include "SteppingAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashActionInitialization::EEShashActionInitialization()
 : G4VUserActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EEShashActionInitialization::~EEShashActionInitialization()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashActionInitialization::BuildForMaster() const
{
  extern int nLayers;
  extern int nBGOs;
  extern int nFibres;
  std::cout << "Printing from EEShashActionInitialization.cc::BuildForMaster() :" << G4endl;
  std::cout << "  Using nLayers = " << nLayers << G4endl;
  std::cout << "  Using nBGOs   = " << nBGOs << G4endl;
  std::cout << "  Using nFibres = " << nFibres << G4endl;

  SetUserAction(new EEShashRunAction);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EEShashActionInitialization::Build() const
{
  extern int nLayers;
  extern int nBGOs;
  extern int nFibres;
  std::cout << "Printing from EEShashActionInitialization.cc::Build() :" << G4endl;
  std::cout << "  Using nLayers = " << nLayers << G4endl;
  std::cout << "  Using nBGOs   = " << nBGOs << G4endl;
  std::cout << "  Using nFibres = " << nFibres << G4endl;

  SetUserAction(new EEShashPrimaryGeneratorAction);
  SetUserAction(new EEShashRunAction);
  SetUserAction(new EEShashEventAction);
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
