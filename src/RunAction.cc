#include "RunAction.hh"
#include "G4Run.hh"

RunAction::RunAction()
{
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);

    // Criar uma árvore para salvar as variáveis
    analysisManager->CreateNtuple("Data", "Simulação");
    analysisManager->CreateNtupleDColumn("InitialAlphaEnergy"); // Energia inicial da alfa
    analysisManager->CreateNtupleIColumn("NumPhotonsEmitted");  // Nº de fótons emitidos
    analysisManager->CreateNtupleIColumn("NumPhotonsRemitted"); 
    analysisManager->CreateNtupleIColumn("NumPhotonsDetected"); // Nº de fótons detectados
    analysisManager->FinishNtuple();
}

RunAction::~RunAction() {
    delete G4AnalysisManager::Instance();
}

void RunAction::BeginOfRunAction(const G4Run*) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->OpenFile("output.root");
}

void RunAction::EndOfRunAction(const G4Run*) {
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    analysisManager->Write();
    analysisManager->CloseFile();
}
