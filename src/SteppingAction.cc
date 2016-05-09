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
#include "G4RunManager.hh"
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
class DetectorConstruction;
class EventAction;
SteppingAction *SteppingAction::theSteppingAction=NULL;
SteppingAction::~SteppingAction()
{theSteppingAction=NULL;}
SteppingAction::SteppingAction()
{
  theAnalysis = Analysis::GetInstance();
  theSteppingAction=this;
}

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{

  G4Track *tr = aStep->GetTrack();
  //if(tr->GetVolume()->GetName()=="box_phys" && tr->GetTrackID()!=1){
  if ( tr->GetTrackID()!=1){ 
    tr->SetTrackStatus(fStopAndKill);
    temp_X.clear();
    temp_Y.clear();
    temp_Z.clear();
    temp_name.clear(); } 
  

  /*if(tr->GetTrackID()==1){
    G4ThreeVector preStepPos = aStep->GetPreStepPoint()->GetPosition();
    temp_X.push_back(preStepPos.x());
    temp_Y.push_back(preStepPos.y());
    temp_Z.push_back(preStepPos.z());
    temp_name.push_back(aStep->GetPreStepPoint()->GetMaterial()->GetName());
    }*/


}


