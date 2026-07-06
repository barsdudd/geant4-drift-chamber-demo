#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "Randomize.hh"
#include <cmath>

SteppingAction::SteppingAction() : G4UserSteppingAction() {}
SteppingAction::~SteppingAction() {}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4Track* track = step->GetTrack();
  G4Event* event = G4EventManager::GetEventManager()->GetNonconstCurrentEvent();
  
  EventInfo* info = static_cast<EventInfo*>(event->GetUserInformation());
  if (!info) return;
  
  G4String particleName = track->GetDefinition()->GetParticleName();
  
  if (particleName == "mu-" || particleName == "mu+") {
    info->hasMuon = true;
    
    G4VPhysicalVolume* volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();
    if (volume && volume->GetName() == "DriftChamber") {
      info->reachedChamber = true;
      
      if (step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary) {
	G4int planeID = volume->GetCopyNo();
	G4ThreeVector pos = step->GetPreStepPoint()->GetPosition();
	G4double x = pos.x();
	G4double y = pos.y();
        
	// Constants
	G4double wireSpacing = 2.0*cm;
	G4double shift = 1.0*cm;
	G4double theta = 14.0*deg;
	G4double offset = -1.5*m;
        
	G4double w = 0.0;
	G4String planeName = "";
	
	// Physical plane order (i=0,1: U/U', i=2,3: X/X', i=4,5: V/V')
	switch (planeID) {
	case 0:
	  w = x * std::cos(theta) + y * std::sin(theta);
	  planeName = "U ";
	  break;
	case 1:
	  w = x * std::cos(theta) + y * std::sin(theta) - shift;
	  planeName = "U'";
	  break;
	case 2: // first middle plane
	  w = x;
	  planeName = "X ";
	  break;
	case 3: // second middle plane
	  w = x - shift;
	  planeName = "X'";
	  break;
	case 4:
	  w = x * std::cos(theta) - y * std::sin(theta);
	  planeName = "V ";
	  break;
	case 5:
	  w = x * std::cos(theta) - y * std::sin(theta) - shift;
	  planeName = "V'";
	  break;
	}
	
	// Compute the wire ID
	G4int wireID = std::floor((w - offset) / wireSpacing);
        
	// Wire center position
	G4double wirePos = wireID * wireSpacing + offset + wireSpacing / 2.0;
	G4double trueDist = std::abs(w - wirePos);
        
	// Smear with 0.3 mm resolution
	G4double smearedDist = trueDist + G4RandGauss::shoot(0.0, 0.3*mm);
	if (smearedDist < 0.0) smearedDist = 0.0;
        
	Hit hit;
	hit.planeID = planeID;
	hit.wireID = wireID;
	hit.driftDistance = smearedDist;
        
	info->hits.push_back(hit);
      }
    }
  }
}
