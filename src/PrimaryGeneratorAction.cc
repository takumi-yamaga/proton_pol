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
/// \copied from B5PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),     
  particlegun_(nullptr), messenger_(nullptr), 
  proton_(nullptr),
  momentum_(200.*MeV),
  randomize_primary_(false)
{
  G4int num_particle = 1;
  particlegun_ = new G4ParticleGun(num_particle);
  
  auto particleTable = G4ParticleTable::GetParticleTable();
  proton_ = particleTable->FindParticle("proton");
  
  // default particle kinematics
  particlegun_->SetParticlePosition(G4ThreeVector(0.,0.,-50.*mm));
  particlegun_->SetParticleDefinition(proton_);
  
  // define commands for this class
  DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete particlegun_;
  delete messenger_;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
  G4ParticleDefinition* particle = proton_;  
  particlegun_->SetParticleDefinition(proton_);

  auto pp = momentum_;
  auto mass = particle->GetPDGMass();
  auto ekin = std::sqrt(pp*pp+mass*mass)-mass;
  particlegun_->SetParticleEnergy(ekin);

  auto direction = G4ThreeVector(0.,0.,1.);
  particlegun_->SetParticleMomentumDirection(direction);

  auto polarization = G4ThreeVector(0.,1.,0.);
  particlegun_->SetParticlePolarization(polarization);

  particlegun_->GeneratePrimaryVertex(event);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::DefineCommands()
{
  // Define /proton_pol/generator command directory using generic messenger class
  messenger_ 
    = new G4GenericMessenger(this, 
        "/proton_pol/generator/", 
        "Primary generator control");

  // momentum command
  auto& momentumCmd
    = messenger_->DeclarePropertyWithUnit("momentum", "GeV", momentum_, 
        "Mean momentum of primaries.");
  momentumCmd.SetParameterName("p", true);
  momentumCmd.SetRange("p>=0.");                                
  momentumCmd.SetDefaultValue("1.");

  // randomizePrimary command
  auto& randomCmd
    = messenger_->DeclareProperty("randomizePrimary", randomize_primary_);
  G4String guidance
    = "Boolean flag for randomizing primary particle types.\n";   
  guidance
    += "In case this flag is false, you can select the primary particle\n";
  guidance += "  with /gun/particle command.";                               
  randomCmd.SetGuidance(guidance);
  randomCmd.SetParameterName("flg", true);
  randomCmd.SetDefaultValue("true");
}

//..oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
