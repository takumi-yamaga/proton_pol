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
/// \copied from B5EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "ProtonDistributionGenerator.hh"

#include "G4UserEventAction.hh"
#include "globals.hh"

#include <vector>
#include <array>

// named constants
const G4int kTotalDCs = 2;
const G4int kTotalHistogramsForDC = 3;
const G4int kTotalHistogramsForAnalysis = 6;

const G4int kDCINId = 0;
const G4int kDCOUTId = 1;

/// Event action

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    virtual ~EventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

private:
    // hit collections Ids
    std::array<G4int, kTotalDCs> dc_hitcollection_id_;
    // histograms Ids
    std::array<std::array<G4int, kTotalDCs>, kTotalHistogramsForDC> dc_histogram_id_;
    std::array<G4int, kTotalHistogramsForAnalysis> analysis_histogram_id_;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
