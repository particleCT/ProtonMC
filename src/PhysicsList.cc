
#include "PhysicsList.hh"
#include "G4PhysListFactory.hh"
#include "G4VPhysicsConstructor.hh"
#include "G4IonQMDPhysics.hh"

// Physic lists (contained inside the Geant4 distribution)
#include "G4EmStandardPhysics_option3.hh"
#include "G4EmStandardPhysics_option2.hh"
#include "G4EmStandardPhysics_option1.hh"
#include "G4EmStandardPhysics_option4.hh"
#include "G4EmStandardPhysics.hh"
#include "G4StepLimiterPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4DecayPhysics.hh"
#include "G4EmExtraPhysics.hh"
#include "G4StoppingPhysics.hh"

#include "G4HadronElasticPhysics.hh"
#include "G4ParallelWorldProcess.hh"
#include "G4HadronElasticProcess.hh"
#include "G4HadronElasticPhysicsHP.hh"

#include "G4HadronInelasticQBBC.hh"
#include "G4HadronPhysicsQGSP_BIC.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4IonBinaryCascadePhysics.hh"
#include "G4Decay.hh"


// Physic dedicated to the ion-ion inelastic processes            
#include "LocalIonIonInelasticPhysic.hh"


#include "G4LossTableManager.hh"
#include "G4UnitsTable.hh"
#include "G4ProcessManager.hh"
#include "G4IonFluctuations.hh"
#include "G4IonParametrisedLossModel.hh"
#include "G4EmProcessOptions.hh"

#include "G4RadioactiveDecayPhysics.hh"
#include "G4StepLimiter.hh"

#include "HadrontherapyStepMax.hh"
#include <stdexcept>
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
/////////////////////////////////////////////////////////////////////////////
PhysicsList::PhysicsList(G4String& parWorldName):G4VModularPhysicsList(),pWorldName(parWorldName)
{
  G4LossTableManager::Instance();
  defaultCutValue = 1.0*mm;//1.0*mm;
  cutForGamma     = defaultCutValue;
  cutForElectron  = defaultCutValue;
  cutForPositron  = defaultCutValue;

  helIsRegisted  = false;
  bicIsRegisted  = false;
  biciIsRegisted = false;
  locIonIonInelasticIsRegistered = false;
  radioactiveDecayIsRegisted = false;

  stepMaxProcess  = 0;
  SetVerboseLevel(1);

  // EM physics
  emPhysicsList = new G4EmStandardPhysics_option4();
  hadronPhys.push_back( new LocalIonIonInelasticPhysic());
  hadronPhys.push_back( new G4DecayPhysics());
  hadronPhys.push_back( new G4EmExtraPhysics());
  hadronPhys.push_back( new G4HadronElasticPhysics());
  hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC());
  hadronPhys.push_back( new G4StoppingPhysics());
  //hadronPhys.push_back( new G4IonBinaryCascadePhysics());
  //hadronPhys.push_back( new G4RadioactiveDecayPhysics());
  
  //hadronPhys.push_back( new G4IonQMDPhysics());

  //AddPhysicsList("standard_opt4");
  //AddPhysicsList("local_ion_ion_inelastic");
  //hadronPhys.push_back( new G4DecayPhysics());
  //hadronPhys.push_back( new G4RadioactiveDecayPhysics());
  //hadronPhys.push_back( new G4EmExtraPhysics());
  //hadronPhys.push_back( new G4HadronElasticPhysicsHP());
  //hadronPhys.push_back( new G4StoppingPhysics());
  //hadronPhys.push_back( new G4HadronPhysicsQGSP_BIC_HP());
  
  // Decay physics and all particles
  decPhysicsList = new G4DecayPhysics();
  //emName = G4String("emstandard_opt3");
    
  //AddPhysicsList("binary_ion");
  //AddPhysicsList("qmd_ion_ion_inelastic");
  //AddPhysicsList("standard_opt3");
  //AddPhysicsList("local_ion_ion_inelastic");  
  //AddPhysicsList("elastic");
  

}

