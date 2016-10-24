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
  f1 = new TFile(Form("%s_%.0f_%d_%d_%d.root",theName.data(),theGenerator->ENER,angle,thread,theGenerator->A),"recreate");

  t = new TTree("phase","PS");

  t->Branch("x0",&x0,"x0/D");
  t->Branch("y0",&y0,"y0/D");
  t->Branch("z0",&z0,"z0/D");
  
  t->Branch("px0",&px0,"px0/D");
  t->Branch("py0",&py0,"py0/D");
  t->Branch("pz0",&pz0,"pz0/D");
  t->Branch("Einit",&Einit,"Einit/D");
  
  t->Branch("tracks_X",&tracks_X);
  t->Branch("tracks_Y",&tracks_Y);
  t->Branch("tracks_Z",&tracks_Z);
  t->Branch("tracks_E",&tracks_E);
  /*t->Branch("Radlen",&Radlen);
  t->Branch("mat_name",&mat_name);*/

  t->Branch("x1",&x1,"x1/D");
  t->Branch("y1",&y1,"y1/D");
  t->Branch("z1",&z1,"z1/D");
  
  t->Branch("px1",&px1,"px1/D");
  t->Branch("py1",&py1,"py1/D");
  t->Branch("pz1",&pz1,"pz1/D");


  t->Branch("theta_y1",&theta_y1,"theta_y1/D");
  t->Branch("theta_z1",&theta_z1,"theta_z1/D");


  t->Branch("Estop",&Estop,"Estop/D");
  t->Branch("Id",&Id,"Id/I");
}


void Analysis::RearFrontDetector(G4Step* aStep, G4String theName)
{
  
  theSteppingAction = SteppingAction::GetInstance();
  f1->cd();

  if(theName=="FrontTracker"){

    x0  = aStep->GetPreStepPoint()->GetPosition()[0];
    y0  = aStep->GetPreStepPoint()->GetPosition()[1];
    z0  = aStep->GetPreStepPoint()->GetPosition()[2];

    px0 = aStep->GetPreStepPoint()->GetMomentumDirection()[0];
    py0 = aStep->GetPreStepPoint()->GetMomentumDirection()[1];
    pz0 = aStep->GetPreStepPoint()->GetMomentumDirection()[2];
    Einit = theGenerator->Einit;//  aStep->GetPreStepPoint()->GetKineticEnergy();

    theSteppingAction->temp_name.clear(); // clear before starting
    theSteppingAction->temp_X.clear();
    theSteppingAction->temp_Y.clear();
    theSteppingAction->temp_Z.clear();
    theSteppingAction->temp_E.clear();
    theSteppingAction->temp_Radlen.clear();

  }
  
  else if(theName=="RearTracker"){


    x1  = aStep->GetPreStepPoint()->GetPosition()[0];
    y1  = aStep->GetPreStepPoint()->GetPosition()[1];
    z1  = aStep->GetPreStepPoint()->GetPosition()[2];

    px1 = aStep->GetPreStepPoint()->GetMomentumDirection()[0];
    py1 = aStep->GetPreStepPoint()->GetMomentumDirection()[1];
    pz1 = aStep->GetPreStepPoint()->GetMomentumDirection()[2];
    
    //mat_name = &(theSteppingAction->temp_name);
    tracks_X = &(theSteppingAction->temp_X);
    tracks_Y = &(theSteppingAction->temp_Y);
    tracks_Z = &(theSteppingAction->temp_Z);
    tracks_E = &(theSteppingAction->temp_E);
    //Radlen   = &(theSteppingAction->temp_Radlen);

    tracks_X->push_back(x1);
    tracks_Y->push_back(y1);
    tracks_Z->push_back(z1);
    tracks_E->push_back(aStep->GetPreStepPoint()->GetKineticEnergy());
    /*Radlen->push_back(aStep->GetPreStepPoint()->GetMaterial()->GetRadlen());
    mat_name->push_back( aStep->GetPreStepPoint()->GetMaterial()->GetName().data() );
    */

    theta_y1 = atan2(py1,px1);
    theta_z1 = atan2(pz1,px1); 
    Id    = aStep->GetTrack()->GetTrackID();
    Estop = aStep->GetPreStepPoint()->GetKineticEnergy();
    t->Fill();

    theSteppingAction->temp_name.clear();
    theSteppingAction->temp_X.clear();
    theSteppingAction->temp_Y.clear();
    theSteppingAction->temp_Z.clear();
    theSteppingAction->temp_E.clear();
    theSteppingAction->temp_Radlen.clear();

  }
}
void Analysis::Save(){
  f1->cd();
  theGenerator = PrimaryGeneratorAction::GetInstance();
  t->Write("",TObject::kOverwrite);
  f1->Close();
}
