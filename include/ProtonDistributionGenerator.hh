// ProtonDistributionGenerator.hh

#ifndef ProtonDistributionGenerator_hh
#define ProtonDistributionGenerator_hh

#include"G4ThreeVector.hh"
#include"TTree.h"
#include"TFile.h"

class ProtonDistributionGenerator
{
  private:
    ProtonDistributionGenerator();
    ~ProtonDistributionGenerator();

  public:
    ProtonDistributionGenerator(const ProtonDistributionGenerator&) = delete;
    ProtonDistributionGenerator& operator=(const ProtonDistributionGenerator&) = delete;
    ProtonDistributionGenerator(ProtonDistributionGenerator&&) = delete;
    ProtonDistributionGenerator& operator=(ProtonDistributionGenerator&&) = delete;

    static ProtonDistributionGenerator& GetInstance() {
      static ProtonDistributionGenerator instance;
      return instance;
    }

    void Generate();
    void GetEvent(G4double momentum[3], G4double normal[3], G4double reference[3]);

  private:
    TFile* _in_root_file;
    TTree* _in_tree;

  private:
    G4int _total_events;
    G4int _i_event;
    G4double _momentum_x;
    G4double _momentum_y;
    G4double _momentum_z;
    G4double _normal_x;
    G4double _normal_y;
    G4double _normal_z;
    G4double _reference_x;
    G4double _reference_y;
    G4double _reference_z;
};

#endif

