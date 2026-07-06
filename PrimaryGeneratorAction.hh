#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"

class G4GenericMessenger; // for the mode-switch UI command

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();

    virtual void GeneratePrimaries(G4Event*);

  private:
    G4ParticleGun* fParticleGun;
    G4GenericMessenger* fMessenger; // handles the /demo/ UI commands
    G4bool fDrellYanMode;           // true: Drell-Yan mode, false: proton mode
};

#endif
