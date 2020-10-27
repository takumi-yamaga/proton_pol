// ProtonDistributionGenerator.cc

#include "ProtonDistributionGenerator.hh"

ProtonDistributionGenerator::ProtonDistributionGenerator()
{
  _in_root_file = new TFile("long_cdc_lpn_bs_wcap.root","read");
  _in_tree = (TTree*)_in_root_file->Get("tree");
  _i_event = 0;
  _total_events = _in_tree->GetEntries();
  G4cout << "reading file of <ProtonDistributionGenerator> :: Success!" << G4endl;
  G4cout << "number of entries in tree : " << _total_events << G4endl;

  _in_tree->SetBranchAddress("momentum_x",&_momentum_x);
  _in_tree->SetBranchAddress("momentum_y",&_momentum_y);
  _in_tree->SetBranchAddress("momentum_z",&_momentum_z);
  _in_tree->SetBranchAddress("normal_x",&_normal_x);
  _in_tree->SetBranchAddress("normal_y",&_normal_y);
  _in_tree->SetBranchAddress("normal_z",&_normal_z);
  _in_tree->SetBranchAddress("reference_x",&_reference_x);
  _in_tree->SetBranchAddress("reference_y",&_reference_y);
  _in_tree->SetBranchAddress("reference_z",&_reference_z);
}

ProtonDistributionGenerator::~ProtonDistributionGenerator()
{
  _in_root_file->Close();
  delete _in_root_file;
}

void ProtonDistributionGenerator::Generate()
{
  if(_i_event>=_total_events){
    _i_event=0;
  }
  _in_tree->GetEntry(_i_event);
  _i_event++;

  return;
}

void ProtonDistributionGenerator::GetEvent(G4double momentum[3], G4double normal[3], G4double reference[3])
{
  momentum[0] = _momentum_x;
  momentum[1] = _momentum_y;
  momentum[2] = _momentum_z;

  normal[0] = _normal_x;
  normal[1] = _normal_y;
  normal[2] = _normal_z;

  reference[0] = _reference_x;
  reference[1] = _reference_y;
  reference[2] = _reference_z;

  return;
}