/////////////////////////////////////////////////////////////////////////////
PhysicsList::~PhysicsList()
{
  delete emPhysicsList;
  delete decPhysicsList;
  for(size_t i=0; i<hadronPhys.size(); i++) {delete hadronPhys[i];}
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::AddPackage(const G4String& name)
{
  G4PhysListFactory factory;
  G4VModularPhysicsList* phys =factory.GetReferencePhysList(name);
  G4int i=0;
  const G4VPhysicsConstructor* elem= phys->GetPhysics(i);
  G4VPhysicsConstructor* tmp = const_cast<G4VPhysicsConstructor*> (elem);
  while (elem !=0)
	{
	  RegisterPhysics(tmp);
	  elem= phys->GetPhysics(++i) ;
	  tmp = const_cast<G4VPhysicsConstructor*> (elem);
	}
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::ConstructParticle()
{
  decPhysicsList->ConstructParticle();
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::ConstructProcess()
{
  // transportation
  //
  AddTransportation();

  // parallel world
  //
  //AddParallelWorldProcess();

  // electromagnetic physics list
  //
  emPhysicsList->ConstructProcess();
  em_config.AddModels();

  // decay physics list
  //
  decPhysicsList->ConstructProcess();

  // hadronic physics lists
  for(size_t i=0; i<hadronPhys.size(); i++) {
    hadronPhys[i]->ConstructProcess();
  }

  // step limitation (as a full process)
  //
  //  AddStepMax();
}
/////////////////////////////////////////////////////////////////////////////
void PhysicsList::AddParallelWorldProcess() {
  // Add parallel world process
  G4ParallelWorldProcess* theParallelWorldProcess = new G4ParallelWorldProcess("paraWorldProc");
  theParallelWorldProcess->SetParallelWorld(pWorldName);
  theParallelWorldProcess->SetLayeredMaterialFlag();
  GetParticleIterator()->reset();

  while ( (*GetParticleIterator())() ) {
    G4ParticleDefinition* particle = GetParticleIterator()->value();
    if ( !particle->IsShortLived() ) {
      G4ProcessManager* pmanager = particle->GetProcessManager();
      pmanager->AddProcess(theParallelWorldProcess);
      if ( theParallelWorldProcess->IsAtRestRequired(particle) ) { pmanager->SetProcessOrdering(theParallelWorldProcess,idxAtRest,9999); }
      pmanager->SetProcessOrdering(theParallelWorldProcess,idxAlongStep,1);
      pmanager->SetProcessOrdering(theParallelWorldProcess,idxPostStep,9999);

    }
  }
}
/////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
void PhysicsList::AddPhysicsList(const G4String& name)
{

  if (verboseLevel>1) {
    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">" << G4endl;
  }
  if (name == emName) return;

  /////////////////////////////////////////////////////////////////////////////
  //   ELECTROMAGNETIC MODELS
  /////////////////////////////////////////////////////////////////////////////
  if (name == "standard_opt2") {
     emName = name;
     delete emPhysicsList;
     emPhysicsList = new G4EmStandardPhysics_option2();
     G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option2" << G4endl;
  } else if (name == "standard_opt1") {
 	  emName = name;
 	  delete emPhysicsList;
 	  emPhysicsList = new G4EmStandardPhysics_option1();
 	  G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option1" << G4endl;
  } else if (name == "standard_opt4") {
    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmStandardPhysics_option4();
    G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option4" << G4endl;
   } else if (name == "standard") {
 	  emName = name;
 	  delete emPhysicsList;
 	  emPhysicsList = new G4EmStandardPhysics();
 	  G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics" << G4endl;

   } else if (name == "standard_opt3") {
     emName = name;
     delete emPhysicsList;
     emPhysicsList = new G4EmStandardPhysics_option3();
     G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmStandardPhysics_option3" << G4endl;

   }else if (name == "LowE_Livermore") {
    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmLivermorePhysics();
    G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmLivermorePhysics" << G4endl;

  } else if (name == "LowE_Penelope") {
    emName = name;
    delete emPhysicsList;
    emPhysicsList = new G4EmPenelopePhysics();
    G4cout << "THE FOLLOWING ELECTROMAGNETIC PHYSICS LIST HAS BEEN ACTIVATED: G4EmLivermorePhysics" << G4endl;

    /////////////////////////////////////////////////////////////////////////////
    //   HADRONIC MODELS
    /////////////////////////////////////////////////////////////////////////////
  } else if (name == "elastic" && !helIsRegisted) {
    G4cout << "THE FOLLOWING HADRONIC ELASTIC PHYSICS LIST HAS BEEN ACTIVATED: G4HadronElasticPhysics()" << G4endl;

    hadronPhys.push_back(new G4HadronElasticPhysics);
    helIsRegisted = true;

  } else if (name == "local_ion_ion_inelastic") {
    hadronPhys.push_back(new LocalIonIonInelasticPhysic());
    locIonIonInelasticIsRegistered = true;

  
  } else if (name == "binary" && !bicIsRegisted) {
    //hadronPhys.push_back(new G4HadronInelasticQBBC());
    hadronPhys.push_back(new G4HadronPhysicsQGSP_BIC());
    bicIsRegisted = true;
    G4cout << "THE FOLLOWING HADRONIC INELASTIC PHYSICS LIST HAS BEEN ACTIVATED: G4HadronInelasticQBBC()" << G4endl;

  } else if (name == "binary_ion" && !biciIsRegisted) {
    hadronPhys.push_back(new G4IonBinaryCascadePhysics());
    biciIsRegisted = true;

  } else if (name == "qmd_ion_ion_inelastic" && !locIonIonInelasticIsRegistered) {
    hadronPhys.push_back(new G4IonQMDPhysics());
    locIonIonInelasticIsRegistered = true;

  } else if (name == "radioactive_decay" && !radioactiveDecayIsRegisted ) {
    hadronPhys.push_back(new G4RadioactiveDecayPhysics());
    radioactiveDecayIsRegisted = true;

  } else {

    G4cout << "PhysicsList::AddPhysicsList: <" << name << ">"
           << " is not defined"
           << G4endl;
    std::stringstream ss;
    ss << "PhysicsList::AddPhysicsList: <" << name << ">"
            << " is not defined";
    throw std::runtime_error(ss.str());
  }
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::AddStepMax()
{
  // Step limitation seen as a process
  stepMaxProcess = new HadrontherapyStepMax();
  G4StepLimiter* stepLimiter = new G4StepLimiter();
  GetParticleIterator()->reset();
  while ((*GetParticleIterator())()){
    G4ParticleDefinition* particle = GetParticleIterator()->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    if (stepMaxProcess->IsApplicable(*particle) && pmanager)
      {
        pmanager ->AddDiscreteProcess(stepMaxProcess);
	pmanager->AddDiscreteProcess(stepLimiter);
      }
  }
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::SetCuts()
{
  G4ProductionCutsTable::GetProductionCutsTable()->SetEnergyRange(100*eV, 100*GeV);
  if (verboseLevel >0){
    G4cout << "PhysicsList::SetCuts:";
    G4cout << "CutLength : " << G4BestUnit(defaultCutValue,"Length") << G4endl;
  }

  // set cut values for gamma at first and for e- second and next for e+,
  // because some processes for e+/e- need cut values for gamma
  SetCutValue(cutForGamma, "gamma");
  SetCutValue(cutForElectron, "e-");
  SetCutValue(cutForPositron, "e+");
  
  if (verboseLevel>0) DumpCutValuesTable();
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::SetCutForGamma(G4double cut)
{
  cutForGamma = cut;
  SetParticleCuts(cutForGamma, G4Gamma::Gamma());
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::SetCutForElectron(G4double cut)
{
  cutForElectron = cut;
  SetParticleCuts(cutForElectron, G4Electron::Electron());
}

/////////////////////////////////////////////////////////////////////////////
void PhysicsList::SetCutForPositron(G4double cut)
{
  cutForPositron = cut;
  SetParticleCuts(cutForPositron, G4Positron::Positron());
}

