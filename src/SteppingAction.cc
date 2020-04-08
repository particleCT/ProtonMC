#include <vector>
#include <iostream>
#include <iomanip>
#include "Analysis.hh"
#include "G4EventManager.hh"
#include "G4EnergyLossTables.hh"
#include "G4TrackVector.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include "globals.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4FastStep.hh"
#include "G4ParticleChange.hh"
#include "G4DynamicParticle.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4SteppingManager.hh"
#include "G4EventManager.hh"
#include "G4ios.hh"
#include "iomanip"
#include "G4UImanager.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "SteppingAction.hh"
#include "G4RunManager.hh"
class DetectorConstruction;
class EventAction;
SteppingAction *SteppingAction::theSteppingAction=NULL;
SteppingAction::~SteppingAction()
{theSteppingAction=NULL;}
SteppingAction::SteppingAction()
{
  theAnalysis  = Analysis::GetInstance();
  theGenerator = PrimaryGeneratorAction::GetInstance();
  theSteppingAction=this;
}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
  
  G4Track *tr = aStep->GetTrack();
  G4ThreeVector preStepPos = aStep->GetPreStepPoint()->GetPosition();
  // Primary in the center
  if(abs(preStepPos.x()*mm)<1.0 && aStep->GetTrack()->GetTrackID()==1 && theGenerator->MiddleAlive==0){
    theGenerator->MiddleAlive=1;
    NPrimMiddle++;
  }


  if(tr->GetTrackID()==1){
    temp_X.push_back(preStepPos.x());
    temp_Y.push_back(preStepPos.y());
    temp_Z.push_back(preStepPos.z());
    temp_E.push_back(tr->GetKineticEnergy());
    temp_Radlen.push_back(tr->GetMaterial()->GetRadlen());
    temp_name.push_back(aStep->GetPreStepPoint()->GetMaterial()->GetName());
  }
  /*
  if (tr->GetNextVolume()->GetName()=="physWorld" || tr->GetTrackID()!=1 ) {
      temp_X.clear();
      temp_Y.clear();
      temp_Z.clear();
      temp_E.clear();
      temp_Radlen.clear();
      temp_name.clear(); 
      tr->SetTrackStatus(fStopAndKill);}
  */
}


