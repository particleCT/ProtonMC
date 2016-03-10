//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id$
//
//---------------------------------------------------------------------------
//
// ClassName:   EmPhysics_pCT
//
// Author:      V.Ivanchenko 13.03.2008
//
// Modified:
// 21.04.2008 V.Ivanchenko add long-lived D and B mesons; use spline
// 28.05.2008 V.Ivanchenko linLossLimit=0.01 for ions 0.001 for others
//
//----------------------------------------------------------------------------
//

#include "EmPhysics_pCT.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4EmProcessOptions.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4RayleighScattering.hh"
#include "G4PEEffectFluoModel.hh"
#include "G4KleinNishinaModel.hh"

#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4MscStepLimitType.hh"
#include "G4UrbanMscModel.hh"
#include "G4DummyModel.hh"
#include "G4WentzelVIModel.hh"
#include "G4CoulombScattering.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4Generator2BS.hh"
#include "G4SeltzerBergerModel.hh"

#include "G4eplusAnnihilation.hh"
#include "G4UAtomicDeexcitation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4MuBremsstrahlungModel.hh"
#include "G4MuPairProductionModel.hh"
#include "G4hBremsstrahlungModel.hh"
#include "G4hPairProductionModel.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4NuclearStopping.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

#include "G4PhysicsListHelper.hh"
#include "G4BuilderType.hh"

#include "G4ProcessManager.hh"

