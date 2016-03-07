#ifndef Analysis_hh
#define Analysis_hh
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "TTree.h"
using namespace std;
class G4Step;
class TFile ;
class TTree ;
class PrimaryGeneratorAction;
class DetectorConstruction;
class SteppingAction;
class Analysis
{
public:

  Analysis(G4int,G4int,G4String);
  ~Analysis();
  static inline Analysis* GetInstance() { return theAnalysis; }
  void analyseHit(G4Step*,G4String);
  TTree  *t;
  TFile *f1;
  void Save();

  vector<double>* tracks_X;
  vector<double>* tracks_Y;
  vector<double>* tracks_Z;
  
  vector<TString>*  mat_name;
  
private:

  static Analysis* theAnalysis;
  PrimaryGeneratorAction *theGenerator;
  DetectorConstruction   *theDetector ;
  SteppingAction         *theSteppingAction;
  Double_t x0,y0,z0,px0,py0,pz0;
  Double_t x1,y1,z1,px1,py1,pz1;
  Double_t Einit,Estop;
  G4int Id;
};
#endif
