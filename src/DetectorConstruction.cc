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
#include "G4SubtractionSolid.hh"

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
    G4double rayleighLAr[numEntriesLAr] = {1.25*m, 1.25*m};
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

auto DetectorConstruction::place_source(G4LogicalVolume* logicLAr)
{
    G4NistManager* nist = G4NistManager::Instance();
    
    fAlphaSource = nist->FindOrBuildMaterial("G4_Am");
    // Fonte de Amerício-241 no centro
    G4double sourceRadius = 0.5*cm;
    G4double sourceThickness = 0.1*cm;
    G4Tubs* solidSource = new G4Tubs("AlphaSource", 0, sourceRadius, sourceThickness/2, 0, 360*deg);
    G4LogicalVolume* logicSource = new G4LogicalVolume(solidSource, fAlphaSource, "AlphaSource");
    auto physSource = new G4PVPlacement(0, G4ThreeVector(0, 0, 0), logicSource, "AlphaSource", logicLAr, false, 0);

    return std::make_pair(logicSource, physSource);

}

auto DetectorConstruction::build_arapuca(G4LogicalVolume* logicLAr)
{
    G4NistManager* nist = G4NistManager::Instance();
    // Espessura da caixa metálica
    G4double metalThickness = 1*cm;
    G4double metalSize = 20*cm; // Tamanho externo da caixa metálica
    G4double larSize = metalSize - 2 * metalThickness; // Tamanho interno de argônio líquido

    // Propriedades ópticas do Argônio Líquido (não alterado)
    const G4int numEntriesLAr = 2;
    G4double ScintEnergyLAr[numEntriesLAr] = {9.60*eV, 9.80*eV};
    G4double rindexLAr[numEntriesLAr] = {5, 5};
   
    //  propriedades da Janela óptica (filtro dicroico) (ARAPUCA)
    fArapucafilterMaterial = nist->FindOrBuildMaterial("G4_SILICON_DIOXIDE");

    G4MaterialPropertiesTable* fARAPUCA = new G4MaterialPropertiesTable();
    fARAPUCA->AddProperty("RINDEX", ScintEnergyLAr, rindexLAr, numEntriesLAr); // coloquei igual do argonio para o photon entrar sem problema ( ser transparente)
    G4double AbsorptionArapuca[2]   = {1e-18*m, 1e-18*m};  
    G4double AbsorptionArapuca_x[2]   = {1 ,1};  
    fARAPUCA->AddProperty("ABSLENGTH", AbsorptionArapuca_x, AbsorptionArapuca, 2); //do tpb ta bom, que vai ser absorvido rapido
    fArapucafilterMaterial->SetMaterialPropertiesTable(fARAPUCA);

    //propriedades da carcaça
    auto fplasticMaterial = nist->FindOrBuildMaterial("G4_POLYVINYL_CHLORIDE"); // ou outro material plástico

    //propriedades to PTP
    const G4int NUM = 4;
    G4double photonEnergy[NUM] = {3.35*eV, 3.6*eV, 4.3*eV, 5.0*eV}; // Energias de 240 nm (5.17 eV) até 370 nm (3.35 eV)
    G4double rIndexPTP[NUM] = {1.65, 1.65, 1.65, 1.65};  // Índice de refração típico do PTP
    G4double absLength[NUM] = {10*m, 10*m, 0.1*mm,  0.05*mm }; // Absorção: transparente no visível, absorve bem no UV
    G4double wlsAbsLength[NUM] = {10*m,10*m, 0.1*mm, 0.05*mm}; // WLS absorção — controla onde ele pega o fóton VUV
    G4double wlsEmission[NUM] = { 3.35*eV, 3.45*eV, 3.55*eV, 3.60*eV};     // Emissão espectral do PTP (~340–360 nm)
    G4double efficiency[NUM] = {0.9, 0.9, 0.9, 0.9};    // Eficiência quântica da conversão
    
    // Cria a tabela de propriedades
    G4MaterialPropertiesTable* MPT_PTP = new G4MaterialPropertiesTable();
    MPT_PTP->AddProperty("RINDEX", photonEnergy, rIndexPTP, NUM);
    MPT_PTP->AddProperty("ABSLENGTH", photonEnergy, absLength, NUM);
    MPT_PTP->AddProperty("WLSABSLENGTH", photonEnergy, wlsAbsLength, NUM);
    MPT_PTP->AddProperty("WLSCOMPONENT", photonEnergy, wlsEmission, NUM);
    MPT_PTP->AddProperty("EFFICIENCY", photonEnergy, efficiency, NUM);
    MPT_PTP->AddConstProperty("WLSTIMECONSTANT", 1.0*ns); // tempo rápido
    // Define o material PTP
    G4Material* PTP = new G4Material("PTP", 1.23*g/cm3, 2);
    // Define os elementos carbono e hidrogênio
    G4Element* elC = new G4Element("Carbon",  "C", 6., 12.01*g/mole);
    G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.008*g/mole);

    PTP->AddElement(elC, 20);
    PTP->AddElement(elH, 14);
    PTP->SetMaterialPropertiesTable(MPT_PTP);

    //constroi o filtro
    fArapucaHeight = 6.0*cm;
    G4double arapuca_open = metalSize / 4;
    G4double arapucaBoxSize = metalSize / 3;
    G4double arapucaBox_thickness = 1*cm ;
    G4double filter_thickness = 0.4*cm ;
    G4double ptp_thickness = 0.05*cm ;
    G4Box* solidArapucaFilter = new G4Box("Arapuca_Filter", arapuca_open/2, arapuca_open/2, filter_thickness/2);
    G4LogicalVolume* logicArapucaFilter = new G4LogicalVolume(solidArapucaFilter, fArapucafilterMaterial, "ArapucaFilterVolume");
    G4ThreeVector arapucaPosFilter = G4ThreeVector(0, 0, -metalSize/2 + fArapucaHeight + arapucaBoxSize/2 - arapucaBox_thickness + filter_thickness/2);
    auto physArapucaFilter = new G4PVPlacement(0, arapucaPosFilter, logicArapucaFilter, "Arapuca_Filter", logicLAr, false, 0);
    
    //deposita pTp
    G4Box* solidPTPlayer = new G4Box("Arapuca_ptp", arapuca_open/2, arapuca_open/2, ptp_thickness/2);
    G4LogicalVolume* logicPTP = new G4LogicalVolume(solidPTPlayer, PTP, "PTP_Volume");
    G4ThreeVector ptpPos = G4ThreeVector(0, 0, -metalSize/2 + fArapucaHeight + arapucaBoxSize/2 - arapucaBox_thickness + filter_thickness + ptp_thickness/2);
    auto physPTP = new G4PVPlacement(0, ptpPos, logicPTP, "PTP_phys", logicLAr, false, 0);

    //constroi a caixa de plastico
    G4Box* externArapuca = new G4Box("ext_Arapuca", arapucaBoxSize/2, arapucaBoxSize/2, arapucaBoxSize/2);
    G4Box* internArapuca = new G4Box("int_Arapuca", arapucaBoxSize/2 - arapucaBox_thickness, arapucaBoxSize/2-arapucaBox_thickness, arapucaBoxSize/2-arapucaBox_thickness);
    G4SubtractionSolid* arapucaShell = new G4SubtractionSolid("ArapucaShell", externArapuca, internArapuca);
    G4Box* opening = new G4Box("open_Arapuca", arapuca_open/2, arapuca_open/2, arapucaBox_thickness/2 + 0.1*mm); // ligeiramente maior que a parede para garantir a subtração
    G4ThreeVector openingPos(0, 0, (arapucaBoxSize/2) - (arapucaBox_thickness/2));
    G4SubtractionSolid* arapucaSheelOpening = new G4SubtractionSolid("ArapucaShellOpening", arapucaShell, opening, nullptr, openingPos);
    G4LogicalVolume* logicArapucaSheel = new G4LogicalVolume(arapucaSheelOpening, fplasticMaterial, "ArapucaShellVolume");
    G4ThreeVector arapucaPos = G4ThreeVector(0, 0, -metalSize/2 + fArapucaHeight );
    auto physArapuca = new G4PVPlacement(0, arapucaPos, logicArapucaSheel, "Arapuca_Box", logicLAr, false, 0);


    return std::make_pair(logicArapucaFilter, physArapucaFilter);

}


G4VPhysicalVolume* DetectorConstruction::Construct() {
    // Obter materiais do NIST
    G4NistManager* nist = G4NistManager::Instance();
    
    // Criar volume externo (ar atmosférico)
    auto [logicWorld, physWorld] = build_world();
    //Criar criostato
    auto [logicMetal, physMetal] = build_cryostat(logicWorld);
    //Encher criostato
    auto [logicLAr, physLar] = fill_cryostat(logicMetal);
    //Colocar fonte
    auto [logicSource, physSource] = place_source(logicLAr);
    //Colocar Arapuca
    auto [logicArapuca, physArapuca] = build_arapuca(logicLAr);

    physWorld->CheckOverlaps();
    return physWorld;
}
