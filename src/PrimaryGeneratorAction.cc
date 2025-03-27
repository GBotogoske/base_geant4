#include "PrimaryGeneratorAction.hh"
#include "EventAction.hh"
#include "G4RunManager.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include <cmath>

PrimaryGeneratorAction::PrimaryGeneratorAction() {
    // Criando a fonte de partículas (1 partícula por evento)
    fParticleGun = new G4ParticleGun(1);

    // Definição da partícula alfa
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "alpha";
    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
    fParticleGun->SetParticleDefinition(particle);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    // Espectro de energias do Am-241
    G4double energies[] = {5.388*MeV, 5.443*MeV, 5.486*MeV, 5.512*MeV, 5.544*MeV};
    G4double probabilities[] = {1.4/100, 12.8/100, 85.2/100, 0.2/100, 0.34/100};
    
    // Escolher aleatoriamente uma energia com base nas probabilidades
    G4double rand = G4UniformRand();
    G4double cumulative = 0.0;
    G4double selectedEnergy = energies[0];
    for (size_t i = 0; i < 5; ++i) {
        cumulative += probabilities[i];
        if (rand <= cumulative) {
            selectedEnergy = energies[i];
            break;
        }
    }
    
    fParticleGun->SetParticleEnergy(selectedEnergy);

     // Salvar energia inicial para EventAction
    auto eventAction = (EventAction*) G4RunManager::GetRunManager()->GetUserEventAction();
    if (eventAction) {
        eventAction->SetInitialAlphaEnergy(selectedEnergy);
    } 

    // Definir a posição da partícula dentro do disco de Am-241
    G4double R = 0.5*cm; // Raio do disco de Am-241
    G4double d =  0.1 * cm; // Espessura do disco (muito fina)
    G4double r = R * std::sqrt(G4UniformRand());
    G4double theta = 2 * M_PI * G4UniformRand();
    G4double x = r * std::cos(theta);
    G4double y = r * std::sin(theta);
    // posição inicial da partícula exatamente na superfície inferior do disco
    G4double z = -d;  // Coloca a partícula imediatamente abaixo do disco

    fParticleGun->SetParticlePosition(G4ThreeVector(x, y, z));

    // Direção isotrópica apenas no hemisfério inferior
    G4double phi = 2 * M_PI * G4UniformRand();
    G4double costheta = -G4UniformRand(); // entre 0 e -1
    G4double sintheta = std::sqrt(1 - costheta * costheta);
    G4double ux = sintheta * std::cos(phi);
    G4double uy = sintheta * std::sin(phi);
    G4double uz = costheta;
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(ux, uy, uz));

    // Disparar partícula
    fParticleGun->GeneratePrimaryVertex(anEvent);
}
