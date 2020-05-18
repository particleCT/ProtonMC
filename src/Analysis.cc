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
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Analysis.hh"

Analysis* Analysis::theAnalysis=NULL;
Analysis::~Analysis(){theAnalysis=NULL;}

Analysis::Analysis(G4int thread, G4double angle,G4String theName, G4int nProtons){
  theAnalysis  = this;
  NPart = nProtons;
  theGenerator = PrimaryGeneratorAction::GetInstance();
  theDetector  = DetectorConstruction::GetInstance();
  f1 = new TFile(Form("%s_%.0f_%.1f_%d_%d.root",theName.data(),theGenerator->ENER,angle,thread,theGenerator->A),"recreate");

  t = new TTree("phase","PS");

  t->Branch("x0",&x0,"x0/F");
  t->Branch("y0",&y0,"y0/F");
  t->Branch("z0",&z0,"z0/F");
  
  t->Branch("px0",&px0,"px0/F");
  t->Branch("py0",&py0,"py0/F");
  t->Branch("pz0",&pz0,"pz0/F");
  t->Branch("Einit",&Einit,"Einit/F");

  t->Branch("x1",&x1,"x1/F");
  t->Branch("y1",&y1,"y1/F");
  t->Branch("z1",&z1,"z1/F");
  
  t->Branch("px1",&px1,"px1/F");
  t->Branch("py1",&py1,"py1/F");
  t->Branch("pz1",&pz1,"pz1/F");
  t->Branch("Estop",&Estop,"Estop/F");
  t->Branch("EnergyMid",&EnergyMid,"EnergyMid/F");

  /*t->Branch("tracks_X",&tracks_X);
  t->Branch("tracks_Y",&tracks_Y);
  t->Branch("tracks_Z",&tracks_Z);*/
  /*t->Branch("tracks_E",&tracks_E);
  t->Branch("Radlen",&Radlen);
  t->Branch("mat_name",&mat_name);
  */
  t->Branch("proc_name",&proc_name);
  t->Branch("part_name",&part_name);
  t->Branch("Id",&Id,"Id/I");
}


void Analysis::RearFrontDetector(G4Step* aStep, G4String theName)
{
  
  f1->cd();
  theSteppingAction = SteppingAction::GetInstance();
  if(theName=="FrontTracker"){

    x0  = aStep->GetPreStepPoint()->GetPosition()[0];
    y0  = aStep->GetPreStepPoint()->GetPosition()[1];
    z0  = aStep->GetPreStepPoint()->GetPosition()[2];

    px0 = aStep->GetPreStepPoint()->GetMomentumDirection()[0];
    py0 = aStep->GetPreStepPoint()->GetMomentumDirection()[1];
    pz0 = aStep->GetPreStepPoint()->GetMomentumDirection()[2];
    Einit = theGenerator->Einit;
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

    mat_name = &(theSteppingAction->temp_name);
    tracks_X = &(theSteppingAction->temp_X);
    tracks_Y = &(theSteppingAction->temp_Y);
    tracks_Z = &(theSteppingAction->temp_Z);
    tracks_E = &(theSteppingAction->temp_E);
    Radlen   = &(theSteppingAction->temp_Radlen);

    Estop = aStep->GetPreStepPoint()->GetKineticEnergy();    
    Id    = aStep->GetTrack()->GetTrackID();
    if(aStep->GetTrack()->GetCreatorProcess()!=0) proc_name = aStep->GetTrack()->GetCreatorProcess()->GetProcessName();
    else proc_name  = "primary";
    part_name  = aStep->GetTrack()->GetDynamicParticle()->GetDefinition()->GetParticleName();
    t->Fill();
    //if(part_name==theGenerator->particle->GetParticleName()) NPrimEnd++;
    if(Id==1) NPrimEnd++;

    // Energy total in the middle
    //theSteppingAction->TotEnergyDeposit = 0;
    theSteppingAction->temp_name.clear();
    theSteppingAction->temp_X.clear();
    theSteppingAction->temp_Y.clear();
    theSteppingAction->temp_Z.clear();
    theSteppingAction->temp_E.clear();
    theSteppingAction->temp_Radlen.clear();

    aStep->GetTrack()->SetTrackStatus(fStopAndKill);

  }
}
void Analysis::Save(){
  f1->cd();
  t->Write("",TObject::kOverwrite);
  t2 = new TTree("header","H");
  t2->Branch("NPart",&NPart,"NPart/I");
  t2->Branch("NEnergyDeposit",&NEnergyDeposit,"NEnergyDeposit/I");
  t2->Branch("TotEnergyDeposit",&TotEnergyDeposit,"TotEnergyDeposit/F");
  t2->Branch("NPrimaryMiddle",&NPrimMiddle,"NPrimaryMiddle/I");
  t2->Branch("NPrimaryCylinderMiddle",&NPrimCylinderMiddle,"NPrimaryCylinderMiddle/I");
  t2->Branch("NPrimaryEnd",&NPrimEnd,"NPrimaryEnd/I");
  t2->Fill();
  cout<<"Energy: "<<TotEnergyDeposit<<" [MeV] in "<<NPrimCylinderMiddle<<" events"<<endl;
  cout<<"S_MC: "<<TotEnergyDeposit/float(NPrimCylinderMiddle)<<endl;
  cout<<"g_MC: "<<NPrimMiddle<<"/"<<NPrimEnd<<" ="<<float(NPrimMiddle)/float(NPrimEnd)<<endl;
  t2->Write("",TObject::kOverwrite);
  f1->Close();
}
