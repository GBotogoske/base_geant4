#include "DetectorConstruction.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4MaterialPropertiesTable.hh"

#include <utility> 

DetectorConstruction::DetectorConstruction() {}
DetectorConstruction::~DetectorConstruction() {}

auto DetectorConstruction::build_world()
{
    G4NistManager* nist = G4NistManager::Instance();
    // Criar volume externo (ar atmosférico)
    G4double worldSize = 50*cm;
    G4Box* solidWorld = new G4Box("World", worldSize, worldSize, worldSize);
    G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, nist->FindOrBuildMaterial("G4_AIR"), "World");
    G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0);

    return std::make_pair(logicWorld, physWorld);
}

auto DetectorConstruction::build_cryostat(G4LogicalVolume* logicWorld)
{
    G4NistManager* nist = G4NistManager::Instance();
    fMetalMaterial = nist->FindOrBuildMaterial("G4_STAINLESS-STEEL");
    // Espessura da caixa metálica
    G4double metalThickness = 1*cm;
    G4double metalSize = 20*cm; // Tamanho externo da caixa metálica
    
    // Criar a caixa metálica (volume metálico com espessura)
    G4Box* solidMetal = new G4Box("MetalBlock", metalSize/2, metalSize/2, metalSize/2);
    G4LogicalVolume* logicMetal = new G4LogicalVolume(solidMetal, fMetalMaterial, "MetalBlock");
    // Colocando a caixa metálica dentro do volume de ar
    G4VPhysicalVolume* physMetal = new G4PVPlacement(0, G4ThreeVector(), logicMetal, "MetalBlock", logicWorld, false, 0);
    return std::make_pair(logicMetal, physMetal);
}

auto DetectorConstruction::fill_cryostat(G4LogicalVolume* logicMetal)
{
    G4NistManager* nist = G4NistManager::Instance();
    // Definir materiais
    fLArMaterial = nist->FindOrBuildMaterial("G4_lAr");

    // Propriedades ópticas do Argônio Líquido (não alterado)
    G4MaterialPropertiesTable* larMPT = new G4MaterialPropertiesTable();
    const G4int numEntriesLAr = 2;
    G4double ScintEnergyLAr[numEntriesLAr] = {9.60*eV, 9.80*eV};
    G4double ScintSpectrumLAr[numEntriesLAr] = {1.0, 1.0};
    larMPT->AddProperty("SCINTILLATIONCOMPONENT1", ScintEnergyLAr, ScintSpectrumLAr, numEntriesLAr);
    larMPT->AddProperty("SCINTILLATIONCOMPONENT2", ScintEnergyLAr, ScintSpectrumLAr, numEntriesLAr);
    larMPT->AddConstProperty("SCINTILLATIONYIELD", 40000.0 / MeV);
    larMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT1", 6.0*ns);
    larMPT->AddConstProperty("SCINTILLATIONTIMECONSTANT2", 1.6*us);
    larMPT->AddConstProperty("SCINTILLATIONYIELD1", 0.3);
    larMPT->AddConstProperty("SCINTILLATIONYIELD2", 0.7);
    larMPT->AddConstProperty("RESOLUTIONSCALE", 1.);
    
    G4double rindexLAr[numEntriesLAr] = {1.23, 1.23};
    larMPT->AddProperty("RINDEX", ScintEnergyLAr, rindexLAr, numEntriesLAr);
    G4double absorptionLAr[numEntriesLAr] = {50.*m, 50.*m};
    larMPT->AddProperty("ABSLENGTH", ScintEnergyLAr, absorptionLAr, numEntriesLAr);
    G4double rayleighLAr[numEntriesLAr] = {0.9*m, 0.9*m};
    larMPT->AddProperty("RAYLEIGH", ScintEnergyLAr, rayleighLAr, numEntriesLAr);
    //larMPT->AddConstProperty("BIRKS_CONSTANT", 0.0486 * mm/MeV , true);
    fLArMaterial->SetMaterialPropertiesTable(larMPT);


    G4double metalThickness = 1*cm;
    G4double metalSize = 20*cm; // Tamanho externo da caixa metálica
    G4double larSize = metalSize - 2 * metalThickness; // Tamanho interno de argônio líquido

    // Volume de argônio líquido (dentro da caixa metálica)
    G4Box* solidLAr = new G4Box("LArVolume", larSize/2, larSize/2, larSize/2);
    G4LogicalVolume* logicLAr = new G4LogicalVolume(solidLAr, fLArMaterial, "LArVolume");
    G4VPhysicalVolume* physLAr = new G4PVPlacement(0, G4ThreeVector(), logicLAr, "LArVolume", logicMetal, false, 0);

    return std::make_pair(logicLAr, physLAr);


}


