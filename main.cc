#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"

#include "FTFP_BERT.hh"
#include "G4OpticalPhysics.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4PhysListFactory.hh"
#include "G4EmStandardPhysics.hh"
#include "G4VModularPhysicsList.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"


int main(int argc, char** argv) {
    // Criar o run manager
    auto* runManager = new G4RunManager();

    // Definir a geometria do detector
    runManager->SetUserInitialization(new DetectorConstruction());

    // Definir a physics list (FTFP_BERT + G4OpticalPhysics)

    G4VModularPhysicsList *physicsList = new FTFP_BERT;
    physicsList->ReplacePhysics(new G4EmStandardPhysics_option2());

    G4OpticalPhysics *opticalPhysics = new G4OpticalPhysics();
    physicsList->RegisterPhysics(new G4OpticalPhysics());

    runManager->SetUserInitialization(physicsList);

    // Definir as ações do usuário
    runManager->SetUserInitialization(new ActionInitialization());

    // Inicializar o kernel do Geant4
    runManager->Initialize();

    // Inicializar o gerenciador de visualização
    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    // Verificar se vamos rodar em modo interativo ou batch
    if (argc == 1) {
        // Modo interativo
        G4UIExecutive* ui = new G4UIExecutive(argc, argv);
        G4UImanager::GetUIpointer()->ApplyCommand("/control/execute init_vis.mac");
        ui->SessionStart();
        delete ui;
    } else {
        // Modo batch (executa um macro passado como argumento)
        G4String command = "/control/execute ";
        G4String fileName = argv[1];
        G4UImanager::GetUIpointer()->ApplyCommand(command + fileName);
    }

    // Clean-up
    delete visManager;
    delete runManager;

    return 0;
}
