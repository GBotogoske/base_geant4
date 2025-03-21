#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4AnalysisManager.hh"

EventAction::EventAction()
    : G4UserEventAction(),
      fInitialAlphaEnergy(0.),
      fNumPhotonsEmitted(0),
      fNumPhotonsRemitted(0),
      fNumPhotonsDetected(0) {}

EventAction::~EventAction() {}

void EventAction::BeginOfEventAction(const G4Event*) {
    fNumPhotonsEmitted = 0;
    fNumPhotonsRemitted = 0;
    fNumPhotonsDetected = 0;
    fWavelengthEmitted.clear();
}

void EventAction::EndOfEventAction(const G4Event*) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillNtupleDColumn(0, fInitialAlphaEnergy);
    analysisManager->FillNtupleIColumn(1, fNumPhotonsEmitted);
    analysisManager->FillNtupleIColumn(2, fNumPhotonsRemitted);
    analysisManager->FillNtupleIColumn(3, fNumPhotonsDetected);
    analysisManager->AddNtupleRow();
}

void EventAction::SetInitialAlphaEnergy(G4double energy) {

    fInitialAlphaEnergy = energy;
}

void EventAction::AddEmittedPhoton() {
    fNumPhotonsEmitted++;
}

void EventAction::AddRemittedPhoton() {
    fNumPhotonsRemitted++;
}

void EventAction::AddDetectedPhoton() {
    fNumPhotonsDetected++;
}

void EventAction::AddWavelengthEmitted(G4double energy)
{
    G4double convert_factor=1240.0;
    fWavelengthEmitted.push_back(convert_factor/energy);
}
