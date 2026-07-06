#include "G4RunManagerFactory.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#include "FTFP_BERT.hh" // Standard physics list

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

int main(int argc, char** argv)
{
  // Initialize the UI (start in interactive mode if no arguments are given)
  G4UIExecutive* ui = nullptr;
  if ( argc == 1 ) {
    ui = new G4UIExecutive(argc, argv);
  }
  
  // Construct the run manager (default manager with multithreading support)
  auto* runManager = G4RunManagerFactory::CreateRunManager(G4RunManagerType::Default);
  
  // 1. Register the geometry
  runManager->SetUserInitialization(new DetectorConstruction());
  
  // 2. Register the physics processes (FTFP_BERT, the standard list for high energies)
  G4VModularPhysicsList* physicsList = new FTFP_BERT;
  runManager->SetUserInitialization(physicsList);
  
  // 3. Register the user actions (beam generation, etc.)
  runManager->SetUserInitialization(new ActionInitialization());
  
  // Initialize the visualization manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  
  // Get the UI manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  if ( ! ui ) {
    // Batch mode (a macro file was passed as an argument)
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // Interactive mode (load vis.mac, etc.)
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  }
  
  // Cleanup
  delete visManager;
  delete runManager;
  
  return 0;
}
