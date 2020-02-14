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
/// \copied from B5EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "DriftChamberHit.hh"
#include "Constants.hh"
#include "Analysis.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

using std::array;
using std::vector;


namespace {

// Utility function which finds a hit collection with the given Id
// and print warnings if not found 
G4VHitsCollection* GetHC(const G4Event* event, G4int collId) {
  auto hce = event->GetHCofThisEvent();
  if (!hce) {
      G4ExceptionDescription msg;
      msg << "No hits collection of this event found." << G4endl; 
      G4Exception("EventAction::EndOfEventAction()",
                  "Code001", JustWarning, msg);
      return nullptr;
  }

  auto hc = hce->GetHC(collId);
  if ( ! hc) {
    G4ExceptionDescription msg;
    msg << "Hits collection " << collId << " of this event not found." << G4endl; 
    G4Exception("EventAction::EndOfEventAction()",
                "Code001", JustWarning, msg);
  }
  return hc;  
}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
: G4UserEventAction(), 
  dc_hitcollection_id_{{ -1, -1 }}
{
  // set printing per each event
  G4RunManager::GetRunManager()->SetPrintProgress(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event*)
{
  // Find hit collections and histogram Ids by names (just once)
  // and save them in the data members of this class

  if (dc_hitcollection_id_[0] == -1) {
    auto sdManager = G4SDManager::GetSDMpointer();
    auto analysisManager = G4AnalysisManager::Instance();

    // hits collections names
    array<G4String, kTotalDCs> dc_hitcollection_name 
      = {{ "dcin/dc_hitcollection", "dcout/dc_hitcollection" }};

    // histograms names
    array<array<G4String, kTotalDCs>, kTotalHistogramsForDC> dc_histogram_name 
      = {{ {{ "dcin_numhit", "dcout_numhit" }},
        {{ "dcin_direction", "dcout_direction" }},
        {{ "dcin_hitposition_xy", "dcout_hitposition_xy" }} }};
    array<G4String, kTotalHistogramsForAnalysis> analysis_histogram_name 
      = {{ "scattering_angle",
        "scattering_angle_x",
        "scattering_angle_y" }};

    for (auto i_dc = 0; i_dc < kTotalDCs; ++i_dc) {
      // hit collections IDs
      dc_hitcollection_id_[i_dc] = sdManager->GetCollectionID(dc_hitcollection_name[i_dc]);

      // histograms IDs
      dc_histogram_id_[0][i_dc] = analysisManager->GetH1Id(dc_histogram_name[0][i_dc]);
      dc_histogram_id_[1][i_dc] = analysisManager->GetH1Id(dc_histogram_name[1][i_dc]);
      dc_histogram_id_[2][i_dc] = analysisManager->GetH2Id(dc_histogram_name[2][i_dc]);
    }
    for (auto i_analysis = 0; i_analysis < kTotalHistogramsForAnalysis; ++i_analysis) {
      analysis_histogram_id_[i_analysis]
        = analysisManager->GetH1Id(analysis_histogram_name[i_analysis]);
    }
  }
}     

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::EndOfEventAction(const G4Event* event)
{
  //
  // Fill histograms & ntuple
  // 

  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();


  // Drift chambers hits

  // DCIN
  G4ThreeVector dcin_momentum = G4ThreeVector(0);
  G4bool dcin_has_hit = false;
  auto dcin_hc = GetHC(event,dc_hitcollection_id_[kDCINId]); 
  if(dcin_hc){
    auto total_hits = dcin_hc->GetSize();
    analysisManager->FillH1(dc_histogram_id_[0][kDCINId], total_hits );

    if(total_hits==1){
      auto hit = static_cast<DriftChamberHit*>(dcin_hc->GetHit(0));
      dcin_momentum = hit->GetMomentum();
      G4double direction = dcin_momentum.theta()/deg;
      G4ThreeVector local_position = hit->GetLocalPosition();
      analysisManager->FillH1(dc_histogram_id_[1][kDCINId], direction);
      analysisManager->FillH2(dc_histogram_id_[2][kDCINId],
          local_position.x(), local_position.y());
      dcin_has_hit = true;
      G4ThreeVector polarization = hit->GetPolarization();
      //G4cout << "==============================================================" << G4endl;
      //G4cout << "initial polarization " << G4endl;
      //G4cout << polarization.x() << ", " << polarization.y() << ", " << polarization.z() << G4endl;
      //G4cout << "==============================================================" << G4endl;
    }
  }

  // DCOUT
  G4ThreeVector dcout_momentum = G4ThreeVector(0);
  auto dcout_hc = GetHC(event,dc_hitcollection_id_[kDCOUTId]); 
  if(dcout_hc){
    auto total_hits = dcout_hc->GetSize();
    analysisManager->FillH1(dc_histogram_id_[0][kDCOUTId], total_hits );

    for(unsigned long i_hit = 0; i_hit<total_hits; i_hit++){
      auto hit = static_cast<DriftChamberHit*>(dcout_hc->GetHit(i_hit));
      dcout_momentum = hit->GetMomentum();
      G4double direction = dcout_momentum.theta()/deg;
      G4ThreeVector local_position = hit->GetLocalPosition();
      analysisManager->FillH1(dc_histogram_id_[1][kDCOUTId], direction);
      analysisManager->FillH2(dc_histogram_id_[2][kDCOUTId],
          local_position.x(), local_position.y());
      if(dcin_has_hit){
        G4double scattering_angle = acos(dcin_momentum.dot(dcout_momentum)/dcin_momentum.mag()/dcout_momentum.mag())/deg;
        analysisManager->FillH1(analysis_histogram_id_[0], scattering_angle);

        G4ThreeVector dcin_momentum_x = dcin_momentum;
        dcin_momentum_x.setY(0.);
        G4ThreeVector dcout_momentum_x = dcout_momentum;
        dcout_momentum_x.setY(0.);
        G4double scattering_angle_x = acos(dcin_momentum_x.dot(dcout_momentum_x)/dcin_momentum_x.mag()/dcout_momentum_x.mag())/deg;
        scattering_angle_x *= (dcout_momentum_x.x()-dcin_momentum_x.x());
        scattering_angle_x /= fabs(dcout_momentum_x.x()-dcin_momentum_x.x());
        analysisManager->FillH1(analysis_histogram_id_[1], scattering_angle_x);

        G4ThreeVector dcin_momentum_y = dcin_momentum;
        dcin_momentum_y.setX(0.);
        G4ThreeVector dcout_momentum_y = dcout_momentum;
        dcout_momentum_y.setX(0.);
        G4double scattering_angle_y = acos(dcin_momentum_y.dot(dcout_momentum_y)/dcin_momentum_y.mag()/dcout_momentum_y.mag())/deg;
        scattering_angle_y *= (dcout_momentum_y.y()-dcin_momentum_y.y());
        scattering_angle_y /= fabs(dcout_momentum_y.y()-dcin_momentum_y.y());
        analysisManager->FillH1(analysis_histogram_id_[2], scattering_angle_y);
        //G4ThreeVector polarization = hit->GetPolarization();
        //G4cout << polarization.x() << ", " << polarization.y() << ", " << polarization.z() << G4endl;
        //G4cout << hit->GetParticleID() << " : " << hit->GetTrackID() << ", " << hit->GetParentID() << std::endl;
        //G4cout << " --> (" << local_position.x() << ", " << local_position.y() << ", " << local_position.z() << ")" << G4endl;
      }
    }
  }


  //
  // Print diagnostics
  // 

  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( printModulo == 0 || event->GetEventID() % printModulo != 0) return;

  // Drift chambers
  //for (G4int i_dc = 0; i_dc < kNumberofDCs; ++i_dc) {
  //  auto hc = GetHC(event, dc_hitcollection_id_[i_dc]);
  //  if ( ! hc ) return;
  //  G4cout << "Drift Chamber " << i_dc + 1 << " has " <<  hc->GetSize()  << " hits." << G4endl;
  //}

  // set printing per each event
  if(event->GetEventID() >= 10){
    G4RunManager::GetRunManager()->SetPrintProgress(10);
  }
  if(event->GetEventID() >= 100){
    G4RunManager::GetRunManager()->SetPrintProgress(100);
  }
  if(event->GetEventID() >= 1000){
    G4RunManager::GetRunManager()->SetPrintProgress(1000);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
