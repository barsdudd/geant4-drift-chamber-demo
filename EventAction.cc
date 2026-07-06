#include "EventAction.hh"
#include "RunAction.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4PrimaryVertex.hh"
#include "G4PrimaryParticle.hh"
#include "G4ParticleDefinition.hh"
#include "G4AutoLock.hh" // for mutual exclusion
#include <TTree.h>

EventAction::EventAction() : G4UserEventAction() {}
EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event* event) {
    G4EventManager::GetEventManager()->GetNonconstCurrentEvent()->SetUserInformation(new EventInfo());
}

void EventAction::EndOfEventAction(const G4Event* event) {
    EventInfo* info = static_cast<EventInfo*>(event->GetUserInformation());
    
    G4String primaryName = "";
    if (event->GetNumberOfPrimaryVertex() > 0 && event->GetPrimaryVertex(0)->GetPrimary(0) != nullptr) {
        primaryName = event->GetPrimaryVertex(0)->GetPrimary(0)->GetParticleDefinition()->GetParticleName();
    }

    if (primaryName == "proton") {
        return; 
    }

    if (info && info->reachedChamber) {
        // Acquire the lock (only one thread at a time may run the write block below)
        G4AutoLock lock(&RunAction::rootMutex);
        
        RunAction::rootEventID = event->GetEventID();
        RunAction::rootPlanes.clear();
        RunAction::rootWires.clear();
        RunAction::rootDrifts.clear();
        
        for (const auto& hit : info->hits) {
            RunAction::rootPlanes.push_back(hit.planeID);
            RunAction::rootWires.push_back(hit.wireID);
            RunAction::rootDrifts.push_back(hit.driftDistance);
        }
        
        if (RunAction::hitTree) {
            RunAction::hitTree->Fill();
        }
    }
}
