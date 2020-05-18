#ifndef Analysis_hh
#define Analysis_hh
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "TTree.h"
using namespace std;
class G4Step;
class TFile ;
class TTree ;
class TH3D  ;
class PrimaryGeneratorAction;
class DetectorConstruction;
class SteppingAction;
class Analysis
{
public:
  Analysis(G4int,G4double, G4String, G4int);
  ~Analysis();
  static inline Analysis* GetInstance() { return theAnalysis; }
  void analyseHit(G4Step*,G4String);
  TTree  *t; //phasespace
  TTree  *t2;//specific data
  TFile *f1;
  void Save();
  void RearFrontDetector(G4Step* aStep, G4String theName);
  vector<double>* tracks_X;
  vector<double>* tracks_Y;
  vector<double>* tracks_Z;
  vector<double>* Eloss;
  vector<double>* Length;
  vector<double>* tracks_E;
  vector<double>* Radlen;
  vector<TString>*  mat_name;
  vector<TString>*  vol_name;
  std::string  proc_name, part_name;
  G4float  TotEnergyDeposit = 0.;
  G4int    NEnergyDeposit =0;
  G4int    NPart;
  G4int NPrimEnd=0;
  G4int NPrimMiddle=0;
  G4int NPrimCylinderMiddle=0;
private:

  static Analysis* theAnalysis;
  PrimaryGeneratorAction *theGenerator;
  DetectorConstruction   *theDetector ;
  SteppingAction         *theSteppingAction;
  Float_t x0,y0,z0,px0,py0,pz0;
  Float_t x1,y1,z1,px1,py1,pz1;
  Float_t theta_y1, theta_z1;
  Float_t Einit,Estop, EnergyMid;
  G4int Id;
};
#endif
