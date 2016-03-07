#include "vector"
#include "G4ios.hh"
#include "TTree.h"
#include "TFile.h"
#include "TH3D.h"
#include "globals.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4ThreeVector.hh"
#include "G4Material.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Analysis.hh"

Analysis* Analysis::theAnalysis=NULL;
Analysis::~Analysis(){theAnalysis=NULL;}

Analysis::Analysis(G4int thread, G4int angle,G4String theName){
  theAnalysis  = this;

  theGenerator = PrimaryGeneratorAction::GetInstance();
  theDetector  = DetectorConstruction::GetInstance();
  f1 = new TFile(Form("%s_%.0f_%d_%d.root",theName.data(),theGenerator->ENER,angle,thread),"recreate");

  t = new TTree("phase","PS");

  t->Branch("x0",&x0,"x0/D");
  t->Branch("y0",&y0,"y0/D");
  t->Branch("z0",&z0,"z0/D");
  
  t->Branch("px0",&px0,"px0/D");
  t->Branch("py0",&py0,"py0/D");
  t->Branch("pz0",&pz0,"pz0/D");
  t->Branch("Einit",&Einit,"Einit/D");
  
  /*t->Branch("tracks_X",&tracks_X);
  t->Branch("tracks_Y",&tracks_Y);
  t->Branch("tracks_Z",&tracks_Z);
  t->Branch("mat_name",&mat_name);*/

  t->Branch("x1",&x1,"x1/D");
  t->Branch("y1",&y1,"y1/D");
  t->Branch("z1",&z1,"z1/D");
  
  t->Branch("px1",&px1,"px1/D");
  t->Branch("py1",&py1,"py1/D");
  t->Branch("pz1",&pz1,"pz1/D");

  t->Branch("Estop",&Estop,"Estop/D");
  t->Branch("Id",&Id,"Id/I");
}


void Analysis::analyseHit(G4Step* aStep, G4String theName)
{
  
  theSteppingAction = SteppingAction::GetInstance();
  f1->cd();

  if(theName=="sd1"){
    x0  = aStep->GetPostStepPoint()->GetPosition()[0];
    y0  = aStep->GetPostStepPoint()->GetPosition()[1];
    z0  = aStep->GetPostStepPoint()->GetPosition()[2];

    px0 = aStep->GetPostStepPoint()->GetMomentumDirection()[0];
    py0 = aStep->GetPostStepPoint()->GetMomentumDirection()[1];
    pz0 = aStep->GetPostStepPoint()->GetMomentumDirection()[2];
    Einit = aStep->GetPostStepPoint()->GetKineticEnergy();

    /*theSteppingAction->temp_name.clear();
    theSteppingAction->temp_X.clear();
    theSteppingAction->temp_Y.clear();
    theSteppingAction->temp_Z.clear();*/

  }
  
  else if(theName=="sd2"){
    x1  = aStep->GetPostStepPoint()->GetPosition()[0];
    y1  = aStep->GetPostStepPoint()->GetPosition()[1];
    z1  = aStep->GetPostStepPoint()->GetPosition()[2];
    
    px1 = aStep->GetPostStepPoint()->GetMomentumDirection()[0];
    py1 = aStep->GetPostStepPoint()->GetMomentumDirection()[1];
    pz1 = aStep->GetPostStepPoint()->GetMomentumDirection()[2];
    
    /*mat_name = &(theSteppingAction->temp_name);
    tracks_X = &(theSteppingAction->temp_X);
    tracks_Y = &(theSteppingAction->temp_Y);
    tracks_Z = &(theSteppingAction->temp_Z);

    tracks_X->push_back(x1);
    tracks_Y->push_back(y1);
    tracks_Z->push_back(z1);
    mat_name->push_back( aStep->GetPreStepPoint()->GetMaterial()->GetName().data() );*/

    Id    = aStep->GetTrack()->GetTrackID();
    Estop = aStep->GetPostStepPoint()->GetKineticEnergy();
    Einit = theGenerator->ENER;
    t->Fill();

    /*theSteppingAction->temp_name.clear();
    theSteppingAction->temp_X.clear();
    theSteppingAction->temp_Y.clear();
    theSteppingAction->temp_Z.clear();*/

  }


}


void Analysis::Save(){
  f1->cd();
  theGenerator = PrimaryGeneratorAction::GetInstance();
  t->Write("",TObject::kOverwrite);
  f1->Close();
}