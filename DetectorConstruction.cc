#include "DetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"

DetectorConstruction::DetectorConstruction() : G4VUserDetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager* nist = G4NistManager::Instance();

  G4Material* world_mat  = nist->FindOrBuildMaterial("G4_AIR");
  G4Material* target_mat = nist->FindOrBuildMaterial("G4_lH2");
  G4Material* dump_mat   = nist->FindOrBuildMaterial("G4_Fe");
  G4Material* dc_mat     = nist->FindOrBuildMaterial("G4_Ar");

  // World
  G4Box* solidWorld = new G4Box("World", 2.*m, 2.*m, 10.*m);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);

  // 1. Target (red)
  G4Box* solidTarget = new G4Box("Target", 5.*cm, 5.*cm, 20.*cm);
  G4LogicalVolume* logicTarget = new G4LogicalVolume(solidTarget, target_mat, "Target");
  new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicTarget, "Target", logicWorld, false, 0, true);

  // 2. Beam dump (blue)
  G4Box* solidDump = new G4Box("Dump", 0.5*m, 0.5*m, 2.*m);
  G4LogicalVolume* logicDump = new G4LogicalVolume(solidDump, dump_mat, "Dump");
  new G4PVPlacement(0, G4ThreeVector(0, 0, 3.0*m), logicDump, "Dump", logicWorld, false, 0, true);

  // 3. Drift chambers (green, 40 mm spacing)
  G4Box* solidDC = new G4Box("DriftChamber", 1.*m, 1.*m, 1.5*cm); // thickness set to 1.5 cm to avoid overlaps
  G4LogicalVolume* logicDC = new G4LogicalVolume(solidDC, dc_mat, "DriftChamber");
  for (G4int i = 0; i < 6; i++) {
    G4double z_pos = 5500.0*mm + i * 40.0*mm; 
    new G4PVPlacement(0, G4ThreeVector(0, 0, z_pos), logicDC, "DriftChamber", logicWorld, false, i, true);
  }

  // Explicitly apply visualization attributes
  logicWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
  
  G4VisAttributes* targetVis = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0, 0.8)); // red
  targetVis->SetForceSolid(true);
  logicTarget->SetVisAttributes(targetVis);

  G4VisAttributes* dumpVis = new G4VisAttributes(G4Colour(0.0, 0.0, 1.0, 0.5)); // blue
  dumpVis->SetForceSolid(true);
  logicDump->SetVisAttributes(dumpVis);

  G4VisAttributes* dcVis = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0, 0.2)); // green (translucent)
  dcVis->SetForceSolid(true);
  logicDC->SetVisAttributes(dcVis);

  return physWorld;
}
