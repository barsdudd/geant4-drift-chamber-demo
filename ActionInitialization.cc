#include "ActionInitialization.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "SteppingAction.hh"
#include "EventAction.hh"

ActionInitialization::ActionInitialization()
 : G4VUserActionInitialization()
{}

ActionInitialization::~ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
    // Master-side setup when running multithreaded.
    // Registering RunAction here makes the master handle file creation in
    // BeginOfRunAction and writing/closing in EndOfRunAction.
    SetUserAction(new RunAction());
}

void ActionInitialization::Build() const
{
    // Actions run on worker threads (or in single-threaded mode)
    SetUserAction(new PrimaryGeneratorAction());
    SetUserAction(new RunAction());
    SetUserAction(new SteppingAction());
    SetUserAction(new EventAction());
}
