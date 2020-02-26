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
//
/// \copied from B5RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "Analysis.hh"

#include "time.h"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
 : G4UserRunAction()
{ 
  auto analysisManager = G4AnalysisManager::Instance();
  G4cout << "Using " << analysisManager->GetType() << G4endl;

  // Default settings
  analysisManager->SetNtupleMerging(true);
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetFileName("proton_pol");

  // Creating 1D histograms
  analysisManager // H1-ID = 0
    ->CreateH1("dcin_numhit","dcin : number of hits", 10, 0., 10.);
  analysisManager // H1-ID = 1
    ->CreateH1("dcout_numhit","dcout : number of hits", 10, 0., 10.);
  analysisManager // H1-ID = 2
    ->CreateH1("dcin_direction","dcin : direction", 180, 0., 180.);
  analysisManager // H1-ID = 3
    ->CreateH1("dcout_direction","dcout : direction", 180, 0., 180.);
  analysisManager // H1-ID = 4
    ->CreateH1("analysis_theta","analysis : theta", 180, 0., 180.);
  analysisManager // H1-ID = 5
    ->CreateH1("analysis_phi","analysis : phi", 360, -180., 180.);
  analysisManager // H1-ID = 6
    ->CreateH1("analysis_cosphi","analysis : cos(phi)", 200, -1., 1.);
  analysisManager // H1-ID = 7
    ->CreateH1("analysis_sinphi","analysis : sin(phi)", 200, -1., 1.);
  
  // Creating 2D histograms
  analysisManager  // H2-ID = 0                                              
    ->CreateH2("dcin_hitposition_xy","dcin : hit position on x-y plane;x;y",
               50, -100., 100, 50, -100., 100.); 
  analysisManager  // H2-ID = 1                                                
    ->CreateH2("dcout_hitposition_xy","dcout : hit position on x-y plane;x;y", 
               50, -100., 100, 50, -100., 100.);
  analysisManager  // H2-ID = 2                                                
    ->CreateH2("analysis_theta_vs_cosphi","analysis : theta vs. cos(phi)", 
               180, 0., 180., 200, -1., 1.);
  analysisManager  // H2-ID = 3                                                
    ->CreateH2("analysis_theta_vs_sinphi","analysis : theta vs. sin(phi)", 
               180, 0., 180., 200, -1., 1.);

  // Creating tree
  analysisManager->CreateNtuple("EventTree", "Event Tree");

  analysisManager->CreateNtupleIColumn("dcin_nhit");        // column Id = 0
  analysisManager->CreateNtupleFColumn("dcin_position_x");  // column Id = 1
  analysisManager->CreateNtupleFColumn("dcin_position_y");  // column Id = 2
  analysisManager->CreateNtupleFColumn("dcin_position_z");  // column Id = 3
  analysisManager->CreateNtupleFColumn("dcin_momentum_x");  // column Id = 4
  analysisManager->CreateNtupleFColumn("dcin_momentum_y");  // column Id = 5
  analysisManager->CreateNtupleFColumn("dcin_momentum_z");  // column Id = 6

  analysisManager->CreateNtupleIColumn("dcout_nhit");       // column Id = 7
  analysisManager->CreateNtupleFColumn("dcout_position_x"); // column Id = 8
  analysisManager->CreateNtupleFColumn("dcout_position_y"); // column Id = 9
  analysisManager->CreateNtupleFColumn("dcout_position_z"); // column Id =10
  analysisManager->CreateNtupleFColumn("dcout_momentum_x"); // column Id =11
  analysisManager->CreateNtupleFColumn("dcout_momentum_y"); // column Id =12
  analysisManager->CreateNtupleFColumn("dcout_momentum_z"); // column Id =13

  analysisManager->FinishNtuple();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
  delete G4AnalysisManager::Instance();  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run* /*run*/)
{ 
  G4long random_seed  = time(NULL);
  G4int random_luxury = 5;
  CLHEP::HepRandom::setTheSeed(random_seed,random_luxury);

  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(true);
  G4RunManager::GetRunManager()->SetRandomNumberStoreDir("./rndm/");

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // Open an output file 
  // The default file name is set in RunAction::RunAction(),
  // it can be overwritten in a macro
  analysisManager->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run* /*run*/)
{
  // save histograms & ntuple
  //
  auto analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
