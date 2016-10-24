#include "TFile.h"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4ElementVector.hh"
#include "G4NistElementBuilder.hh"
#include "G4NistManager.hh"
#include "G4EmCalculator.hh"
#include "G4Proton.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "OrganicMaterial.hh"

OrganicMaterial* OrganicMaterial::theMaterial=NULL;

OrganicMaterial::OrganicMaterial(){   

  theMaterial = this; 

  man = G4NistManager::Instance();
  elH  = man->FindOrBuildElement( 1);
  elC  = man->FindOrBuildElement( 6);
  elN  = man->FindOrBuildElement( 7);
  elO  = man->FindOrBuildElement( 8);
  elF  = man->FindOrBuildElement( 9);
  elNa = man->FindOrBuildElement(11);
  elMg = man->FindOrBuildElement(12);
  elSi = man->FindOrBuildElement(14);
  elP  = man->FindOrBuildElement(15);
  elS  = man->FindOrBuildElement(16);
  elCl = man->FindOrBuildElement(17);
  elAr = man->FindOrBuildElement(18);
  elK  = man->FindOrBuildElement(19);
  elCa = man->FindOrBuildElement(20);
  elFe = man->FindOrBuildElement(26);
  elZn = man->FindOrBuildElement(30);

  elBa = man->FindOrBuildElement(56);
  water = new G4Material("Water",1.0*g/cm3,nel=2,kStateLiquid);   // 1.0*g/cm3                                                                      
  water->AddElement(elH,11.20*perCent);
  water->AddElement(elO,88.80*perCent);
  water->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);

}

OrganicMaterial::~OrganicMaterial(){ theMaterial = NULL; }

