#include "RunAction.hh"
#include "G4Run.hh"
#include "G4Threading.hh"
#include <TFile.h>
#include <TTree.h>

G4Mutex RunAction::rootMutex = G4MUTEX_INITIALIZER;
TFile* RunAction::outputFile = nullptr;
TTree* RunAction::hitTree = nullptr;
int RunAction::rootEventID = 0;
std::vector<int> RunAction::rootPlanes;
std::vector<int> RunAction::rootWires;
std::vector<double> RunAction::rootDrifts;

RunAction::RunAction() : G4UserRunAction() {}
RunAction::~RunAction() {}

void RunAction::BeginOfRunAction(const G4Run*) {
    // Create the output file on the master thread only
    if (IsMaster()) {
        outputFile = new TFile("ChamberHits.root", "RECREATE");
        hitTree = new TTree("Hits", "Drift Chamber Hits");
        
        hitTree->Branch("EventID", &rootEventID, "EventID/I");
        hitTree->Branch("PlaneID", &rootPlanes);
        hitTree->Branch("WireID", &rootWires);
        hitTree->Branch("DriftDistance", &rootDrifts);
    }
}

void RunAction::EndOfRunAction(const G4Run*) {
    // Write and close the file on the master thread only
    if (IsMaster() && outputFile) {
        outputFile->Write();
        outputFile->Close();
        delete outputFile;
        outputFile = nullptr;
    }
}
