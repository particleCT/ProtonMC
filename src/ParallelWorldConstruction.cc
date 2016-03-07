#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4Transform3D.hh"
#include "G4RotationMatrix.hh"
#include "G4PVPlacement.hh"
#include "PrimaryGeneratorAction.hh"
#include "SensitiveDetector.hh"
#include "DetectorConstruction.hh"
#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4NistManager.hh"
#include "Riostream.h"
#include "ParallelWorldConstruction.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"
using namespace CLHEP;
ParallelWorldConstruction::ParallelWorldConstruction(G4String& worldName) :G4VUserParallelWorld(worldName) 
{
  theDetector  = DetectorConstruction::GetInstance();
;}

ParallelWorldConstruction::~ParallelWorldConstruction() {;}

void ParallelWorldConstruction::Construct() {
  /*
  G4VPhysicalVolume* ghostWorld = GetWorld();
  //Radiographic detector

  SensitiveDetector* sd1               = new SensitiveDetector("sd1");
  G4Box* rad_vol1                      = new G4Box("rad_vol1",1*mm,theDetector->PhantomHalfY,theDetector->PhantomHalfZ);
  G4LogicalVolume * rad_log1           = new G4LogicalVolume(rad_vol1,theDetector->water,"rad_log1",0,0,0);
  G4VisAttributes* sd_att = new G4VisAttributes(G4Colour(0,1,1));
  sd_att->SetVisibility(true);
  rad_log1->SetVisAttributes(sd_att);
  rad_log1->SetSensitiveDetector(sd1);
  new G4PVPlacement(0,G4ThreeVector(-1*theDetector->PhantomHalfX + 1*mm,0,0),"rad_phys1",rad_log1,ghostWorld,false,0);

  SensitiveDetector* sd2               = new SensitiveDetector("sd2");
  G4Box* rad_vol2                      = new G4Box("rad_vol2",1*mm,theDetector->PhantomHalfY,theDetector->PhantomHalfZ);
  G4LogicalVolume * rad_log2           = new G4LogicalVolume(rad_vol2,theDetector->water,"rad_log2",0,0,0);
  rad_log2->SetVisAttributes(sd_att);
  rad_log2->SetSensitiveDetector(sd2);
  new G4PVPlacement(0,G4ThreeVector(theDetector->PhantomHalfX - 1*mm,0,0),"rad_phys2",rad_log2,ghostWorld,false,0);
*/
}
