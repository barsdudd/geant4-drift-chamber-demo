#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4VUserEventInformation.hh"
#include "globals.hh"
#include <vector>

struct Hit {
    G4int planeID;
    G4int wireID;
    G4double driftDistance;
};

class EventInfo : public G4VUserEventInformation {
public:
    G4bool hasMuon;
    G4bool reachedChamber;
    std::vector<Hit> hits;
    
    EventInfo() : hasMuon(false), reachedChamber(false) {}
    virtual ~EventInfo() {}
    virtual void Print() const override {}
};

class EventAction : public G4UserEventAction
{
  public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event) override;
    virtual void EndOfEventAction(const G4Event* event) override;
};

#endif
