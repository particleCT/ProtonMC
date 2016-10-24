#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "OrganicMaterial.hh"
class G4VPhysicalVolume;
class G4LogicalVolume;
class OrganicMaterial;


#ifdef MIRD
class G4HumanPhantomMaterial;
#endif


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
#ifdef MIRD
  G4HumanPhantomMaterial* material;
  std::map<std::string,G4bool> sensitivities;
#endif
  static DetectorConstruction* theDetector;

};


#endif

