#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4ios.hh"
#include "TTree.h"


#include <sstream>
#include <string>
#include <vector>
#include <iostream>//
#include <fstream>//
using namespace std;

class G4ParticleGun;
class G4Event;
class DetectorConstruction;
class TFile;
class TTree;
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:

  PrimaryGeneratorAction(G4double);
  ~PrimaryGeneratorAction();
  G4double ENER, ESPR, ANGU_X, ANGU_Y, CORR_X, CORR_Y, SPOT_CX, SPOT_CY, SPOT_CZ, SPOT_X, SPOT_Y, SPOT_Z, RAD;   
  TFile *f;
  TTree *t;


  vector<G4double> linspace(double , double , double );
  void GeneratePrimaries(G4Event* );
  static G4String GetPrimaryName() ;                
  static inline PrimaryGeneratorAction* GetInstance() { return theGenerator; }

  std::vector<double>* tracks_X;
  std::vector<double>* tracks_Y;
  std::vector<double>* tracks_Z;

  std::vector<G4double> beamPosZ;
  std::vector<G4double> beamPosY;

  Double_t x0,y0,z0,px0,py0,pz0;
  Double_t x1,y1,z1,px1,py1,pz1;
  Double_t E0,Estop;
  Int_t   Id;
  G4int nProtonsGenerated;
  Double_t x,y,z,theta,phi,Einit;
  G4ThreeVector Position, Momentum;

  private:
  static PrimaryGeneratorAction* theGenerator;
  G4ParticleGun*  	       particleGun;  //pointer a to G4 service class
  DetectorConstruction* theDetector;  
  G4double Z_Position;
  G4String PSD_Path;
  G4String PSD_Name;
  G4String PARTICLE;
  G4double fieldSizeZ,fieldSizeY;
  
  std::ifstream *inP;
  
  static G4String thePrimaryParticleName;
};

#endif



