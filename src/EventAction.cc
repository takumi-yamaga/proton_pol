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
      = {{ "difference_dcin_and_dcout_direction" }};

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
  G4double dcin_direction = 0.;
  G4bool dcin_has_hit = false;
  G4double dcout_direction = 0.;
  G4bool dcout_has_hit = false;
  for (G4int i_dc = 0; i_dc < kTotalDCs; ++i_dc) {
    auto hc = GetHC(event, dc_hitcollection_id_[i_dc]);
    if ( ! hc ) return;

    auto total_hits = hc->GetSize();
    analysisManager->FillH1(dc_histogram_id_[0][i_dc], total_hits );

    for (unsigned long i_hit = 0; i_hit < total_hits; ++i_hit) {
      auto hit = static_cast<DriftChamberHit*>(hc->GetHit(i_hit));
      G4ThreeVector momentum = hit->GetMomentum();
      G4double direction = momentum.theta()*deg;
      G4ThreeVector local_position = hit->GetLocalPosition();
      analysisManager->FillH1(dc_histogram_id_[1][i_dc], direction);
      analysisManager->FillH2(dc_histogram_id_[2][i_dc], local_position.x(), local_position.y());
      if(i_hit == 0){
        if(i_dc == 0){
          dcin_has_hit = true;
          dcin_direction = direction;
        }
        else if (i_dc == 1){
          dcout_has_hit = true;
          dcout_direction = direction;
        }
      }

    }
    if(dcin_has_hit && dcout_has_hit){
      G4double diff_direction = dcin_direction - dcout_direction;
      analysisManager->FillH1(analysis_histogram_id_[0], diff_direction);
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

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
