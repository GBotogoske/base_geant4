#ifndef DETECTORCONSTRUCTION_HH
#define DETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Material.hh"

class DetectorConstruction : public G4VUserDetectorConstruction {

    public:
        DetectorConstruction();
        ~DetectorConstruction();

        virtual G4VPhysicalVolume* Construct();

    private:
        G4Material* fLArMaterial;
        G4Material* fMetalMaterial;
        G4Material* fTPBCoating;
        G4Material* fAlphaSource;
        G4Material* fArapucaMaterial; // Material óptico da janela
        G4double fArapucaHeight;  // Altura da ARAPUCA em relação à base
        
};

#endif
