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
}

ParallelWorldConstruction::~ParallelWorldConstruction() {}
void ParallelWorldConstruction::Construct() {
  //G4VPhysicalVolume* ghostWorld = GetWorld();
  //Radiographic detector
  /*SensitiveDetector* sd3               = new SensitiveDetector("sd3");
  G4Box* rad_vol3                      = new G4Box("rad_vol3",1*mm,theDetector->PhantomHalfY,theDetector->PhantomHalfZ);
  G4LogicalVolume * rad_log3           = new G4LogicalVolume(rad_vol3,theDetector->water,"rad_log3",0,0,0);
  G4VisAttributes* sd_att = new G4VisAttributes(G4Colour(0,3,1));
  sd_att->SetVisibility(true);
  rad_log3->SetVisAttributes(sd_att);
  rad_log3->SetSensitiveDetector(sd3);
  new G4PVPlacement(0,G4ThreeVector(0,0,0),"rad_phys3",rad_log3,ghostWorld,false,0);*/
}
