#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "OrganicMaterial.hh"
class G4VPhysicalVolume;
class G4LogicalVolume;
class OrganicMaterial;




class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction(G4String,G4double,G4double);
  ~DetectorConstruction();
  virtual G4VPhysicalVolume* Construct();  
  static inline DetectorConstruction* GetInstance() { return theDetector; }
  G4String thePhantom;
  G4double theThickness;
  OrganicMaterial* theMaterial;

  G4int theAngle;
  G4double PhantomHalfX,PhantomHalfY,PhantomHalfZ;
  G4Material* water;
private:
  static DetectorConstruction* theDetector;

};


#endif

