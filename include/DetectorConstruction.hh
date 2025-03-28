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
        auto build_world();
        auto build_cryostat(G4LogicalVolume* LogicWorld);
        auto fill_cryostat(G4LogicalVolume* LogicMetal);
        auto place_source(G4LogicalVolume* LogicLAr);
        auto build_arapuca(G4LogicalVolume* LogicLAr);

    private:
        G4Material* fLArMaterial;
        G4Material* fMetalMaterial;
        G4Material* fTPBCoating;
        G4Material* fAlphaSource;
        G4Material* fArapucafilterMaterial; // Material óptico da janela
        G4double fArapucaHeight;  // Altura da ARAPUCA em relação à base
        
};

#endif