G4Material* OrganicMaterial::ConstructMaterial(G4String Name,G4double density)
{  
 
  std::map<G4Element*, G4double> elVector; 
  if (Name=="Air" ) {//1.025*mg/cm3 

    elVector.insert(std::pair< G4Element*,G4double> ( elN, 70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO, 30*perCent ));

  }

  ////  ////  ////  ////  //// 
  //  Y. Watanabe, “Derivation of linear attenuation coefficients from CT numbers for low-energy photons,” Physics in medicine and biology 4  //  4(9), 2201 (1999).
  ////  ////  ////  ////  ////

  else if(Name=="LN300"){//0.3*g/cm3 
    elVector.insert(std::pair< G4Element*,G4double> ( elH,  8.33*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  60.32*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  1.67*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  17.38*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 11.54*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elSi, 0.61*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.15*perCent ));


  }

  else if(Name=="LN450"){//0.45*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  8.330*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  60.32*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  1.670*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  17.38*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.150*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elSi, 0.610*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 11.54*perCent ));

  }
  else if(Name=="AP6Adipose"){//0.92*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  8.360*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  69.14*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.360*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  16.93*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.140*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elF,  3.070*perCent ));

  }
  else if (Name=="Breast"){//0.99*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  8.680*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  69.95*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.370*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  17.91*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.140*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 0.950*perCent ));

  } 
  else if (Name=="Water"){//1.0*g/cm3   

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  11.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  88.80*perCent ));

  } 
  else if (Name=="CTsolidwater"){//1.015*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  8.090*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  67.22*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.400*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  19.84*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.130*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 2.320*perCent ));

  }  
  else if (Name=="Brain"){//1.045*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.83*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  72.54*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  1.690*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  14.86*perCent ));    

  }
  else if (Name=="Liver"){//1.08*g/cm3 

    /*elVector.insert(std::pair< G4Element*,G4double> ( elH,  11.00*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  4.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  1.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  82.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 1.200*perCent ));*/
    elVector.insert(std::pair< G4Element*,G4double> ( elH,  8.060*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  67.01*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.470*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  20.01*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.140*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 2.310*perCent ));

  }  
  else if (Name=="InnerBone"){//1.12*g/cm3

    /*elVector.insert(std::pair< G4Element*,G4double> ( elH,  7.900*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  63.79*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  4.230*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  9.880*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 14.20*perCent ));*/
    elVector.insert(std::pair< G4Element*,G4double> ( elH,  6.670*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  55.64*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  1.906*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  23.52*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  3.230*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.110*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 8.860*perCent ));

  }
  else if (Name=="MineralBone"){//1.145*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  6.550*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  53.69*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.150*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  3.210*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elF,  16.74*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 17.66*perCent ));

  }
  else if (Name=="CB230"){//1.34*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  6.680*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  53.48*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.120*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  25.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.050*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 12.02*perCent ));

  }
  else if (Name=="CB250"){//1.56*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  4.770*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  41.63*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  1.520*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  31.99*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.080*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 20.03*perCent ));

  }
  else if (Name=="CorticalBone"){//1.84*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  3.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  31.26*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  0.990*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  37.57*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.050*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 27.03*perCent ));
  }

  ////  ////  ////  ////  //// 
  //   1  N. Hünemohr, H. Paganetti, S. Greilich, O. Jäkel, and J. Seco, “Tissue decomposition from dual energy CT data for MC based dose ca  //   lculation in particle therapy,” 
  //   Medical Physics 41(6), 061714 (2014).
  ////  ////  ////  ////  ////
  
  else if (Name=="LungInflated"){//0.26*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  10.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  3.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  75.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.200*perCent ));
    
  }
  else if (Name=="Adipose3"){//0.93*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  11.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  68.30*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  19.90*perCent ));
    
  }
  else if (Name=="Adipose2"){//0.95*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  11.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  60.00*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  0.700*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  27.90*perCent ));
    
  }
  else if (Name=="Adipose1"){//0.97*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  11.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  51.90*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  1.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  35.60*perCent ));

  }   
  else if (Name=="SoftTissue"){//1.03*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  25.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.700*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  60.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.200*perCent ));

  }
  else if (Name=="Muscle3"){//1.05*g/mc3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.30*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  11.30*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  3.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  75.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.200*perCent ));
    
  }
  else if (Name=="Liver3"){//1.07*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  12.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  3.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  73.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.300*perCent ));

  }
  else if (Name=="Skin2"){//1.09*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.10*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  20.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  4.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  65.00*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.100*perCent ));

  }
  else if (Name=="Cartilage"){//1.10*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  6.600*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  24.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  3.700*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  47.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 12.00*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  5.700*perCent ));
    
  }
  else if (Name=="ConnectiveTissue"){ //1.12*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  9.500*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  21.00*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  6.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  63.10*perCent ));

  }
  else if (Name=="HumerusWholeSpecimen"){//1.39*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  6.700*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  35.30*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.800*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  35.30*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 13.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  6.300*perCent ));

  }
  else if (Name=="Ribs2and6"){//1.41*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  6.400*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  26.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  3.900*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  43.90*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 13.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  6.000*perCent ));
    
  }  
  else if (Name=="FemurWholeSpecimen"){//1.43*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  6.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  33.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.900*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  36.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 14.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  6.600*perCent ));

    
  }
  else if (Name=="Ribs10"){//1.52*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  5.600*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  23.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  4.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  43.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 15.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  7.300*perCent ));

  }
  else if (Name=="Cranium"){//1.61*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  3.400*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  15.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  4.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  43.80*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 22.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  10.40*perCent ));
    
  }  
  else if (Name=="FemurCylindricalShaft"){//1.75*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  4.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  20.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  3.800*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  41.80*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 20.30*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  9.400*perCent ));

  }

  ////  ////  ////  ////  //// 
  //   Loma Linda Collaboration Material
  ////  ////  ////  ////  ////
  
  else if (Name=="SoftTissue_LL"){//1.03*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  25.60*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.700*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  60.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }

  else if (Name=="CorticalBone_LL"){//1.80*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  4.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  15.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  4.500*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  45.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  10.10*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 20.10*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }

  else if (Name=="TrabecularBone_LL"){//1.18*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  8.500*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  40.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.800*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  36.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  3.400*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 7.400*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.100*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }

  else if (Name=="SpinalDisc_LL"){//1.10*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  9.600*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  9.900*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  74.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.500*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  2.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.900*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }

  else if (Name=="BrainTissue_LL"){//1.04*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  10.70*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  14.50*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  2.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  71.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.400*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.200*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.300*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }

  else if (Name=="ToothEnamel_LL"){//2.89*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  0.950*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  1.110*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  0.230*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  41.66*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.790*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.230*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  18.71*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.340*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 35.97*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.020*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }

  else if (Name=="ToothDentin_LL"){//2.14*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  2.670*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  12.77*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  4.270*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  40.40*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.650*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.590*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  11.86*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.040*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 26.74*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.010*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }

  else if (Name=="SinusCavities_LL"){//0.00120*g/cm3

    elVector.insert(std::pair< G4Element*,G4double> ( elH,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elC,  0.010*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elN,  75.47*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elO,  23.20*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elNa, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elMg, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elP,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elS,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCl, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elAr, 1.280*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elK,  0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elCa, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elFe, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elZn, 0.000*perCent ));
    elVector.insert(std::pair< G4Element*,G4double> ( elBa, 0.000*perCent ));

  }
  else {
    cerr << "OrganicMaterial; no match for : " << Name << endl;
    return 0;

  }

  G4double massdensity = density;//ConvertToMassDensity(elVector,density);
  G4String name        = Form("%s_%.5f",Name.c_str(),massdensity);

  //Check if the materials is already been created
  std::map<G4String,G4Material*>::iterator it;
  it = theMaterialList.find(name);
  if(it != theMaterialList.end() )  return it->second;
  
  // Create and insert the material
  mat = new G4Material(name,massdensity*(g/cm3),nel=elVector.size(),kStateSolid);
  for(auto itr=elVector.begin(); itr!=elVector.end(); itr++) mat->AddElement(itr->first, itr->second);
  theMaterialList.insert(std::pair< G4String,G4Material*> ( name, mat ));

  return mat;
}

////////////////////////////////////////////////////////////////////////

G4double OrganicMaterial::ConvertToMassDensity(std::map<G4Element*, G4double> elementVector, G4double density){
  G4double Ng = 0;
  for(auto itr=elementVector.begin(); itr!=elementVector.end(); itr++){
    G4double frac = itr->second;
    G4double Z    = itr->first->GetZ();
    G4double A    = man->GetAtomicMassAmu(Z);
    Ng           += frac*(Z/A);
  }
  Ng*=Avogadro;   
  G4double massdensity = (density*water->GetElectronDensity()*1000)/Ng; // mg to g
  return massdensity;
}                 

////////////////////////////////////////////////////////////////////////

G4Material* OrganicMaterial::ConstructCompositeMaterial(G4String Name1,G4String Name2, G4double density, G4double ratio)
{

 G4Material* mat1     = (ConstructMaterial(Name1,density));
 G4Material* mat2     = (ConstructMaterial(Name2,density));
 G4double massdensity = (mat1->GetDensity()/(g/cm3))*(1-ratio) + (mat2->GetDensity()/(g/cm3))*(ratio);

 G4Material* compositeMat = new G4Material(Form("%s_%s_%.5f",Name1.data(),Name2.data(),massdensity ),massdensity*g/cm3,nel=2,kStateSolid);

 compositeMat->AddMaterial( mat1, 1-ratio);
 compositeMat->AddMaterial( mat2, ratio);

 return compositeMat;
}
