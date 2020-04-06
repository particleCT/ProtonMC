#ifndef SteppingAction_hh
#define SteppingAction_hh
#include "DetectorConstruction.hh"
#include "G4UserSteppingAction.hh"
#include "G4UserEventAction.hh"
#include "globals.hh"


using namespace std;


//class G4Track;
class G4Step;
class Analysis;
class G4ParticleChange;
class G4DynamicParticle;
class PrimaryGeneratorAction;
class TString;
class SteppingAction : public G4UserSteppingAction
{
public:
  SteppingAction();
  ~SteppingAction();
  std::vector<double> temp_X;
  std::vector<double> temp_Y;
  std::vector<double> temp_Z;
  std::vector<double> temp_E;
  std::vector<double> temp_Radlen;
  std::vector<TString> temp_name;
  G4double TotEnergyDeposit = 0.;
  G4int NPrimMiddle = 0;
  virtual void UserSteppingAction(const G4Step* astep);
  static inline SteppingAction* GetInstance() { return theSteppingAction; }

private:
  static SteppingAction* theSteppingAction;
  Analysis*              theAnalysis;
};

#endif