G4VPhysicalVolume* DetectorConstruction::Construct() {
    // Obter materiais do NIST
    G4NistManager* nist = G4NistManager::Instance();
    
    fAlphaSource = nist->FindOrBuildMaterial("G4_Am");

    // Definir TPB (material customizado)
    G4Element* elC = nist->FindOrBuildElement("C");
    G4Element* elH = nist->FindOrBuildElement("H");
    fTPBCoating = new G4Material("TPB", 1.0*g/cm3, 2);
    fTPBCoating->AddElement(elC, 28);
    fTPBCoating->AddElement(elH, 22);

    // Criar volume externo (ar atmosférico)
    auto [logicWorld, physWorld] = build_world();
    //Criar criostato
    auto [logicMetal, physMetal] = build_cryostat(logicWorld);
    //Encher criostato
    auto [logicLAr, physLar] = fill_cryostat(logicMetal);

    // Espessura da caixa metálica
    G4double metalThickness = 1*cm;
    G4double metalSize = 20*cm; // Tamanho externo da caixa metálica
    G4double larSize = metalSize - 2 * metalThickness; // Tamanho interno de argônio líquido

    // Espessura da camada de TPB
    G4double tpbThickness = 1*um;  // espessura típica muito fina (1 micrômetro)

    // Criar a camada fina de TPB na superfície interna da caixa metálica
    G4double tpbSize = larSize + 2*tpbThickness; // Ligeiramente maior que o volume de LAr
    G4Box* solidTPB = new G4Box("TPBCoating", tpbSize/2, tpbSize/2, tpbSize/2);
    G4LogicalVolume* logicTPB = new G4LogicalVolume(solidTPB, fTPBCoating, "TPBCoating");


    // Posiciona o volume de TPB dentro do volume metálico
    G4VPhysicalVolume* physTPB = new G4PVPlacement(0, G4ThreeVector(), logicTPB, "TPBCoating", logicMetal, false, 0);

    // Agora, o argônio líquido vai dentro da camada de TPB

    // Fonte de Amerício-241 no centro
    G4double sourceRadius = 0.5*cm;
    G4double sourceThickness = 0.1*cm;
    G4Tubs* solidSource = new G4Tubs("AlphaSource", 0, sourceRadius, sourceThickness/2, 0, 360*deg);
    G4LogicalVolume* logicSource = new G4LogicalVolume(solidSource, fAlphaSource, "AlphaSource");
    new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicSource, "AlphaSource", logicLAr, false, 0);

    // Janela óptica (ARAPUCA)
    fArapucaMaterial = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");
    fArapucaHeight = 2.0*cm;
    G4double arapucaSize = metalSize / 4;
    G4Box* solidArapuca = new G4Box("Arapuca", arapucaSize/2, arapucaSize/2, 0.5*cm);
    G4LogicalVolume* logicArapuca = new G4LogicalVolume(solidArapuca, fArapucaMaterial, "ArapucaVolume");
    G4ThreeVector arapucaPos = G4ThreeVector(0, 0, -metalSize/2 + fArapucaHeight);
    new G4PVPlacement(0, arapucaPos, logicArapuca, "Arapuca", logicLAr, false, 0);


    // Propriedades ópticas do TPB (não alterado)
    G4MaterialPropertiesTable* tpbMPT = new G4MaterialPropertiesTable();

    const G4int numEntriesTPB = 5;
    G4double PhotonEnergyTPB[numEntriesTPB] = {2.5*eV, 3.0*eV, 7.0*eV, 9.0*eV, 10.0*eV};
    G4double AbsorptionTPB[numEntriesTPB]   = {1.*m, 1.*m, 1.*m, 1.*m, 1.*m};  // quase sem absorção normal
    G4double WLSAbsorptionTPB[numEntriesTPB] = {1000.*um, 500.*um, 0.01*um, 0.01*um, 0.01*um}; // absorção forte para conversão
 
    G4double EfficiencyTPB[numEntriesTPB]   = {0.0, 0.0, 0.7, 0.7, 0.7};
    const G4int numEntriesEmissionTPB = 8;
    G4double EmissionEnergyTPB[numEntriesEmissionTPB] = {2.0*eV, 2.2*eV, 2.4*eV, 2.6*eV, 2.8*eV, 3.0*eV, 3.1*eV, 3.2*eV};
    G4double EmissionSpectrumTPB[numEntriesEmissionTPB] = {0.0, 0.05, 0.3, 0.6, 1.0, 0.3, 0.05, 0.0};

    // Atualizando o MaterialPropertiesTable do TPB
    tpbMPT->AddProperty("WLSCOMPONENT", EmissionEnergyTPB, EmissionSpectrumTPB, numEntriesEmissionTPB);
    tpbMPT->AddProperty("ABSLENGTH", PhotonEnergyTPB, AbsorptionTPB, numEntriesTPB);
    tpbMPT->AddProperty("WLSABSLENGTH", PhotonEnergyTPB, WLSAbsorptionTPB, numEntriesTPB);
    tpbMPT->AddProperty("EFFICIENCY", PhotonEnergyTPB, EfficiencyTPB, numEntriesTPB);
    tpbMPT->AddConstProperty("WLSTIMECONSTANT", 1.8*ns);

    const G4int numEntriesTPB_RINDEX = 2;
    G4double PhotonEnergyRINDEX[numEntriesTPB_RINDEX] = {2.0*eV, 10.0*eV};
    G4double RindexTPB[numEntriesTPB_RINDEX] = {1.67, 1.67};  // Valor típico para TPB

    tpbMPT->AddProperty("RINDEX", PhotonEnergyRINDEX, RindexTPB, numEntriesTPB_RINDEX);
  
    fTPBCoating->SetMaterialPropertiesTable(tpbMPT);

 
    // Propriedades ópticas do Argônio Líquido (não alterado)
    const G4int numEntriesLAr = 2;
    G4double ScintEnergyLAr[numEntriesLAr] = {9.60*eV, 9.80*eV};
    G4double ScintSpectrumLAr[numEntriesLAr] = {1.0, 1.0};
    G4double rindexLAr[numEntriesLAr] = {1.23, 1.23};
    G4double absorptionLAr[numEntriesLAr] = {50.*m, 50.*m};
    G4double rayleighLAr[numEntriesLAr] = {0.9*m, 0.9*m};

    G4MaterialPropertiesTable* fARAPUCA = new G4MaterialPropertiesTable();
    fARAPUCA->AddProperty("RINDEX", ScintEnergyLAr, rindexLAr, numEntriesLAr); // coloquei igual do argonio para o photon entrar sem problema ( ser transparente)
    G4double AbsorptionArapuca[2]   = {1e-18*m, 1e-18*m};  
    G4double AbsorptionArapuca_x[2]   = {1 ,1};  
    fARAPUCA->AddProperty("ABSLENGTH", AbsorptionArapuca_x, AbsorptionArapuca, 2); //do tpb ta bom, que vai ser absorvido rapido
    fArapucaMaterial->SetMaterialPropertiesTable(fARAPUCA);

    // Superfície absorvente
    G4OpticalSurface* metalSurface = new G4OpticalSurface("MetalSurface");
    metalSurface->SetType(dielectric_metal);
    metalSurface->SetModel(unified);
    metalSurface->SetFinish(polished);

    G4MaterialPropertiesTable* metalMPT = new G4MaterialPropertiesTable();
    const G4int numEntriesMetal = 2;
    G4double photonEnergyMetal[numEntriesMetal] = {2.0*eV, 10.0*eV};
    G4double reflectivityMetal[numEntriesMetal] = {0.0, 0.0};
    metalMPT->AddProperty("REFLECTIVITY", photonEnergyMetal, reflectivityMetal, numEntriesMetal);
    metalSurface->SetMaterialPropertiesTable(metalMPT);

    // Definindo a superfície no sentido TPB → Metal
    new G4LogicalBorderSurface("TPB_to_Metal_Surface", physTPB, physMetal, metalSurface);

    // Definindo a superfície no sentido Metal → TPB
    new G4LogicalBorderSurface("Metal_to_TPB_Surface", physMetal, physTPB, metalSurface);


    physWorld->CheckOverlaps();
    return physWorld;
}
