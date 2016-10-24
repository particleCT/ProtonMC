#include "DetectorConstruction.hh"
#include "G4EmCalculator.hh"
#include "G4NistManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4UImanager.hh"
#include "globals.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include "G4IonTable.hh"
#include <math.h>
#include "G4NavigationHistory.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4TransportationManager.hh"
#include "TFile.h"
#include "TTree.h"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "PrimaryGeneratorAction.hh"
using namespace std;

G4String PrimaryGeneratorAction::thePrimaryParticleName="proton";
PrimaryGeneratorAction* PrimaryGeneratorAction::theGenerator = NULL;

PrimaryGeneratorAction::PrimaryGeneratorAction(G4double E_0,G4int Anumber):A(Anumber)
{ 

  theDetector = DetectorConstruction::GetInstance();
  ENER = E_0;
  nProtonsGenerated = 0;
  theGenerator = this;
  particleGun = new G4ParticleGun(1);

  inP=0;  
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
  

  if(A==1){
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    particle = particleTable->FindParticle("proton");
    Einit = ENER*MeV;
  }
  else{
    particle = G4IonTable::GetIonTable()->GetIon(A/2,A,0);
    Einit = ENER*(A)*MeV;
  }
  // Parallel
  x0 = -1*theDetector->PhantomHalfX -0.1*mm; //Delta epsilon to hit all detectors
  y0 = G4UniformRand()*fieldSizeY-fieldSizeY/2;//G4RandGauss::shoot(0,8*mm);
  z0 = G4UniformRand()*fieldSizeZ-fieldSizeZ/2;//G4RandGauss::shoot(0,8*mm);
  px0 = 1; 
  py0 = 0; 
  pz0 = 0;

  // Conical
  /*phi   = G4UniformRand()*2*pi;//(1./6.)*pi*(G4UniformRand()-0.5);
  theta = (1./20.)*pi*(G4UniformRand()-0.5);
  px0 = std::cos(theta);
  py0 = std::sin(theta)*std::cos(phi);
  pz0 = std::sin(theta)*std::sin(phi);

  x0 = -1*theDetector->PhantomHalfX -2*m;
  y0 = 0;
  z0 = 0;*/

  Position = G4ThreeVector(x0,y0,z0);
  Momentum = G4ThreeVector(px0,py0,pz0);

  particleGun->SetParticleDefinition(particle);
  particleGun->SetParticleEnergy(Einit);
  particleGun->SetParticleMomentumDirection(Momentum);
  particleGun->SetParticlePosition(Position);
  particleGun->GeneratePrimaryVertex(anEvent);
  nProtonsGenerated++;
  if(nProtonsGenerated%20000==0) cout << nProtonsGenerated << endl;
}


vector<G4double> PrimaryGeneratorAction::linspace(double a, double b, double step) {
  vector<double> array;
  while(a <= b) {
    array.push_back(a);
    a += step;           
  }
  return array;
}
