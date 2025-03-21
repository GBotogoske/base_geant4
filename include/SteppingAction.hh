#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class EventAction;

class SteppingAction : public G4UserSteppingAction {
public:
    SteppingAction(EventAction* eventAction);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step* step);
    G4int GetConvertedPhotonCount() const { return fConvertedPhotonCount; }
    bool IsInsideOpticalWindow(const G4ThreeVector& position);
    
private:
    EventAction* fEventAction;
    G4int fConvertedPhotonCount;

};

#endif
