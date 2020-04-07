#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4ParticleDefinition.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "TTree.h"
#include "G4ThreeVector.hh"
using namespace std;
class G4ParticleGun;
class G4Event;
class DetectorConstruction;
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:

  PrimaryGeneratorAction(G4double,G4int);
  ~PrimaryGeneratorAction();
  G4double ENER, ESPR, ANGU_X, ANGU_Y, CORR_X, CORR_Y, SPOT_CX, SPOT_CY, SPOT_CZ, SPOT_X, SPOT_Y, SPOT_Z, RAD;

  vector<G4double> linspace(double , double , double );
  void GeneratePrimaries(G4Event* );
  static G4String GetPrimaryName() ;                
  static inline PrimaryGeneratorAction* GetInstance() { return theGenerator; }

  Double_t x0,y0,z0,px0,py0,pz0;
  Double_t x1,y1,z1,px1,py1,pz1;
  Double_t E0,Estop;
  Int_t   Id;
  G4int Nprotons = 0 ;
  Float_t x,y,z,theta,phi,Einit;
  G4ThreeVector Position;
  G4ThreeVector Momentum;
  G4int nProtonsGenerated;   
  G4double IrradiatedEnergy; 
  G4ParticleDefinition* particle;
  G4int A;
  vector<G4double> beamPosZ;
  vector<G4double> beamPosY;
  bool MiddleAlive;
  private:
  static PrimaryGeneratorAction* theGenerator;
  G4ParticleGun*  	       particleGun;  //pointer a to G4 service class
  DetectorConstruction* theDetector;  
  G4double Z_Position;
  G4String PSD_Path;
  G4String PSD_Name;
  G4String PARTICLE;
  G4double fieldSizeZ,fieldSizeY;
};

#endif