// factory
#include "G4PhysicsConstructorFactory.hh"
//
G4_DECLARE_PHYSCONSTR_FACTORY(EmPhysics_pCT);

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmPhysics_pCT::EmPhysics_pCT(G4int ver)
  : G4VPhysicsConstructor("EmPhysics_pCT"), verbose(ver)
{
  G4LossTableManager::Instance();
  SetPhysicsType(bElectromagnetic);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmPhysics_pCT::EmPhysics_pCT(G4int ver, const G4String&)
  : G4VPhysicsConstructor("EmPhysics_pCT"), verbose(ver)
{
  G4LossTableManager::Instance();
  SetPhysicsType(bElectromagnetic);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EmPhysics_pCT::~EmPhysics_pCT()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmPhysics_pCT::ConstructParticle()
{
// gamma
 G4Gamma::Gamma();

// leptons
  G4Electron::Electron();
  G4Positron::Positron();

// barions
  G4Proton::Proton();
  G4AntiProton::AntiProton();

// ions
  G4Deuteron::Deuteron();
  G4Triton::Triton();
  G4He3::He3();
  G4Alpha::Alpha();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EmPhysics_pCT::ConstructProcess()
{
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
/*
  // muon & hadron bremsstrahlung and pair production
  G4MuBremsstrahlung* mub = new G4MuBremsstrahlung();
  G4MuPairProduction* mup = new G4MuPairProduction();
  G4hBremsstrahlung* pib = new G4hBremsstrahlung();
  G4hPairProduction* pip = new G4hPairProduction();
  G4hBremsstrahlung* kb = new G4hBremsstrahlung();
  G4hPairProduction* kp = new G4hPairProduction();
  G4hBremsstrahlung* pb = new G4hBremsstrahlung();
  G4hPairProduction* pp = new G4hPairProduction();

  // muon & hadron multiple scattering
  G4MuMultipleScattering* mumsc = new G4MuMultipleScattering();
  mumsc->AddEmModel(0, new G4WentzelVIModel());
  //G4hMultipleScattering* pimsc = new G4hMultipleScattering();
  // pimsc->AddEmModel(0, new G4WentzelVIModel());
  // G4hMultipleScattering* kmsc = new G4hMultipleScattering();
  // kmsc->AddEmModel(0, new G4WentzelVIModel());
  //G4hMultipleScattering* pmsc = new G4hMultipleScattering();
  //pmsc->AddEmModel(0, new G4WentzelVIModel());
  G4hMultipleScattering* hmsc = new G4hMultipleScattering("ionmsc");
*/
  // nuclear stopping
  G4NuclearStopping* ionnuc = new G4NuclearStopping();
  G4NuclearStopping* pnuc = new G4NuclearStopping();

  // Add standard EM Processes
  aParticleIterator->reset();
  while( (*aParticleIterator)() ){
    G4ParticleDefinition* particle = aParticleIterator->value();
    G4String particleName = particle->GetParticleName();
    if(verbose > 1)
      G4cout << "### " << GetPhysicsName() << " instantiates for " 
	     << particleName << G4endl;
    if (particleName == "gamma") {

      ph->RegisterProcess(new G4PhotoElectricEffect(), particle);
      ph->RegisterProcess(new G4ComptonScattering(), particle);
      ph->RegisterProcess(new G4GammaConversion(), particle);

    }else   if (particleName == "e-") {

      G4eMultipleScattering* msc = new G4eMultipleScattering();
      //msc->AddEmModel(0, new G4UrbanMscModel95());
      msc->SetStepLimitType(fUseDistanceToBoundary);
      G4eIonisation* eIoni = new G4eIonisation();
      //eIoni->SetStepFunction(0.2, 100*um);

      //G4eBremsstrahlung* brem = new G4eBremsstrahlung();
      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(eIoni, particle);
      //ph->RegisterProcess(brem, particle);

    } else if (particleName == "e+") {

      G4eMultipleScattering* msc = new G4eMultipleScattering();
      //msc->AddEmModel(0, new G4UrbanMscModel95());
      msc->SetStepLimitType(fUseDistanceToBoundary);
      G4eIonisation* eIoni = new G4eIonisation();
      //eIoni->SetStepFunction(0.2, 100*um);

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(eIoni, particle);
      //ph->RegisterProcess(new G4eBremsstrahlung(), particle);
      ph->RegisterProcess(new G4eplusAnnihilation(), particle);

    }/* else if (particleName == "mu+" ||
               particleName == "mu-"    ) {

      G4MuIonisation* muIoni = new G4MuIonisation();
      //muIoni->SetStepFunction(0.2, 50*um);

      ph->RegisterProcess(mumsc, particle);
      ph->RegisterProcess(muIoni, particle);
      ph->RegisterProcess(mub, particle);
      ph->RegisterProcess(mup, particle);
      ph->RegisterProcess(new G4CoulombScattering(), particle);

    } */else if (particleName == "alpha" ||
               particleName == "He3") {

      G4hMultipleScattering* msc = new G4hMultipleScattering();
      G4ionIonisation* ionIoni = new G4ionIonisation();
      //ionIoni->SetStepFunction(0.1, 10*um);

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(ionIoni, particle);
      ph->RegisterProcess(ionnuc, particle);

    } else if (particleName == "GenericIon") {

      G4ionIonisation* ionIoni = new G4ionIonisation();
      ionIoni->SetEmModel(new G4IonParametrisedLossModel());
      G4hMultipleScattering* msc = new G4hMultipleScattering();
      //ionIoni->SetStepFunction(0.1, 1*um);

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(ionIoni, particle);
      ph->RegisterProcess(ionnuc, particle);

    } /*else if (particleName == "pi+" ||
               particleName == "pi-" ) {

      G4hMultipleScattering* pimsc = new G4hMultipleScattering();
      G4hIonisation* hIoni = new G4hIonisation();
      //hIoni->SetStepFunction(0.2, 50*um);

      ph->RegisterProcess(pimsc, particle);
      ph->RegisterProcess(hIoni, particle);
      ph->RegisterProcess(pib, particle);
      ph->RegisterProcess(pip, particle);

    } else if (particleName == "kaon+" ||
               particleName == "kaon-" ) {

      G4hMultipleScattering* kmsc = new G4hMultipleScattering();
      G4hIonisation* hIoni = new G4hIonisation();
      //hIoni->SetStepFunction(0.2, 50*um);

      ph->RegisterProcess(kmsc, particle);
      ph->RegisterProcess(hIoni, particle);
      ph->RegisterProcess(kb, particle);
      ph->RegisterProcess(kp, particle);

    }*/ else if (particleName == "proton" ||
	       particleName == "anti_proton") {

      G4hMultipleScattering* pmsc = new G4hMultipleScattering();
      G4hIonisation* hIoni = new G4hIonisation();
      hIoni->SetStepFunction(0.2, 50*um);

      ph->RegisterProcess(pmsc, particle);
      ph->RegisterProcess(hIoni, particle);
      /*ph->RegisterProcess(pb, particle);
      ph->RegisterProcess(pp, particle);*/
      ph->RegisterProcess(pnuc, particle);

    } /*else if (particleName == "B+" ||
	       particleName == "B-" ||
	       particleName == "D+" ||
	       particleName == "D-" ||
	       particleName == "Ds+" ||
	       particleName == "Ds-" ||
               particleName == "anti_He3" ||
               particleName == "anti_alpha" ||
               particleName == "anti_deuteron" ||
               particleName == "anti_lambda_c+" ||
               particleName == "anti_omega-" ||
               particleName == "anti_sigma_c+" ||
               particleName == "anti_sigma_c++" ||
               particleName == "anti_sigma+" ||
               particleName == "anti_sigma-" ||
               particleName == "anti_triton" ||
               particleName == "anti_xi_c+" ||
               particleName == "anti_xi-" ||
               particleName == "deuteron" ||
	       particleName == "lambda_c+" ||
               particleName == "omega-" ||
               particleName == "sigma_c+" ||
               particleName == "sigma_c++" ||
               particleName == "sigma+" ||
               particleName == "sigma-" ||
               particleName == "tau+" ||
               particleName == "tau-" ||
               particleName == "triton" ||
               particleName == "xi_c+" ||
               particleName == "xi-" ) {

      ph->RegisterProcess(hmsc, particle);
      ph->RegisterProcess(new G4hIonisation(), particle);
    }*/
  }
    
  // Em options
  //      
  G4EmProcessOptions opt;
  opt.SetVerbose(verbose);
  
  // Multiple Coulomb scattering
  //
  //opt.SetPolarAngleLimit(CLHEP::pi);
    
  // Physics tables
  //
  //opt.SetMinEnergy(100*eV);
  //opt.SetMaxEnergy(10000*MeV);
  //opt.SetDEDXBinning(9009);
  //opt.SetLambdaBinning(9090);

  // Nuclear stopping
  //pnuc->SetMaxKinEnergy(MeV);
    
  // Ionization
  //
  //opt.SetSubCutoff(true);    

  // Deexcitation
  //G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  //G4LossTableManager::Instance()->SetAtomDeexcitation(de);
  //de->SetFluo(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
