#include "PrimaryGeneratorAction.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4GenericMessenger.hh" // for the mode-switch UI command
#include "G4RandomTools.hh"
#include "G4PhysicalConstants.hh" // needed for pi

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0),
  fMessenger(0),
  fDrellYanMode(false) // default is the "realistic (proton)" mode
{
  fParticleGun = new G4ParticleGun(1);

  // --- Define a UI command to switch modes from the terminal ---
  fMessenger = new G4GenericMessenger(this, "/demo/", "Demo Configuration");
  fMessenger->DeclareProperty("drellYanMode", fDrellYanMode, "Set true for Drell-Yan mode, false for Proton mode");
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  delete fParticleGun;
  delete fMessenger;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();

  if (fDrellYanMode) {
      // ==========================================
      // Mode 2: toy Drell-Yan (muon pair)
      // ==========================================
      G4ParticleDefinition* muPlus = particleTable->FindParticle("mu+");
      G4ParticleDefinition* muMinus = particleTable->FindParticle("mu-");

      fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., 0.)); // target center


      // --- Generate the angles ---
      // Sample theta uniformly between 0.01 and 0.1 rad (about 0.5 to 5.7 degrees)
      G4double theta1 = 0.01 + G4UniformRand() * 0.09; 
      G4double phi1   = G4UniformRand() * 2.0 * CLHEP::pi;

      G4double px1 = std::sin(theta1) * std::cos(phi1);
      G4double py1 = std::sin(theta1) * std::sin(phi1);
      G4double pz1 = std::cos(theta1);

      // First particle: mu+
      fParticleGun->SetParticleDefinition(muPlus);
      fParticleGun->SetParticleEnergy(25.0 * GeV);
      // fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.05, 0.0, 1.0).unit());
      fParticleGun->SetParticleMomentumDirection(G4ThreeVector(px1, py1, pz1).unit());
      fParticleGun->GeneratePrimaryVertex(anEvent);

      // Second particle: mu- (back-to-back in the transverse plane)
      fParticleGun->SetParticleDefinition(muMinus);
      fParticleGun->SetParticleEnergy(25.0 * GeV);
      // fParticleGun->SetParticleMomentumDirection(G4ThreeVector(-0.05, 0.0, 1.0).unit());
      fParticleGun->SetParticleMomentumDirection(G4ThreeVector(-px1, -py1, pz1).unit());
      fParticleGun->GeneratePrimaryVertex(anEvent);

  } else {
      // ==========================================
      // Mode 1: realistic (50 GeV proton)
      // ==========================================
      G4ParticleDefinition* proton = particleTable->FindParticle("proton");
      fParticleGun->SetParticleDefinition(proton);
      fParticleGun->SetParticleEnergy(50.0 * GeV);
      fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
      fParticleGun->SetParticlePosition(G4ThreeVector(0., 0., -20.0*cm)); // just upstream of the target
      fParticleGun->GeneratePrimaryVertex(anEvent);
  }
}
