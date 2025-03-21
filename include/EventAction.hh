#ifndef EVENTACTION_HH
#define EVENTACTION_HH

#include "G4UserEventAction.hh"
#include "globals.hh"
#include "vector"

class EventAction : public G4UserEventAction {
public:
    EventAction();
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);

    // Setters que o SteppingAction vai usar para atualizar informações
    void SetInitialAlphaEnergy(G4double energy);
    void SetFinalAlphaEnergy(G4double energy);
    void AddEmittedPhoton();
    void AddRemittedPhoton();
    void AddDetectedPhoton();
    void AddWavelengthEmitted(G4double energy);
    void AddWavelengthRemitted(G4double energy);

private:
    G4double fInitialAlphaEnergy;
    G4double fFinalAlphaEnergy;
    G4int fNumPhotonsEmitted;
    G4int fNumPhotonsRemitted;
    G4int fNumPhotonsDetected;
    std::vector<G4double> fWavelengthEmitted;
    std::vector<G4double> fWavelengthRemitted;

};

#endif
