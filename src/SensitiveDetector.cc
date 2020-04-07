#include "SensitiveDetector.hh"
#include "Analysis.hh"
using namespace CLHEP;

SensitiveDetector::SensitiveDetector(G4String name):G4VSensitiveDetector(name),theName(name)
{
  theAnalysis = Analysis::GetInstance();  
}

G4bool SensitiveDetector::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{

  if(theName=="doseCube"){
    theAnalysis->TotEnergyDeposit += aStep->GetTotalEnergyDeposit()*MeV;
    if(aStep->GetTrack()->GetTrackID()==1) theAnalysis->NEnergyDeposit++;
  }
  if ( aStep->GetPreStepPoint()->GetStepStatus() == fGeomBoundary){// && aStep->GetTrack()->GetTrackID()==1){
      theAnalysis->RearFrontDetector(aStep, theName);
  }
        
  return true;
}
