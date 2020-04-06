#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "G4EmCalculator.hh"
#include "G4NistManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UImanager.hh"
#include "G4Proton.hh"
#include "G4Alpha.hh"
#include "G4Deuteron.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include "G4Proton.hh"
#include "G4IonTable.hh"
#include <math.h>
#include "G4NavigationHistory.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"
#include "TFile.h"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
using namespace std;

PrimaryGeneratorAction* PrimaryGeneratorAction::theGenerator = NULL;

PrimaryGeneratorAction::PrimaryGeneratorAction(G4double energy, G4int ANumber):ENER(energy), A(ANumber)
{ 
  theGenerator = this;
  theDetector = DetectorConstruction::GetInstance();

  particleGun = new G4ParticleGun();
  nProtonsGenerated = 0;
  IrradiatedEnergy  = 0.;
  fieldSizeY = 2*(theDetector->PhantomHalfY);
  fieldSizeZ = 2*(theDetector->PhantomHalfZ);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  theGenerator = NULL;
  delete particleGun;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  if(A==1) particle = G4Proton::Proton();//G4IonTable::GetIonTable()->GetIon(1,1,0); // proton
  else if(A==2) particle = G4Deuteron::Deuteron();
  else if(A==4) particle = G4Alpha::Alpha();
  else particle = G4IonTable::GetIonTable()->GetIon(int(A/2),A,0); // rest

  particleGun->SetParticleDefinition(particle);
  Einit = ENER*A*MeV;

  /*G4double cosTheta = (1./5.)*(G4UniformRand()-0.5);
  phi = (1./16.)*(pi*G4UniformRand()-pi/2);
  G4double sinTheta = std::sqrt(1. - cosTheta*cosTheta);*/
  px0 = 1;//sinTheta*std::cos(phi);
  py0 = 0;//sinTheta*std::sin(phi);
  pz0 = 0;//cosTheta;

  x0 = -1*theDetector->PhantomHalfX -0.01*mm; 
  y0 = G4UniformRand()*fieldSizeY-fieldSizeY/2;
  z0 = G4UniformRand()*fieldSizeZ-fieldSizeZ/2; 

  Position = G4ThreeVector(x0,y0,z0); 
  Momentum = G4ThreeVector(px0,py0,pz0);

  particleGun->SetParticleEnergy(Einit);
  particleGun->SetParticleMomentumDirection(Momentum);
  particleGun->SetParticlePosition(Position);
  particleGun->GeneratePrimaryVertex(anEvent);
  IrradiatedEnergy += Einit;
  nProtonsGenerated++;
  if(nProtonsGenerated%20000==0) cout << nProtonsGenerated << endl;
}

