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
      = {{ "analysis_theta",
        "analysis_phi",
        "analysis_cosphi",
        "analysis_sinphi",
        "analysis_theta_vs_cosphi",
        "analysis_theta_vs_sinphi" }};

    for (auto i_dc = 0; i_dc < kTotalDCs; ++i_dc) {
      // hit collections IDs
      dc_hitcollection_id_[i_dc] = sdManager->GetCollectionID(dc_hitcollection_name[i_dc]);

      // histograms IDs
      dc_histogram_id_[0][i_dc] = analysisManager->GetH1Id(dc_histogram_name[0][i_dc]);
      dc_histogram_id_[1][i_dc] = analysisManager->GetH1Id(dc_histogram_name[1][i_dc]);
      dc_histogram_id_[2][i_dc] = analysisManager->GetH2Id(dc_histogram_name[2][i_dc]);
    }
    for (auto i_analysis = 0; i_analysis < kTotalHistogramsForAnalysis; ++i_analysis) {
      if(i_analysis<4){
        analysis_histogram_id_[i_analysis]
          = analysisManager->GetH1Id(analysis_histogram_name[i_analysis]);
      }
      else{
        analysis_histogram_id_[i_analysis]
          = analysisManager->GetH2Id(analysis_histogram_name[i_analysis]);
      }
    }
  }
}     

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::EndOfEventAction(const G4Event* event)
{
  // Get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();

  // ======================================================
  // DCIN =================================================
  // ======================================================
  G4int dcin_total_hits = 0;
  G4ThreeVector dcin_position = G4ThreeVector(0);
  G4ThreeVector dcin_momentum = G4ThreeVector(0);
  G4bool dcin_has_hit = false;

  auto dcin_hc = GetHC(event,dc_hitcollection_id_[kDCINId]); 
  if(dcin_hc){
    dcin_total_hits = dcin_hc->GetSize();
    analysisManager->FillH1(dc_histogram_id_[0][kDCINId], dcin_total_hits );

    if(dcin_total_hits>0){
      dcin_has_hit = true;
      auto hit = static_cast<DriftChamberHit*>(dcin_hc->GetHit(0));
      dcin_position = hit->GetGlobalPosition();
      dcin_momentum = hit->GetMomentum();
      analysisManager->FillH1(dc_histogram_id_[1][kDCINId], dcin_momentum.theta()/deg);
      analysisManager->FillH2(dc_histogram_id_[2][kDCINId], dcin_position.x(), dcin_position.y());
    }
  }
  // ======================================================
  // ======================================================



  // ======================================================
  // DCOUT ================================================
  // ======================================================
  G4int dcout_total_hits = 0;
  G4ThreeVector dcout_position = G4ThreeVector(0);
  G4ThreeVector dcout_momentum = G4ThreeVector(0);
  G4bool dcout_has_hit = false;

  auto dcout_hc = GetHC(event,dc_hitcollection_id_[kDCOUTId]); 
  if(dcout_hc){
    dcout_total_hits = dcout_hc->GetSize();
    analysisManager->FillH1(dc_histogram_id_[0][kDCOUTId], dcout_total_hits );

    if(dcout_total_hits>0){
      dcout_has_hit = true;
      auto hit = static_cast<DriftChamberHit*>(dcout_hc->GetHit(0));
      dcout_position = hit->GetGlobalPosition();
      dcout_momentum = hit->GetMomentum();
      analysisManager->FillH1(dc_histogram_id_[1][kDCINId], dcout_momentum.theta()/deg);
      analysisManager->FillH2(dc_histogram_id_[2][kDCINId], dcout_position.x(), dcout_position.y());
    }
  }
  // ======================================================
  // ======================================================


  // ======================================================
  // Analysis =============================================
  // ======================================================
  if(dcout_has_hit){
    G4double momentum = dcout_momentum.mag()/MeV;
    G4double theta = dcout_momentum.theta()/deg;
    G4double phi   = dcout_momentum.phi()/deg;
    if(10.<theta&&theta<20.){
      analysisManager->FillH1(analysis_histogram_id_[0], theta);
      analysisManager->FillH1(analysis_histogram_id_[1], phi);
      analysisManager->FillH1(analysis_histogram_id_[2], cos(phi*deg));
      analysisManager->FillH1(analysis_histogram_id_[3], sin(phi*deg));
      analysisManager->FillH2(analysis_histogram_id_[4], theta, cos(phi*deg));
      analysisManager->FillH2(analysis_histogram_id_[5], theta, sin(phi*deg));
    }
  }
  // ======================================================
  // ======================================================


  // ======================================================
  // Fill Tree ============================================
  // ======================================================
  if(dcin_has_hit){
    analysisManager->FillNtupleIColumn(0,dcin_total_hits);
    analysisManager->FillNtupleFColumn(1,(G4float)dcin_position.x());
    analysisManager->FillNtupleFColumn(2,(G4float)dcin_position.y());
    analysisManager->FillNtupleFColumn(3,(G4float)dcin_position.z());
    analysisManager->FillNtupleFColumn(4,(G4float)dcin_momentum.x());
    analysisManager->FillNtupleFColumn(5,(G4float)dcin_momentum.y());
    analysisManager->FillNtupleFColumn(6,(G4float)dcin_momentum.z());
  }
  if(dcout_has_hit){
    analysisManager->FillNtupleIColumn(7,dcout_total_hits);
    analysisManager->FillNtupleFColumn(8,(G4float)dcout_position.x());
    analysisManager->FillNtupleFColumn(9,(G4float)dcout_position.y());
    analysisManager->FillNtupleFColumn(10,(G4float)dcout_position.z());
    analysisManager->FillNtupleFColumn(11,(G4float)dcout_momentum.x());
    analysisManager->FillNtupleFColumn(12,(G4float)dcout_momentum.y());
    analysisManager->FillNtupleFColumn(13,(G4float)dcout_momentum.z());
  }
  analysisManager->AddNtupleRow();
  // ======================================================
  // ======================================================


  ////
  //// Print diagnostics
  //// 

  //auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  //if ( printModulo == 0 || event->GetEventID() % printModulo != 0) return;

  // Drift chambers
  //for (G4int i_dc = 0; i_dc < kNumberofDCs; ++i_dc) {
  //  auto hc = GetHC(event, dc_hitcollection_id_[i_dc]);
  //  if ( ! hc ) return;
  //  G4cout << "Drift Chamber " << i_dc + 1 << " has " <<  hc->GetSize()  << " hits." << G4endl;
  //}

  // set printing per each event
  if(event->GetEventID()){
    G4int print_progress = (G4int)log10(event->GetEventID());
    G4RunManager::GetRunManager()->SetPrintProgress(pow(10,print_progress));
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
