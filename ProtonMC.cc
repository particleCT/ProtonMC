#include "EmPhysics_pCT.hh"
#include "HadrontherapyPhysicsList.hh"
#include "PrimaryGeneratorAction.hh"
#include "OrganicMaterial.hh"
#include "G4NistManager.hh"
#include "DetectorConstruction.hh"
#include "SteppingAction.hh"
#include "G4EmCalculator.hh"
#include "G4RunManager.hh"
#include <TROOT.h>
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4EmCalculator.hh"
#include "G4Proton.hh"
#include "G4UnitsTable.hh"
#include "Analysis.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4UImanager.hh"
#include "ParallelWorldConstruction.hh"
#ifdef VIS
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"
#endif
#include <iostream>
#include <fstream>
using namespace std;
void calcRSP(OrganicMaterial* theMaterial);

int main(int argc,char** argv) {
  gROOT->ProcessLine("#include <vector>");
  time_t seed;
  time(&seed);
  CLHEP::RanecuEngine *theRanGenerator = new CLHEP::RanecuEngine;  
  theRanGenerator->setSeed(seed);
  CLHEP::HepRandom::setTheEngine(theRanGenerator);
 
  G4int nProtons  = atoi(argv[1]); 
  G4double Energy = atof(argv[2]); // MeV
  G4String Model  = argv[3];
  G4int angle     = atoi(argv[4]); // Degrees
  G4double thick  = atof(argv[5]); // cm
  G4int   thread  = atoi(argv[6]); 
  G4String paraWorldName = "ParallelWorld";

  G4RunManager* runManager = new G4RunManager;
  runManager->SetUserInitialization(new HadrontherapyPhysicsList(paraWorldName));
  DetectorConstruction* myDC = new DetectorConstruction(Model,angle,thick);
  myDC->RegisterParallelWorld( new ParallelWorldConstruction(paraWorldName));
  runManager->SetUserAction( new PrimaryGeneratorAction(Energy));
  runManager->SetUserAction( new SteppingAction() );
  runManager->SetUserInitialization( myDC );
  Analysis* theAnalysis    = new Analysis(thread,angle,Model);
  OrganicMaterial* theMaterial = OrganicMaterial::GetInstance();  

  runManager->Initialize();

#ifdef VIS
  G4UImanager * UImanager = G4UImanager::GetUIpointer();
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
  G4UIExecutive* ui = new G4UIExecutive(argc, argv);
  G4cout << " UI session starts ..." << G4endl;
  UImanager->ApplyCommand("/control/execute vis.mac");
  ui->SessionStart();

#endif
  runManager->BeamOn( nProtons );
  //calcRSP(theMaterial);
  theAnalysis->Save();

#ifdef VIS
    delete visManager;
    delete ui;
#endif
    delete runManager;
  }


void calcRSP(OrganicMaterial* theMaterial){
  G4ParticleDefinition* particle = G4Proton::Definition();
  G4EmCalculator* emCal = new G4EmCalculator;
  ofstream myfile;
  myfile.open ("RSP.txt");
  for(auto itr=theMaterial->theMaterialList.begin(); itr!=theMaterial->theMaterialList.end(); itr++) {
    G4int I = 0;
    G4Material* mat = itr->second;
    G4double tot =0;

    for(int j=0;j<5000;j++){
      
      G4double dedx_w = emCal->ComputeTotalDEDX( float(j/10)*MeV,particle,theMaterial->water);
      G4double dedx_b = emCal->ComputeTotalDEDX( float(j/10)*MeV,particle,mat);
      tot +=dedx_b/dedx_w;
      I+=1;
    }
    G4double RSP = tot/I;
    myfile<<" "<<mat->GetDensity()/(g/cm3)<<" "<<RSP<<" "<<mat->GetName()<<endl;
  }
  myfile.close();
  }



