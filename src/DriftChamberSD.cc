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
/// \copied from B5DriftChamberSD.cc
/// \brief Implementation of the DriftChamber class

#include "DriftChamberSD.hh"
#include "DriftChamberHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DriftChamberSD::DriftChamberSD(G4String name)
: G4VSensitiveDetector(name), 
  fHitsCollection(nullptr), fHCID(-1)
{
  collectionName.insert("dc_hitcollection");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DriftChamberSD::~DriftChamberSD()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DriftChamberSD::Initialize(G4HCofThisEvent* hce)
{
  fHitsCollection 
    = new DriftChamberHitsCollection(SensitiveDetectorName,collectionName[0]);

  if (fHCID<0) { 
     fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection); 
  }
  hce->AddHitsCollection(fHCID,fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool DriftChamberSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  auto track = step->GetTrack();

  auto charge = track->GetDefinition()->GetPDGCharge();
  if (charge==0.) return true;

  auto particle_id = track->GetParticleDefinition()->GetPDGEncoding();
  if(particle_id != 2212) return true;

  auto parent_id = track->GetParentID();
  if(parent_id != 0) return true;
  
  auto preStepPoint = step->GetPreStepPoint();

  auto touchable = step->GetPreStepPoint()->GetTouchable();
  auto motherPhysical = touchable->GetVolume(1); // mother
  auto copyNo = motherPhysical->GetCopyNo();

  auto global_position = preStepPoint->GetPosition();
  auto local_position
    = touchable->GetHistory()->GetTopTransform().TransformPoint(global_position);

  auto hit = new DriftChamberHit(copyNo);
  hit->SetGlobalPosition(global_position);
  hit->SetLocalPosition(local_position);
  hit->SetHitTime(preStepPoint->GetGlobalTime());
  hit->SetMomentum(preStepPoint->GetMomentum());
  hit->SetPolarization(track->GetPolarization());
  hit->SetTrackID(track->GetTrackID());
  hit->SetParentID(parent_id);
  hit->SetParticleID(particle_id);
  
  fHitsCollection->insert(hit);
  
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
