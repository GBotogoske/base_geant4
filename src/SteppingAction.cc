#include "SteppingAction.hh"
#include "EventAction.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4OpticalPhoton.hh"
#include "G4Alpha.hh"
#include "G4SystemOfUnits.hh"
#include "G4VProcess.hh"

SteppingAction::SteppingAction(EventAction* eventAction)
    : fEventAction(eventAction) {}

SteppingAction::~SteppingAction() {}

bool SteppingAction::IsInsideOpticalWindow(const G4ThreeVector& position) {
    G4double window_x_min = -5 * mm;
    G4double window_x_max = 5 * mm;
    G4double window_y_min = -5 * mm;
    G4double window_y_max = 5 * mm;
    G4double window_z_min = 10 * mm;
    G4double window_z_max = 12 * mm;
    
    return (position.x() > window_x_min && position.x() < window_x_max &&
            position.y() > window_y_min && position.y() < window_y_max &&
            position.z() > window_z_min && position.z() < window_z_max);
}


void SteppingAction::UserSteppingAction(const G4Step* step) {

    G4Track* track = step->GetTrack();
    
    G4StepPoint *aPrePoint = step->GetPreStepPoint();
    G4StepPoint *aPostPoint = step->GetPostStepPoint();
    G4VPhysicalVolume *aPrePV = aPrePoint->GetPhysicalVolume();
    G4String PreVolName = "";
    G4VPhysicalVolume *aPostPV = aPostPoint->GetPhysicalVolume();
    G4String PostVolName = "";
    if (aPrePV)
        PreVolName = aPrePV->GetName();
    if (aPrePV)
        PostVolName = aPostPV->GetName();

    //std::cout << track->GetDefinition()->GetParticleName() << std::endl;
    if (track->GetDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()) {

        const G4VProcess* creatorProcess = track->GetCreatorProcess();
        G4String processName = creatorProcess->GetProcessName();
        G4LogicalVolume* currentVolume = track->GetVolume()->GetLogicalVolume();
        G4String volumeName = currentVolume->GetName();
        
        G4double photonEnergy = track->GetKineticEnergy();  // pega a energia do fóton
         // Contagem de fótons na criação (como antes)
        if (track->GetCurrentStepNumber() == 1) {
         
            if (creatorProcess) {
                if (processName == "Scintillation") {
                    fEventAction->AddEmittedPhoton();
                    fEventAction->AddWavelengthEmitted(photonEnergy);
                }
                else if (processName == "OpWLS") {
                    fEventAction->AddRemittedPhoton();
                    fEventAction->AddWavelengthRemitted(photonEnergy);
                }
            }
        }

        // Verifica se fóton entrou no volume metálico ou na ARAPUCA
        if (volumeName == "MetalBlock" || volumeName == "ArapucaVolume") {
            G4ThreeVector prePos = aPrePoint->GetPosition();
            G4ThreeVector postPos = aPostPoint->GetPosition();
            G4double arapucaTopSurfaceZ = (-10.0*cm) + (2.0*cm) + (0.5*cm);  // -metalSize/2 + fArapucaHeight + 0.5 cm
            //std::cout << PreVolName << " " << PostVolName << std::endl;
            //std::cout << prePos.z() << " " << postPos.z() << std::endl;
            if (volumeName =="ArapucaVolume" && processName == "OpWLS")
            {   
                if (prePos.z() >= arapucaTopSurfaceZ) {
                    fEventAction->AddDetectedPhoton();  // contou!
                }
            }
            track->SetTrackStatus(fStopAndKill); // mata o fóton imediatamente
            return;
        }


    }
}
    
