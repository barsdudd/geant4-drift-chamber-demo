#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "G4Threading.hh" // provides G4Mutex
#include <vector>

class G4Run;
class TFile;
class TTree;

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void EndOfRunAction(const G4Run*);

    // Shared variables for ROOT output (protected by the mutex)
    static G4Mutex rootMutex;
    static TFile* outputFile;
    static TTree* hitTree;
    static int rootEventID;
    static std::vector<int> rootPlanes;
    static std::vector<int> rootWires;
    static std::vector<double> rootDrifts;
};

#endif
