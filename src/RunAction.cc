#include "RunAction.hh"
#include "G4Run.hh"
#include "G4AnalysisManager.hh"

RunAction::RunAction() {}

RunAction::~RunAction() {
    delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("output.root");
}

void RunAction::EndOfRunAction(const G4Run*) {
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->Write();
    analysisManager->CloseFile();
}