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
/// \copied from B5DriftChamberHit.hh
/// \brief Definition of the DriftChamberHit class

#ifndef DriftChamberHit_h
#define DriftChamberHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"

class G4AttDef;
class G4AttValue;

/// Drift chamber hit
///
/// It records:
/// - the layer ID
/// - the particle time
/// - the particle local and global positions

class DriftChamberHit : public G4VHit
{
  public:
    DriftChamberHit();
    DriftChamberHit(G4int layerID);
    DriftChamberHit(const DriftChamberHit &right);
    virtual ~DriftChamberHit();

    const DriftChamberHit& operator=(const DriftChamberHit &right);
    G4bool operator==(const DriftChamberHit &right) const;
    
    inline void *operator new(size_t);
    inline void operator delete(void *aHit);
    
    virtual void Draw();
    virtual const std::map<G4String,G4AttDef>* GetAttDefs() const;
    virtual std::vector<G4AttValue>* CreateAttValues() const;
    virtual void Print();

    inline void SetTrackID(G4int id) { track_id_ = id; }
    inline G4int GetTrackID() const { return track_id_; }

    inline void SetParentID(G4int id) { parent_id_ = id; }
    inline G4int GetParentID() const { return parent_id_; }

    inline void SetParticleID(G4int id) { particle_id_ = id; }
    inline G4int GetParticleID() const { return particle_id_; }

    inline void SetLayerID(G4int id) { layer_id_ = id; }
    inline G4int GetLayerID() const { return layer_id_; }

    inline void SetHitTime(G4double time) { hit_time_ = time; }
    inline G4double GetHitTime() const { return hit_time_; }

    inline void SetLocalPosition(G4ThreeVector position) { local_position_ = position; }
    inline G4ThreeVector GetLocalPosition() const { return local_position_; }

    inline void SetGlobalPosition(G4ThreeVector position) { global_position_ = position; }
    inline G4ThreeVector GetGlobalPosition() const { return global_position_; }
    
    inline void SetMomentum(G4ThreeVector momentum) { momentum_ = momentum; }
    inline G4ThreeVector GetMomentum() const { return momentum_; }
    
    inline void SetPolarization(G4ThreeVector polarization) { polarization_ = polarization; }
    inline G4ThreeVector GetPolarization() const { return polarization_; }
    
  private:
    G4int track_id_;
    G4int parent_id_;
    G4int particle_id_;
    G4int layer_id_;
    G4double hit_time_;
    G4ThreeVector local_position_;
    G4ThreeVector global_position_;
    G4ThreeVector momentum_;
    G4ThreeVector polarization_;
};

using DriftChamberHitsCollection = G4THitsCollection<DriftChamberHit>;

extern G4ThreadLocal G4Allocator<DriftChamberHit>* DriftChamberHitAllocator;

inline void* DriftChamberHit::operator new(size_t)
{
  if (!DriftChamberHitAllocator) {
       DriftChamberHitAllocator = new G4Allocator<DriftChamberHit>;
  }
  return (void*)DriftChamberHitAllocator->MallocSingle();
}

inline void DriftChamberHit::operator delete(void* aHit)
{
  DriftChamberHitAllocator->FreeSingle((DriftChamberHit*) aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
