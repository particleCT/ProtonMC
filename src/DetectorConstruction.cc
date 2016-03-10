#include "G4NistManager.hh"
#include "G4EmCalculator.hh"
#include "DetectorConstruction.hh"
#include "G4RunManager.hh"
#include "TString.h"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4SubtractionSolid.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4Trap.hh"
#include "SensitiveDetector.hh"
#include "G4PVPlacement.hh"
#include "OrganicMaterial.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"

using namespace CLHEP;
DetectorConstruction* DetectorConstruction::theDetector=NULL;
DetectorConstruction::~DetectorConstruction()
{theDetector = NULL; }

DetectorConstruction::DetectorConstruction(G4String theModel,G4int angle,G4double thick)
: G4VUserDetectorConstruction()
{ 
  theThickness = thick;
  theAngle    = angle;
  theDetector = this;
  thePhantom  = theModel;

  theMaterial = OrganicMaterial::GetInstance();

  PhantomHalfX     = theThickness/2.*cm;
  PhantomHalfY     = 33./2.*cm;
  PhantomHalfZ     = 33./2.*cm;
  
}

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  water = theMaterial->water;
  G4Material* air   = theMaterial->ConstructMaterial("Air",0.0001025); 
  G4double world_size       = 3*m;  
  G4double PhantomPositionX = 0*cm;
  G4double PhantomPositionY = 0*cm;
  G4double PhantomPositionZ = 0*cm;  

  G4double InRadius         = 5.5 *cm;
  G4double OutRadius        = 10.5*cm;
  G4double InsertRadius     = 1.4*cm;

  //Cubic Water world
  G4Box* boxWorld = new G4Box("World",world_size,world_size,world_size);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(boxWorld, air,"World");                                    
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0,G4ThreeVector(),logicWorld,"physWorld",0,false,0);
  G4VisAttributes* world_att = new G4VisAttributes(G4Colour(1,0,0));
  world_att->SetVisibility(true);
  logicWorld->SetVisAttributes(world_att);

  G4RotationMatrix* rotTotal = new G4RotationMatrix();
  rotTotal->rotateY(theAngle*pi/180.);

  // Detectors 
  SensitiveDetector* sd1               = new SensitiveDetector("FrontTracker"); // Front Tracker
  G4Box* rad_vol1                      = new G4Box("rad_vol1",1.0*mm,PhantomHalfY,PhantomHalfZ);
  G4LogicalVolume * rad_log1           = new G4LogicalVolume(rad_vol1,air,"rad_log1",0,0,0);
  G4VisAttributes* sd_att = new G4VisAttributes(G4Colour(0,1,1));
  sd_att->SetVisibility(true);
  rad_log1->SetVisAttributes(sd_att);
  rad_log1->SetSensitiveDetector(sd1);
  new G4PVPlacement(0,G4ThreeVector(-1*theDetector->PhantomHalfX + 1*mm,0,0),"rad_phys1",rad_log1,physWorld,false,0);// 2.0 mm thick so the edge fit with the edge of the box

  SensitiveDetector* sd2               = new SensitiveDetector("RearTracker"); // Rear Tracker
  G4Box* rad_vol2                      = new G4Box("rad_vol2",1*mm,theDetector->PhantomHalfY,theDetector->PhantomHalfZ);
  G4LogicalVolume * rad_log2           = new G4LogicalVolume(rad_vol2,air,"rad_log2",0,0,0);
  rad_log2->SetVisAttributes(sd_att);
  rad_log2->SetSensitiveDetector(sd2);
  new G4PVPlacement(0,G4ThreeVector(theDetector->PhantomHalfX + 1*mm,0,0),"rad_phys2",rad_log2,physWorld,false,0);// 2.0 mm thick so the edge fit with the edge of the box

  // Container box      
  G4Box* box_vol = new G4Box("box_vol",PhantomHalfX,PhantomHalfY,PhantomHalfZ);
  G4LogicalVolume* box_log  = new G4LogicalVolume(box_vol,air,"box_log");
  G4PVPlacement*   box_phys = new G4PVPlacement(0,G4ThreeVector(),"box_phys",box_log,physWorld,false,0);
  G4VisAttributes* box_att  = new G4VisAttributes(G4Colour(0,1,0));
  box_att->SetVisibility(true);
  box_log->SetVisAttributes(box_att);

  //----------------------------------------------------------------------------------------------------------------
  // Water Tank phantom
  //----------------------------------------------------------------------------------------------------------------
  if(thePhantom == "WaterTank"){
    cout<<"here"<<endl;
    G4int NSlabs = 1000;
    G4double halfX  = PhantomHalfX/NSlabs;
    G4Box* slab_vol = new G4Box("box_vol",halfX,PhantomHalfY,PhantomHalfZ);
    G4LogicalVolume* slab_log  = new G4LogicalVolume(slab_vol,water,"box_log");
    G4VisAttributes* slab_att  = new G4VisAttributes(G4Colour(0,1,1));
    slab_att->SetVisibility(true);
    slab_log->SetVisAttributes(slab_att);
    for(int i=0;i<NSlabs;i++) new G4PVPlacement(0,G4ThreeVector(2*halfX*(i)-(NSlabs-1)*halfX,0,0),Form("Slabs_%d",i),slab_log,box_phys,true,i);        
  }

  //----------------------------------------------------------------------------------------------------------------
  // Gammex phantom
  //----------------------------------------------------------------------------------------------------------------

  else if(thePhantom == "Gammex"){
    
    //Plastic Material
    //std::string mat_outer[8]  = {"CB250","MineralBone","CorticalBone","CTsolidwater","LN300","CTsolidwater","Liver","CTsolidwater"};
    //double dens_outer[8] = {1.56   , 1.145       , 1.84         , 1.015        , 0.3   , 1.015        , 1.08  , 1.015};
    //std::string mat_inner[8]  = {"CB230","Water","Breast","LN450","Brain","AP6Adipose","InnerBone","CTsolidwater"};
    //double dens_inner[8] = {1.34   , 1.0   , 0.99   , 0.45  , 1.045 , 0.92       , 1.12      , 1.015};
    
    //Loma Linda Material
    std::string mat_outer[8]  = {"SoftTissue_LL","CorticalBone_LL","TrabecularBone_LL","SpinalDisc_LL","BrainTissue_LL","ToothEnamel_LL","ToothDentin_LL","SinusCavities_LL"};
    double dens_outer[8] = {1.03, 1.80, 1.18, 1.10, 1.04, 2.89, 2.14, 0.00120};
    std::string mat_inner[8]  = {"SoftTissue_LL","CorticalBone_LL","TrabecularBone_LL","SpinalDisc_LL","BrainTissue_LL","ToothEnamel_LL","ToothDentin_LL","SinusCavities_LL"};
    double dens_inner[8] = {1.03, 1.80, 1.18, 1.10, 1.04, 2.89, 2.14, 0.00120};

    //Tissue Material
    //std::string mat_inner[8] = {"Cartilage","HumerusWholeSpecimen","Ribs2and6","FemurWholeSpecimen","Ribs10","Cranium","FemurCylindricalShaft","ConnectiveTissue"};
    //std::string mat_inner[8] = {"Adipose3","Adipose2","Adipose1","SoftTissue","Muscle3","Liver3","Skin2","LungInflated"};
    
    //ExternalPhantom
    G4VSolid* ExtPhantom = new G4Tubs("ExtPhantom",0,PhantomHalfX,PhantomHalfX,0,2*pi);
    G4LogicalVolume *PhantomLog = new G4LogicalVolume(ExtPhantom,theMaterial->ConstructMaterial("CTsolidwater",1.015), "PhantomLog",0,0,0);
    
    // Inserts  
    G4VSolid* insert = new G4Tubs("insert",0,InsertRadius,PhantomHalfX,0,2*pi);
    G4VisAttributes* ins_att  = new G4VisAttributes(G4Colour(0,1,0));
    ins_att->SetVisibility(true);
    ins_att->SetForceSolid(true);
    
    G4VisAttributes* ins_att2  = new G4VisAttributes(G4Colour(1,1,0));
    ins_att2->SetVisibility(true);
    ins_att2->SetForceSolid(true);
    
    //Inner Circle
    for (int i = 0; i<8; i++){
      G4LogicalVolume* insert_log = new G4LogicalVolume(insert,theMaterial->ConstructMaterial(mat_inner[i],dens_inner[i]),mat_inner[i],0,0,0);    
      if(i==0)insert_log->SetVisAttributes(ins_att2);
      else insert_log->SetVisAttributes(ins_att);
      G4ThreeVector placement(0,InRadius,0);
      placement.rotateZ(pi/4-pi*i/4);
      new G4PVPlacement(0,placement,insert_log,mat_inner[i],PhantomLog,false,0);
    }
    //Outer Circle
    for (int i = 0; i<8; i++){
      G4LogicalVolume* insert_log = new G4LogicalVolume(insert,theMaterial->ConstructMaterial(mat_outer[i],dens_outer[i]),mat_outer[i],0,0,0);
      insert_log->SetVisAttributes(ins_att);
      G4ThreeVector placement(0,OutRadius,0);
      placement.rotateZ(3*pi/8-pi*i/4);
      new G4PVPlacement(0,placement,insert_log,mat_outer[i],PhantomLog,false,0);
    }

    G4RotationMatrix* rot1 = new G4RotationMatrix();
    rot1->rotateZ(pi/4 + theAngle*pi/8.);
    new G4PVPlacement(rot1,G4ThreeVector(PhantomPositionX,PhantomPositionY,PhantomPositionZ),"Phantom",PhantomLog,box_phys,false,0);
  }

  //----------------------------------------------------------------------------------------------------------------
  // Wedge phantom
  //----------------------------------------------------------------------------------------------------------------

  else if (thePhantom =="Wedge"){
    // Wedge 1 & 2

    //1
    G4Trap* wedge_1 =  new G4Trap("wedge",33*cm,33*cm,16.5*cm,0.0001*cm);
    G4LogicalVolume* wedg1_log  = new G4LogicalVolume(wedge_1,theMaterial->ConstructMaterial("SoftTissue",1.03),"wedg1_log");
    G4RotationMatrix *rot1 = new G4RotationMatrix(0,180*degree,0*degree);
    new G4PVPlacement(rot1,G4ThreeVector(4.125*cm,0,0),"wedg1_phys",wedg1_log,box_phys,false,0);
    G4VisAttributes* wedg1_att  = new G4VisAttributes(G4Colour(0,0,1));
    wedg1_att->SetVisibility(true);
    wedg1_att->SetForceSolid(true);
    wedg1_log->SetVisAttributes(wedg1_att);

    //2
    G4Trap* wedge_2 =  new G4Trap("wedge",33*cm,33*cm,16.5*cm,0.0001*cm);
    G4LogicalVolume* wedg2_log  = new G4LogicalVolume(wedge_2,theMaterial->ConstructMaterial("LungInflated",0.26),"wedg2_log");
    G4RotationMatrix *rot2 = new G4RotationMatrix(0,180*degree,180*degree);
    new G4PVPlacement(rot2,G4ThreeVector(12.375*cm,0,0),"wedg2_phys",wedg2_log,box_phys,false,0);
    G4VisAttributes* wedg2_att  = new G4VisAttributes(G4Colour(1,0,0));
    wedg2_att->SetVisibility(true);
    wedg2_att->SetForceSolid(true);
    wedg2_log->SetVisAttributes(wedg2_att);
    
    // Wedge 3 & 4

    //3
    G4Trap* _wedge3 =  new G4Trap("wedge",33*cm,16.5*cm,16.5*cm,0.0001*cm);
    G4LogicalVolume* wedg3_log  = new G4LogicalVolume(_wedge3,theMaterial->ConstructMaterial("HumerusWholeSpecimen",1.39),"wedg3_log");
    G4RotationMatrix *rot3 = new G4RotationMatrix(0,0,180*degree);
    new G4PVPlacement(rot3,G4ThreeVector(-4.125*cm,8.25*cm,0),"wedg3_phys",wedg3_log,box_phys,false,0);
    G4VisAttributes* wedg3_att  = new G4VisAttributes(G4Colour(1,1,1));
    wedg3_att->SetVisibility(true);
    wedg3_att->SetForceSolid(true);
    wedg3_log->SetVisAttributes(wedg3_att);

    //4
    G4Trap* _wedge4 =  new G4Trap("wedge",33*cm,16.5*cm,16.5*cm,0.0001*cm);
    G4LogicalVolume* wedg4_log  = new G4LogicalVolume(_wedge4,theMaterial->ConstructMaterial("HumerusWholeSpecimen",1.39),"wedg4_log");
    G4RotationMatrix *rot4 = new G4RotationMatrix(0,0,90*degree);
    new G4PVPlacement(rot4,G4ThreeVector(-8.25*cm,-12.375*cm,0),"wedg4_phys",wedg4_log,box_phys,false,0);
    G4VisAttributes* wedg4_att  = new G4VisAttributes(G4Colour(1,1,1));
    wedg4_att->SetVisibility(true);
    wedg4_att->SetForceSolid(true);
    wedg4_log->SetVisAttributes(wedg4_att);

    // Wedge 5 & 6

    //5
    G4Trap* _wedge5 =  new G4Trap("wedge",33*cm,16.5*cm,16.5*cm,0.0001*cm);
    G4LogicalVolume* wedg5_log  = new G4LogicalVolume(_wedge5,theMaterial->ConstructMaterial("Adipose3",0.93),"wedg5_log");
    G4RotationMatrix *rot5 = new G4RotationMatrix(0,0,270*degree);    
    new G4PVPlacement(rot5,G4ThreeVector(-8.25*cm,-4.125*cm,0),"wedg5_phys",wedg5_log,box_phys,false,0);
    G4VisAttributes* wedg5_att  = new G4VisAttributes(G4Colour(1,0,1));
    wedg5_att->SetVisibility(true);
    wedg5_att->SetForceSolid(true);
    wedg5_log->SetVisAttributes(wedg5_att);

    //6
    G4Trap* _wedge6 =  new G4Trap("wedge",33*cm,16.5*cm,16.5*cm,0.0001*cm);
    G4LogicalVolume* wedg6_log  = new G4LogicalVolume(_wedge6,theMaterial->ConstructMaterial("Adipose3",0.93),"wedg6_log");
    new G4PVPlacement(0,G4ThreeVector(-12.375*cm,8.25*cm,0),"wedg6_phys",wedg6_log,box_phys,false,0);
    G4VisAttributes* wedg6_att  = new G4VisAttributes(G4Colour(1,0,1));
    wedg6_att->SetVisibility(true);
    wedg6_att->SetForceSolid(true);
    wedg6_log->SetVisAttributes(wedg6_att);

  }

  //----------------------------------------------------------------------------------------------------------------
  // MTF phantom
  //----------------------------------------------------------------------------------------------------------------
  else if (thePhantom =="MTF"){
    G4VisAttributes* linepair_att  = new G4VisAttributes(G4Colour(0,0,1));
    linepair_att->SetVisibility(true);
    linepair_att->SetForceSolid(true);
    G4Material *bone = theMaterial->ConstructMaterial("CB250",1.76);
    
    // 1lp\cm
    G4double halfX1 = (5./1.)/2.;
    G4Box* linePairBox1 = new G4Box("LinePair",2*cm,halfX1*mm,4*cm);
    G4LogicalVolume* linePairLog1 = new G4LogicalVolume(linePairBox1,bone ,"linePairLog");
    linePairLog1->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,140*mm,-7.5*cm),"linePairPhys",linePairLog1,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(140-4*halfX1)*mm,-7.5*cm),"linePairPhys",linePairLog1,box_phys,false,0);

    // 2lp\cm
    G4double halfX2 = (5./2)/2.;
    G4Box* linePairBox2 = new G4Box("LinePair",2*cm,halfX2*mm,4*cm);
    G4LogicalVolume* linePairLog2 = new G4LogicalVolume(linePairBox2,bone ,"linePairLog");
    linePairLog2->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,120*mm,-7.5*cm),"linePairPhys",linePairLog2,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(120-4*halfX2)*mm,-7.5*cm),"linePairPhys",linePairLog2,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(120-8*halfX2)*mm,-7.5*cm),"linePairPhys",linePairLog2,box_phys,false,0);


    // 3lp\cm
    G4double halfX3 = (5./3.)/2.;
    G4Box* linePairBox3 = new G4Box("LinePair",2*cm,halfX3*mm,4*cm);
    G4LogicalVolume* linePairLog3 = new G4LogicalVolume(linePairBox3,bone ,"linePairLog");
    linePairLog3->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,100*mm,-7.5*cm),"linePairPhys",linePairLog3,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-4*halfX3)*mm,-7.5*cm),"linePairPhys",linePairLog3,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-8*halfX3)*mm,-7.5*cm),"linePairPhys",linePairLog3,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-12*halfX3)*mm,-7.5*cm),"linePairPhys",linePairLog3,box_phys,false,0);

    // 4lp\cm
    G4double halfX4 = (5./4.)/2.;
    G4Box* linePairBox4 = new G4Box("LinePair",2*cm,halfX4*mm,4*cm);
    G4LogicalVolume* linePairLog4 = new G4LogicalVolume(linePairBox4,bone ,"linePairLog");
    linePairLog4->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,80*mm,-7.5*cm),"linePairPhys",linePairLog4,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-4*halfX4)*mm,-7.5*cm),"linePairPhys",linePairLog4,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-8*halfX4)*mm,-7.5*cm),"linePairPhys",linePairLog4,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-12*halfX4)*mm,-7.5*cm),"linePairPhys",linePairLog4,box_phys,false,0);
    
    //5lp\cm
    G4double halfX5 = (5./5.)/2.;
    G4Box* linePairBox5 = new G4Box("LinePair",2*cm,halfX5*mm,4*cm);
    G4LogicalVolume* linePairLog5 = new G4LogicalVolume(linePairBox5,bone ,"linePairLog");
    linePairLog5->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,60*mm,-7.5*cm),"linePairPhys",linePairLog5,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-4*halfX5)*mm,-7.5*cm),"linePairPhys",linePairLog5,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-8*halfX5)*mm,-7.5*cm),"linePairPhys",linePairLog5,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-12*halfX5)*mm,-7.5*cm),"linePairPhys",linePairLog5,box_phys,false,0);

    //6lp\cm
    G4double halfX6 = (5./6.)/2.;
    G4Box* linePairBox6 = new G4Box("LinePair",2*cm,halfX6*mm,4*cm);
    G4LogicalVolume* linePairLog6 = new G4LogicalVolume(linePairBox6,bone ,"linePairLog");
    linePairLog6->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,40*mm,-7.5*cm),"linePairPhys",linePairLog6,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-4*halfX6)*mm,-7.5*cm),"linePairPhys",linePairLog6,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-8*halfX6)*mm,-7.5*cm),"linePairPhys",linePairLog6,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-12*halfX6)*mm,-7.5*cm),"linePairPhys",linePairLog6,box_phys,false,0);

    //7lp\cm
    G4double halfX7 = (5./7.)/2.;
    G4Box* linePairBox7 = new G4Box("LinePair",2*cm,halfX7*mm,4*cm);
    G4LogicalVolume* linePairLog7 = new G4LogicalVolume(linePairBox7,bone ,"linePairLog");
    linePairLog7->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,20*mm,-7.5*cm),"linePairPhys",linePairLog7,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-4*halfX7)*mm,-7.5*cm),"linePairPhys",linePairLog7,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-8*halfX7)*mm,-7.5*cm),"linePairPhys",linePairLog7,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-12*halfX7)*mm,-7.5*cm),"linePairPhys",linePairLog7,box_phys,false,0);

    //8lp\cm
    G4double halfX8 = (5./8.)/2.;
    G4Box* linePairBox8 = new G4Box("LinePair",2*cm,halfX8*mm,4*cm);
    G4LogicalVolume* linePairLog8 = new G4LogicalVolume(linePairBox8,bone ,"linePairLog");
    linePairLog8->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,0*mm,-7.5*cm),"linePairPhys",linePairLog8,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-4*halfX8)*mm,-7.5*cm),"linePairPhys",linePairLog8,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-8*halfX8)*mm,-7.5*cm),"linePairPhys",linePairLog8,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-12*halfX8)*mm,-7.5*cm),"linePairPhys",linePairLog8,box_phys,false,0);

    //9lp\cm
    G4double halfX9 = (5./9.)/2.;
    G4Box* linePairBox9 = new G4Box("LinePair",2*cm,halfX9*mm,4*cm);
    G4LogicalVolume* linePairLog9 = new G4LogicalVolume(linePairBox9,bone ,"linePairLog");
    linePairLog9->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-20*mm,-7.5*cm),"linePairPhys",linePairLog9,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-4*halfX9)*mm,-7.5*cm),"linePairPhys",linePairLog9,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-8*halfX9)*mm,-7.5*cm),"linePairPhys",linePairLog9,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-12*halfX9)*mm,-7.5*cm),"linePairPhys",linePairLog9,box_phys,false,0);

    //10lp\cm
    G4double halfX10 = (5./10.)/2.;
    G4Box* linePairBox10 = new G4Box("LinePair",2*cm,halfX10*mm,4*cm);
    G4LogicalVolume* linePairLog10 = new G4LogicalVolume(linePairBox10,bone ,"linePairLog");
    linePairLog10->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-40*mm,-7.5*cm),"linePairPhys",linePairLog10,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-4*halfX10)*mm,-7.5*cm),"linePairPhys",linePairLog10,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-8*halfX10)*mm,-7.5*cm),"linePairPhys",linePairLog10,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-12*halfX10)*mm,-7.5*cm),"linePairPhys",linePairLog10,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-16*halfX10)*mm,-7.5*cm),"linePairPhys",linePairLog10,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-20*halfX10)*mm,-7.5*cm),"linePairPhys",linePairLog10,box_phys,false,0);

    //11lp\cm
    G4double halfX11 = (5./11.)/2.;
    G4Box* linePairBox11 = new G4Box("LinePair",2*cm,halfX11*mm,4*cm);
    G4LogicalVolume* linePairLog11 = new G4LogicalVolume(linePairBox11,bone ,"linePairLog");
    linePairLog11->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-60*mm,-7.5*cm),"linePairPhys",linePairLog11,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-4*halfX11)*mm,-7.5*cm),"linePairPhys",linePairLog11,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-8*halfX11)*mm,-7.5*cm),"linePairPhys",linePairLog11,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-12*halfX11)*mm,-7.5*cm),"linePairPhys",linePairLog11,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-16*halfX11)*mm,-7.5*cm),"linePairPhys",linePairLog11,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-20*halfX11)*mm,-7.5*cm),"linePairPhys",linePairLog11,box_phys,false,0);

    //12lp\cm
    G4double halfX12 = (5./12.)/2.;
    G4Box* linePairBox12 = new G4Box("LinePair",2*cm,halfX12*mm,4*cm);
    G4LogicalVolume* linePairLog12 = new G4LogicalVolume(linePairBox12,bone ,"linePairLog");
    linePairLog12->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-80*mm,-7.5*cm),"linePairPhys",linePairLog12,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-4*halfX12)*mm,-7.5*cm),"linePairPhys",linePairLog12,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-8*halfX12)*mm,-7.5*cm),"linePairPhys",linePairLog12,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-12*halfX12)*mm,-7.5*cm),"linePairPhys",linePairLog12,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-16*halfX12)*mm,-7.5*cm),"linePairPhys",linePairLog12,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-20*halfX12)*mm,-7.5*cm),"linePairPhys",linePairLog12,box_phys,false,0);

    //13lp\cm
    G4double halfX13 = (5./13.)/2.;
    G4Box* linePairBox13 = new G4Box("LinePair",2*cm,halfX13*mm,4*cm);
    G4LogicalVolume* linePairLog13 = new G4LogicalVolume(linePairBox13,bone ,"linePairLog");
    linePairLog13->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-100*mm,-7.5*cm),"linePairPhys",linePairLog13,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-4*halfX13)*mm,-7.5*cm),"linePairPhys",linePairLog13,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-8*halfX13)*mm,-7.5*cm),"linePairPhys",linePairLog13,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-12*halfX13)*mm,-7.5*cm),"linePairPhys",linePairLog13,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-16*halfX13)*mm,-7.5*cm),"linePairPhys",linePairLog13,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-20*halfX13)*mm,-7.5*cm),"linePairPhys",linePairLog13,box_phys,false,0);

    //14lp\cm
    G4double halfX14 = (5./14.)/2.;
    G4Box* linePairBox14 = new G4Box("LinePair",2*cm,halfX14*mm,4*cm);
    G4LogicalVolume* linePairLog14 = new G4LogicalVolume(linePairBox14,bone ,"linePairLog");
    linePairLog14->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-120*mm,-7.5*cm),"linePairPhys",linePairLog14,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-4*halfX14)*mm,-7.5*cm),"linePairPhys",linePairLog14,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-8*halfX14)*mm,-7.5*cm),"linePairPhys",linePairLog14,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-12*halfX14)*mm,-7.5*cm),"linePairPhys",linePairLog14,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-16*halfX14)*mm,-7.5*cm),"linePairPhys",linePairLog14,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-20*halfX14)*mm,-7.5*cm),"linePairPhys",linePairLog14,box_phys,false,0);

    //15lp\cm
    G4double halfX15 = (5./15.)/2.;
    G4Box* linePairBox15 = new G4Box("LinePair",2*cm,halfX15*mm,4*cm);
    G4LogicalVolume* linePairLog15 = new G4LogicalVolume(linePairBox15,bone ,"linePairLog");
    linePairLog15->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-140*mm,-7.5*cm),"linePairPhys",linePairLog15,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-4*halfX15)*mm,-7.5*cm),"linePairPhys",linePairLog15,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-8*halfX15)*mm,-7.5*cm),"linePairPhys",linePairLog15,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-12*halfX15)*mm,-7.5*cm),"linePairPhys",linePairLog15,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-16*halfX15)*mm,-7.5*cm),"linePairPhys",linePairLog15,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-20*halfX15)*mm,-7.5*cm),"linePairPhys",linePairLog15,box_phys,false,0);

    //16lp\cm
    G4double halfX16 = (5./16.)/2.;
    G4Box* linePairBox16 = new G4Box("LinePair",2*cm,halfX16*mm,4*cm);
    G4LogicalVolume* linePairLog16 = new G4LogicalVolume(linePairBox16,bone ,"linePairLog");
    linePairLog16->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,140*mm,7.5*cm),"linePairPhys",linePairLog16,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(140-4*halfX16)*mm,7.5*cm),"linePairPhys",linePairLog16,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(140-8*halfX16)*mm,7.5*cm),"linePairPhys",linePairLog16,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(140-12*halfX16)*mm,7.5*cm),"linePairPhys",linePairLog16,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(140-16*halfX16)*mm,7.5*cm),"linePairPhys",linePairLog16,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(140-20*halfX16)*mm,7.5*cm),"linePairPhys",linePairLog16,box_phys,false,0);

    //17lp\cm
    G4double halfX17 = (5./17.)/2.;
    G4Box* linePairBox17 = new G4Box("LinePair",2*cm,halfX17*mm,4*cm);
    G4LogicalVolume* linePairLog17 = new G4LogicalVolume(linePairBox17,bone ,"linePairLog");
    linePairLog17->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,120*mm,7.5*cm),"linePairPhys",linePairLog17,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(120-4*halfX17)*mm,7.5*cm),"linePairPhys",linePairLog17,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(120-8*halfX17)*mm,7.5*cm),"linePairPhys",linePairLog17,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(120-12*halfX17)*mm,7.5*cm),"linePairPhys",linePairLog17,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(120-16*halfX17)*mm,7.5*cm),"linePairPhys",linePairLog17,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(120-20*halfX17)*mm,7.5*cm),"linePairPhys",linePairLog17,box_phys,false,0);

    //18lp\cm
    G4double halfX18 = (5./18.)/2.;
    G4Box* linePairBox18 = new G4Box("LinePair",2*cm,halfX18*mm,4*cm);
    G4LogicalVolume* linePairLog18 = new G4LogicalVolume(linePairBox18,bone ,"linePairLog");
    linePairLog18->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,100*mm,7.5*cm),"linePairPhys",linePairLog18,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-4*halfX18)*mm,7.5*cm),"linePairPhys",linePairLog18,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-8*halfX18)*mm,7.5*cm),"linePairPhys",linePairLog18,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-12*halfX18)*mm,7.5*cm),"linePairPhys",linePairLog18,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-16*halfX18)*mm,7.5*cm),"linePairPhys",linePairLog18,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(100-20*halfX18)*mm,7.5*cm),"linePairPhys",linePairLog18,box_phys,false,0);

    //19lp\cm
    G4double halfX19 = (5./19.)/2.;
    G4Box* linePairBox19 = new G4Box("LinePair",2*cm,halfX19*mm,4*cm);
    G4LogicalVolume* linePairLog19 = new G4LogicalVolume(linePairBox19,bone ,"linePairLog");
    linePairLog19->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,80*mm,7.5*cm),"linePairPhys",linePairLog19,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-4*halfX19)*mm,7.5*cm),"linePairPhys",linePairLog19,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-8*halfX19)*mm,7.5*cm),"linePairPhys",linePairLog19,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-12*halfX19)*mm,7.5*cm),"linePairPhys",linePairLog19,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-16*halfX19)*mm,7.5*cm),"linePairPhys",linePairLog19,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(80-20*halfX19)*mm,7.5*cm),"linePairPhys",linePairLog19,box_phys,false,0);

    //20lp\cm
    G4double halfX20 = (5./20.)/2.;
    G4Box* linePairBox20 = new G4Box("LinePair",2*cm,halfX20*mm,4*cm);
    G4LogicalVolume* linePairLog20 = new G4LogicalVolume(linePairBox20,bone ,"linePairLog");
    linePairLog20->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,60*mm,7.5*cm),"linePairPhys",linePairLog20,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-4*halfX20)*mm,7.5*cm),"linePairPhys",linePairLog20,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-8*halfX20)*mm,7.5*cm),"linePairPhys",linePairLog20,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-12*halfX20)*mm,7.5*cm),"linePairPhys",linePairLog20,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-16*halfX20)*mm,7.5*cm),"linePairPhys",linePairLog20,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(60-20*halfX20)*mm,7.5*cm),"linePairPhys",linePairLog20,box_phys,false,0);

    //21lp\cm
    G4double halfX21 = (5./21.)/2.;
    G4Box* linePairBox21 = new G4Box("LinePair",2*cm,halfX21*mm,4*cm);
    G4LogicalVolume* linePairLog21 = new G4LogicalVolume(linePairBox21,bone ,"linePairLog");
    linePairLog21->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,40*mm,7.5*cm),"linePairPhys",linePairLog21,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-4*halfX21)*mm,7.5*cm),"linePairPhys",linePairLog21,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-8*halfX21)*mm,7.5*cm),"linePairPhys",linePairLog21,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-12*halfX21)*mm,7.5*cm),"linePairPhys",linePairLog21,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-16*halfX21)*mm,7.5*cm),"linePairPhys",linePairLog21,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(40-20*halfX21)*mm,7.5*cm),"linePairPhys",linePairLog21,box_phys,false,0);

    //22lp\cm
    G4double halfX22 = (5./22.)/2.;
    G4Box* linePairBox22 = new G4Box("LinePair",2*cm,halfX22*mm,4*cm);
    G4LogicalVolume* linePairLog22 = new G4LogicalVolume(linePairBox22,bone ,"linePairLog");
    linePairLog22->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,20*mm,7.5*cm),"linePairPhys",linePairLog22,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-4*halfX22)*mm,7.5*cm),"linePairPhys",linePairLog22,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-8*halfX22)*mm,7.5*cm),"linePairPhys",linePairLog22,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-12*halfX22)*mm,7.5*cm),"linePairPhys",linePairLog22,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-16*halfX22)*mm,7.5*cm),"linePairPhys",linePairLog22,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(20-20*halfX22)*mm,7.5*cm),"linePairPhys",linePairLog22,box_phys,false,0);

    //23lp\cm
    G4double halfX23 = (5./23.)/2.;
    G4Box* linePairBox23 = new G4Box("LinePair",2*cm,halfX23*mm,4*cm);
    G4LogicalVolume* linePairLog23 = new G4LogicalVolume(linePairBox23,bone ,"linePairLog");
    linePairLog23->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,0*mm,7.5*cm),"linePairPhys",linePairLog23,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-4*halfX23)*mm,7.5*cm),"linePairPhys",linePairLog23,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-8*halfX23)*mm,7.5*cm),"linePairPhys",linePairLog23,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-12*halfX23)*mm,7.5*cm),"linePairPhys",linePairLog23,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-16*halfX23)*mm,7.5*cm),"linePairPhys",linePairLog23,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(0-20*halfX23)*mm,7.5*cm),"linePairPhys",linePairLog23,box_phys,false,0);

    //24lp\cm
    G4double halfX24 = (5./24.)/2.;
    G4Box* linePairBox24 = new G4Box("LinePair",2*cm,halfX24*mm,4*cm);
    G4LogicalVolume* linePairLog24 = new G4LogicalVolume(linePairBox24,bone ,"linePairLog");
    linePairLog24->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-20*mm,7.5*cm),"linePairPhys",linePairLog24,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-4*halfX24)*mm,7.5*cm),"linePairPhys",linePairLog24,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-8*halfX24)*mm,7.5*cm),"linePairPhys",linePairLog24,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-12*halfX24)*mm,7.5*cm),"linePairPhys",linePairLog24,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-16*halfX24)*mm,7.5*cm),"linePairPhys",linePairLog24,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-20-20*halfX24)*mm,7.5*cm),"linePairPhys",linePairLog24,box_phys,false,0);

    //25lp\cm
    G4double halfX25 = (5./25.)/2.;
    G4Box* linePairBox25 = new G4Box("LinePair",2*cm,halfX25*mm,4*cm);
    G4LogicalVolume* linePairLog25 = new G4LogicalVolume(linePairBox25,bone ,"linePairLog");
    linePairLog25->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-40*mm,7.5*cm),"linePairPhys",linePairLog25,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-4*halfX25)*mm,7.5*cm),"linePairPhys",linePairLog25,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-8*halfX25)*mm,7.5*cm),"linePairPhys",linePairLog25,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-12*halfX25)*mm,7.5*cm),"linePairPhys",linePairLog25,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-16*halfX25)*mm,7.5*cm),"linePairPhys",linePairLog25,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-40-20*halfX25)*mm,7.5*cm),"linePairPhys",linePairLog25,box_phys,false,0);

    //26lp\cm
    G4double halfX26 = (5./26.)/2.;
    G4Box* linePairBox26 = new G4Box("LinePair",2*cm,halfX26*mm,4*cm);
    G4LogicalVolume* linePairLog26 = new G4LogicalVolume(linePairBox26,bone ,"linePairLog");
    linePairLog26->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-60*mm,7.5*cm),"linePairPhys",linePairLog26,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-4*halfX26)*mm,7.5*cm),"linePairPhys",linePairLog26,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-8*halfX26)*mm,7.5*cm),"linePairPhys",linePairLog26,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-12*halfX26)*mm,7.5*cm),"linePairPhys",linePairLog26,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-16*halfX26)*mm,7.5*cm),"linePairPhys",linePairLog26,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-60-20*halfX26)*mm,7.5*cm),"linePairPhys",linePairLog26,box_phys,false,0);

    //27lp\cm
    G4double halfX27 = (5./27.)/2.;
    G4Box* linePairBox27 = new G4Box("LinePair",2*cm,halfX27*mm,4*cm);
    G4LogicalVolume* linePairLog27 = new G4LogicalVolume(linePairBox27,bone ,"linePairLog");
    linePairLog27->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-80*mm,7.5*cm),"linePairPhys",linePairLog27,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-4*halfX27)*mm,7.5*cm),"linePairPhys",linePairLog27,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-8*halfX27)*mm,7.5*cm),"linePairPhys",linePairLog27,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-12*halfX27)*mm,7.5*cm),"linePairPhys",linePairLog27,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-16*halfX27)*mm,7.5*cm),"linePairPhys",linePairLog27,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-80-20*halfX27)*mm,7.5*cm),"linePairPhys",linePairLog27,box_phys,false,0);

    //28lp\cm
    G4double halfX28 = (5./28.)/2.;
    G4Box* linePairBox28 = new G4Box("LinePair",2*cm,halfX28*mm,4*cm);
    G4LogicalVolume* linePairLog28 = new G4LogicalVolume(linePairBox28,bone ,"linePairLog");
    linePairLog28->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-100*mm,7.5*cm),"linePairPhys",linePairLog28,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-4*halfX28)*mm,7.5*cm),"linePairPhys",linePairLog28,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-8*halfX28)*mm,7.5*cm),"linePairPhys",linePairLog28,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-12*halfX28)*mm,7.5*cm),"linePairPhys",linePairLog28,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-16*halfX28)*mm,7.5*cm),"linePairPhys",linePairLog28,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-100-20*halfX28)*mm,7.5*cm),"linePairPhys",linePairLog28,box_phys,false,0);

    //29lp\cm
    G4double halfX29 = (5./29.)/2.;
    G4Box* linePairBox29 = new G4Box("LinePair",2*cm,halfX29*mm,4*cm);
    G4LogicalVolume* linePairLog29 = new G4LogicalVolume(linePairBox29,bone ,"linePairLog");
    linePairLog29->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-120*mm,7.5*cm),"linePairPhys",linePairLog29,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-4*halfX29)*mm,7.5*cm),"linePairPhys",linePairLog29,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-8*halfX29)*mm,7.5*cm),"linePairPhys",linePairLog29,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-12*halfX29)*mm,7.5*cm),"linePairPhys",linePairLog29,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-16*halfX29)*mm,7.5*cm),"linePairPhys",linePairLog29,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-120-20*halfX29)*mm,7.5*cm),"linePairPhys",linePairLog29,box_phys,false,0);

    //30lp\cm
    G4double halfX30 = (5./30.)/2.;
    G4Box* linePairBox30 = new G4Box("LinePair",2*cm,halfX30*mm,4*cm);
    G4LogicalVolume* linePairLog30 = new G4LogicalVolume(linePairBox30,bone ,"linePairLog");
    linePairLog30->SetVisAttributes(linepair_att);

    new G4PVPlacement(0,G4ThreeVector(0*cm,-140*mm,7.5*cm),"linePairPhys",linePairLog30,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-4*halfX30)*mm,7.5*cm),"linePairPhys",linePairLog30,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-8*halfX30)*mm,7.5*cm),"linePairPhys",linePairLog30,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-12*halfX30)*mm,7.5*cm),"linePairPhys",linePairLog30,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-16*halfX30)*mm,7.5*cm),"linePairPhys",linePairLog30,box_phys,false,0);
    new G4PVPlacement(0,G4ThreeVector(0*cm,(-140-20*halfX30)*mm,7.5*cm),"linePairPhys",linePairLog30,box_phys,false,0);


    
  }

  //----------------------------------------------------------------------------------------------------------------
  // Constrast phantom
  //----------------------------------------------------------------------------------------------------------------
  else if (thePhantom =="LasVegas"){

    G4VisAttributes* linepair_att  = new G4VisAttributes(G4Colour(0,0,1));
    linepair_att->SetVisibility(true);
    //linepair_att->SetForceSolid(true);
    G4Material *bone = theMaterial->ConstructMaterial("CB250",1.76);

    G4Box* LasVegasBox = new G4Box("LasVegasBox",1*cm,10*cm,10*cm);
    G4RotationMatrix* rot1 = new G4RotationMatrix();
    rot1->rotateY(pi/2.);

    G4Tubs* insert_0_0 = new G4Tubs("insert_0_0",0, (0.5/2)*mm , (0.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_0_0 = new G4SubtractionSolid("sub_0_0",LasVegasBox,insert_0_0, rot1 , G4ThreeVector(-1.*cm,-6*cm,-7.5*cm));

    G4Tubs* insert_0_1 = new G4Tubs("insert_0_1",0, (2/2)*mm , (0.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_0_1 = new G4SubtractionSolid("sub_0_1",sub_0_0,insert_0_1, rot1 , G4ThreeVector(-1.*cm,-6*cm,-4.5*cm));

    G4Tubs* insert_0_2 = new G4Tubs("insert_0_2",0, (4/2)*mm , (0.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_0_2 = new G4SubtractionSolid("sub_0_2",sub_0_1,insert_0_2, rot1 , G4ThreeVector(-1.*cm,-6*cm,-1.5*cm));

    G4Tubs* insert_0_3 = new G4Tubs("insert_0_3",0, (7/2)*mm , (0.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_0_3 = new G4SubtractionSolid("sub_0_3",sub_0_2,insert_0_3, rot1 , G4ThreeVector(-1.*cm,-6*cm,1.5*cm));

    G4Tubs* insert_0_4 = new G4Tubs("insert_0_4",0, (10/2)*mm , (0.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_0_4 = new G4SubtractionSolid("sub_0_4",sub_0_3,insert_0_4, rot1 , G4ThreeVector(-1.*cm,-6*cm,4.5*cm));

    G4Tubs* insert_0_5 = new G4Tubs("insert_0_5",0, (15/2)*mm , (0.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_0_5 = new G4SubtractionSolid("sub_0_5",sub_0_4,insert_0_5, rot1 , G4ThreeVector(-1.*cm,-6*cm,7.5*cm));

    G4Tubs* insert_1_0 = new G4Tubs("insert_1_0",0, (0.5/2)*mm , (1.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_1_0 = new G4SubtractionSolid("sub_1_0",sub_0_5,insert_1_0, rot1 , G4ThreeVector(-1.*cm,-3*cm,-7.5*cm));

    G4Tubs* insert_1_1 = new G4Tubs("insert_1_1",0, (2/2)*mm , (1.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_1_1 = new G4SubtractionSolid("sub_1_1",sub_1_0,insert_1_1, rot1 , G4ThreeVector(-1.*cm,-3*cm,-4.5*cm));

    G4Tubs* insert_1_2 = new G4Tubs("insert_1_2",0, (4/2)*mm , (1.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_1_2 = new G4SubtractionSolid("sub_1_2",sub_1_1,insert_1_2, rot1 , G4ThreeVector(-1.*cm,-3*cm,-1.5*cm));

    G4Tubs* insert_1_3 = new G4Tubs("insert_1_3",0, (7/2)*mm , (1.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_1_3 = new G4SubtractionSolid("sub_1_3",sub_1_2,insert_1_3, rot1 , G4ThreeVector(-1.*cm,-3*cm,1.5*cm));

    G4Tubs* insert_1_4 = new G4Tubs("insert_1_4",0, (10/2)*mm , (1.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_1_4 = new G4SubtractionSolid("sub_1_4",sub_1_3,insert_1_4, rot1 , G4ThreeVector(-1.*cm,-3*cm,4.5*cm));

    G4Tubs* insert_1_5 = new G4Tubs("insert_1_5",0, (15/2)*mm , (1.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_1_5 = new G4SubtractionSolid("sub_1_5",sub_1_4,insert_1_5, rot1 , G4ThreeVector(-1.*cm,-3*cm,7.5*cm));

    G4Tubs* insert_2_0 = new G4Tubs("insert_2_0",0, (0.5/2)*mm , (2.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_2_0 = new G4SubtractionSolid("sub_2_0",sub_1_5,insert_2_0, rot1 , G4ThreeVector(-1.*cm,0*cm,-7.5*cm));

    G4Tubs* insert_2_1 = new G4Tubs("insert_2_1",0, (2/2)*mm , (2.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_2_1 = new G4SubtractionSolid("sub_2_1",sub_2_0,insert_2_1, rot1 , G4ThreeVector(-1.*cm,0*cm,-4.5*cm));

    G4Tubs* insert_2_2 = new G4Tubs("insert_2_2",0, (4/2)*mm , (2.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_2_2 = new G4SubtractionSolid("sub_2_2",sub_2_1,insert_2_2, rot1 , G4ThreeVector(-1.*cm,0*cm,-1.5*cm));

    G4Tubs* insert_2_3 = new G4Tubs("insert_2_3",0, (7/2)*mm , (2.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_2_3 = new G4SubtractionSolid("sub_2_3",sub_2_2,insert_2_3, rot1 , G4ThreeVector(-1.*cm,0*cm,1.5*cm));

    G4Tubs* insert_2_4 = new G4Tubs("insert_2_4",0, (10/2)*mm , (2.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_2_4 = new G4SubtractionSolid("sub_2_4",sub_2_3,insert_2_4, rot1 , G4ThreeVector(-1.*cm,0*cm,4.5*cm));

    G4Tubs* insert_2_5 = new G4Tubs("insert_2_5",0, (15/2)*mm , (2.0/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_2_5 = new G4SubtractionSolid("sub_2_5",sub_2_4,insert_2_5, rot1 , G4ThreeVector(-1.*cm,0*cm,7.5*cm));

    G4Tubs* insert_3_0 = new G4Tubs("insert_3_0",0, (0.5/2)*mm , (3.25/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_3_0 = new G4SubtractionSolid("sub_3_0",sub_2_5,insert_3_0, rot1 , G4ThreeVector(-1.*cm,3*cm,-7.5*cm));

    G4Tubs* insert_3_1 = new G4Tubs("insert_3_1",0, (2/2)*mm , (3.25/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_3_1 = new G4SubtractionSolid("sub_3_1",sub_3_0,insert_3_1, rot1 , G4ThreeVector(-1.*cm,3*cm,-4.5*cm));

    G4Tubs* insert_3_2 = new G4Tubs("insert_3_2",0, (4/2)*mm , (3.25/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_3_2 = new G4SubtractionSolid("sub_3_2",sub_3_1,insert_3_2, rot1 , G4ThreeVector(-1.*cm,3*cm,-1.5*cm));

    G4Tubs* insert_3_3 = new G4Tubs("insert_3_3",0, (7/2)*mm , (3.25/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_3_3 = new G4SubtractionSolid("sub_3_3",sub_3_2,insert_3_3, rot1 , G4ThreeVector(-1.*cm,3*cm,1.5*cm));

    G4Tubs* insert_3_4 = new G4Tubs("insert_3_4",0, (10/2)*mm , (3.25/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_3_4 = new G4SubtractionSolid("sub_3_4",sub_3_3,insert_3_4, rot1 , G4ThreeVector(-1.*cm,3*cm,4.5*cm));

    G4Tubs* insert_4_0 = new G4Tubs("insert_4_0",0, (0.5/2)*mm , (4.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_4_0 = new G4SubtractionSolid("sub_4_0",sub_3_4,insert_4_0, rot1 , G4ThreeVector(-1.*cm,6*cm,-7.5*cm));

    G4Tubs* insert_4_1 = new G4Tubs("insert_4_1",0, (2/2)*mm , (4.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_4_1 = new G4SubtractionSolid("sub_4_1",sub_4_0,insert_4_1, rot1 , G4ThreeVector(-1.*cm,6*cm,-4.5*cm));

    G4Tubs* insert_4_2 = new G4Tubs("insert_4_2",0, (4/2)*mm , (4.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_4_2 = new G4SubtractionSolid("sub_4_2",sub_4_1,insert_4_2, rot1 , G4ThreeVector(-1.*cm,6*cm,-1.5*cm));

    G4Tubs* insert_4_3 = new G4Tubs("insert_4_3",0, (7/2)*mm , (4.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_4_3 = new G4SubtractionSolid("sub_4_3",sub_4_2,insert_4_3, rot1 , G4ThreeVector(-1.*cm,6*cm,1.5*cm));

    G4Tubs* insert_4_4 = new G4Tubs("insert_4_4",0, (10/2)*mm , (4.5/2)*mm , 0. , twopi);
    G4SubtractionSolid* sub_4_4 = new G4SubtractionSolid("sub_4_4",sub_4_3,insert_4_4, rot1 , G4ThreeVector(-1.*cm,6*cm,4.5*cm));

    G4LogicalVolume* LasVegasLog = new G4LogicalVolume(sub_4_4,bone ,"LasVegasLog");
    LasVegasLog->SetVisAttributes(linepair_att);
    new G4PVPlacement(0,G4ThreeVector(),"LasVegasPhys",LasVegasLog,box_phys,false,0);


  }

  //----------------------------------------------------------------------------------------------------------------
  // Slanted edge phantom
  //----------------------------------------------------------------------------------------------------------------

  else if(thePhantom =="SlantedEdge"){
    G4VisAttributes* linepair_att  = new G4VisAttributes(G4Colour(0,0,1));
    linepair_att->SetVisibility(true);
    linepair_att->SetForceSolid(true);
    G4Material *bone = theMaterial->ConstructMaterial("ToothEnamel_LL",2.89);
    
    G4Box* SlantedBox = new G4Box("SlantedBox",5*cm,5*cm,5*cm);
    G4LogicalVolume* SlantedLog = new G4LogicalVolume(SlantedBox,bone ,"SlantedLog");
    SlantedLog->SetVisAttributes(linepair_att);
    
    G4RotationMatrix* rotSlanted = new G4RotationMatrix();
    rotSlanted->rotateX(2.5*pi/180.);
    
    new G4PVPlacement(rotSlanted,G4ThreeVector(),"SlantedPhys",SlantedLog,box_phys,false,0);
}

  //----------------------------------------------------------------------------------------------------------------
  // Catphan phantom
  //----------------------------------------------------------------------------------------------------------------

  else if(thePhantom =="Catphan"){

    //Specific list of material for the catphan phantom
    G4double z, a, density;
    G4String name, symbol;
    G4int ncomponents, natoms;

    G4Element* elH  = new G4Element(name="Hydrogen"  ,symbol="H" , z= 1., a=1.01*g/mole);
    G4Element* elC  = new G4Element(name="Carbon"    ,symbol="C" , z= 6., a=12.01*g/mole);
    G4Element* elO  = new G4Element(name="Oxygen"    ,symbol="O" , z= 8., a=16.00*g/mole);
    G4Element* elF  = new G4Element(name="Fluorine"  ,symbol="F" , z= 9., a=19.00*g/mole);
    G4Element* elAl = new G4Element(name="Aluminium" ,symbol="Al", z= 13.,a=26.98*g/mole);
    G4Element* elW  = new G4Element(name="Tungsten"  ,symbol="W",  z= 74.,a=183.84*g/mole);


    G4Material *Teflon = new G4Material("Teflon",density=2.16*g/cm3,ncomponents=2); //C2F4
    Teflon->AddElement(elC,natoms=2);
    Teflon->AddElement(elF,natoms=4);

    G4Material *PMP    = new G4Material("PMP",density=0.83*g/cm3,ncomponents=2); //H14C7
    PMP->AddElement(elH,natoms=14);
    PMP->AddElement(elC,natoms=7);

    G4Material *LDPE    = new G4Material("LDPE",density=0.92*g/cm3,ncomponents=2); //H4C2
    LDPE->AddElement(elH,natoms=4);
    LDPE->AddElement(elC,natoms=2);

    G4Material *Polystyrene    = new G4Material("Polystyrene",density=1.05*g/cm3,ncomponents=2); //H8C8
    Polystyrene->AddElement(elH,natoms=8);
    Polystyrene->AddElement(elC,natoms=8);

    G4Material *Delrin    = new G4Material("Delrin",density=1.41*g/cm3,ncomponents=3); //H2C1O1
    Delrin->AddElement(elH,natoms=2);
    Delrin->AddElement(elC,natoms=1);
    Delrin->AddElement(elO,natoms=1);

    G4Material *Acrylic    = new G4Material("Acrylic",density=1.18*g/cm3,ncomponents=3); //H8C5O2 
    Acrylic->AddElement(elH,natoms=8);
    Acrylic->AddElement(elC,natoms=5);
    Acrylic->AddElement(elO,natoms=2);

    G4Material* Aluminium = new G4Material("Aluminium",density=2.70*g/cm3,ncomponents=1);
    Aluminium->AddElement(elAl,natoms=1);

    G4Material* TungstenCarbide = new G4Material("TungstenCarbide", density=15.63*g/cm3,ncomponents=2);
    TungstenCarbide->AddElement(elC,natoms=1);
    TungstenCarbide->AddElement(elW,natoms=1);

    G4VisAttributes* att3  = new G4VisAttributes(G4Colour(0,1,1));
    att3->SetVisibility(true);

    G4Tubs* Casing= new G4Tubs("Casing",0,200*mm/2,65*mm/2,0,2*pi);
    G4LogicalVolume* Casing_Log = new G4LogicalVolume(Casing,water,"Casing_Log");
    Casing_Log->SetVisAttributes(att3);

    
    G4RotationMatrix* rotExt = new G4RotationMatrix();
    rotExt->rotateX(pi/2.);
    rotExt->rotateZ(theAngle*pi/180.);
    G4VPhysicalVolume* CasingPhys = new G4PVPlacement(rotExt,G4ThreeVector(),"CasingWorld",Casing_Log,box_phys,false,0);


    // CTP404 Slice Geometry and sensitometry module 
    G4VisAttributes* att  = new G4VisAttributes(G4Colour(0,0,1));
    att->SetVisibility(true);

    G4VisAttributes* sph_att  = new G4VisAttributes(G4Colour(0,1,0));
    sph_att->SetVisibility(true);
    sph_att->SetForceSolid(true);



    G4Tubs* CTP404 = new G4Tubs("CTP404",0,150*mm/2,25/2*mm,0,2*pi);
    G4LogicalVolume* CTP404Log = new G4LogicalVolume(CTP404,water ,"CTP404Log"); // Water

    G4VPhysicalVolume* CTP404World = new G4PVPlacement(0,G4ThreeVector(0,0,20*mm),"CTP404World",CTP404Log,CasingPhys,false,0);

    // 4 Air and Teflons rods
    G4Tubs* small_rod  = new G4Tubs("small_rod", 0, 3*mm/2, 25/2*mm, 0,2*pi);
    G4LogicalVolume* small_rod_1 = new G4LogicalVolume(small_rod,air ,"small_rod_1"); // Air
    G4LogicalVolume* small_rod_2 = new G4LogicalVolume(small_rod,air ,"small_rod_2"); // Air
    G4LogicalVolume* small_rod_3 = new G4LogicalVolume(small_rod,air ,"small_rod_3"); // Air
    G4LogicalVolume* small_rod_4 = new G4LogicalVolume(small_rod,Teflon ,"small_rod_4"); // Teflon

    new G4PVPlacement(0,G4ThreeVector(-2.5*cm,-2.5*cm,0),"small_rod_1World",small_rod_1,CTP404World,false,0);
    new G4PVPlacement(0,G4ThreeVector(-2.5*cm, 2.5*cm,0),"small_rod_2World",small_rod_2,CTP404World,false,0);
    new G4PVPlacement(0,G4ThreeVector( 2.5*cm,-2.5*cm,0),"small_rod_3World",small_rod_3,CTP404World,false,0);
    new G4PVPlacement(0,G4ThreeVector( 2.5*cm, 2.5*cm,0),"small_rod_4World",small_rod_4,CTP404World,false,0);


    /*
    // 5 Acrylic Sphere
    G4Sphere *acrilSphere1 = new G4Sphere ("acrilSphere1", 0, 10/2*mm, 0,2*pi,0,2*pi);
    G4Sphere *acrilSphere2 = new G4Sphere ("acrilSphere2", 0, 8/2*mm, 0,2*pi,0,2*pi);
    G4Sphere *acrilSphere3 = new G4Sphere ("acrilSphere3", 0, 6/2*mm, 0,2*pi,0,2*pi);
    G4Sphere *acrilSphere4 = new G4Sphere ("acrilSphere4", 0, 4/2*mm, 0,2*pi,0,2*pi);
    G4Sphere *acrilSphere5 = new G4Sphere ("acrilSphere5", 0, 2/2*mm, 0,2*pi,0,2*pi);
    
    G4LogicalVolume *acrilLog1 = new G4LogicalVolume(acrilSphere1,Acrylic,"acrilLog1"); //Acrylic
    G4LogicalVolume *acrilLog2 = new G4LogicalVolume(acrilSphere2,Acrylic,"acrilLog2"); //Acrylic   
    G4LogicalVolume *acrilLog3 = new G4LogicalVolume(acrilSphere3,Acrylic,"acrilLog3"); //Acrylic   
    G4LogicalVolume *acrilLog4 = new G4LogicalVolume(acrilSphere4,Acrylic,"acrilLog4"); //Acrylic   
    G4LogicalVolume *acrilLog5 = new G4LogicalVolume(acrilSphere5,Acrylic,"acrilLog5"); //Acrylic   

    acrilLog1->SetVisAttributes(sph_att);
    acrilLog2->SetVisAttributes(sph_att);
    acrilLog3->SetVisAttributes(sph_att);
    acrilLog4->SetVisAttributes(sph_att);
    acrilLog5->SetVisAttributes(sph_att);

    G4ThreeVector placement(0,15*mm,0);
    placement.rotateZ(2*pi/5);
    new G4PVPlacement(0,placement,"acril_1_World",acrilLog1,CTP404World,false,0);
    placement.rotateZ(2*pi/5);
    new G4PVPlacement(0,placement,"acril_2_World",acrilLog2,CTP404World,false,0);
    placement.rotateZ(2*pi/5);
    new G4PVPlacement(0,placement,"acril_3_World",acrilLog3,CTP404World,false,0);
    placement.rotateZ(2*pi/5);
    new G4PVPlacement(0,placement,"acril_4_World",acrilLog4,CTP404World,false,0);
    placement.rotateZ(2*pi/5);
    new G4PVPlacement(0,placement,"acril_5_World",acrilLog5,CTP404World,false,0);
   
    */

    // 8 Sensitometry 12 mm Large Rod
    G4Tubs* large_rod  = new G4Tubs("large_rod", 0, 12*mm/2, 25/2*mm, 0,2*pi);
    G4LogicalVolume* large_rod_1 = new G4LogicalVolume(large_rod,Acrylic ,"large_rod_1");  //Acrylic
    G4LogicalVolume* large_rod_2 = new G4LogicalVolume(large_rod,Polystyrene ,"large_rod_2");  //Polystyrene
    G4LogicalVolume* large_rod_3 = new G4LogicalVolume(large_rod,LDPE ,"large_rod_3");  //LDPE 
    G4LogicalVolume* large_rod_4 = new G4LogicalVolume(large_rod,Delrin ,"large_rod_4"); //Delrin
    G4LogicalVolume* large_rod_5 = new G4LogicalVolume(large_rod,Teflon ,"large_rod_5"); //Teflon
    G4LogicalVolume* large_rod_6 = new G4LogicalVolume(large_rod,air ,"large_rod_6"); //Air 
    G4LogicalVolume* large_rod_7 = new G4LogicalVolume(large_rod,PMP ,"large_rod_7"); //PMP
    G4LogicalVolume* large_rod_8 = new G4LogicalVolume(large_rod,water ,"large_rod_8"); //Water


    G4ThreeVector placement2(0,6*cm,0);
    new G4PVPlacement(0,placement2,"large_rod_1World",large_rod_1,CTP404World,false,0);
    placement2.rotateZ(pi/6);
    new G4PVPlacement(0,placement2,"large_rod_4World",large_rod_2,CTP404World,false,0);
    placement2.rotateZ(2*pi/6);
    new G4PVPlacement(0,placement2,"large_rod_4World",large_rod_3,CTP404World,false,0);
    placement2.rotateZ(2*pi/6);
    new G4PVPlacement(0,placement2,"large_rod_4World",large_rod_4,CTP404World,false,0);
    placement2.rotateZ(pi/6);
    new G4PVPlacement(0,placement2,"large_rod_4World",large_rod_5,CTP404World,false,0);
    placement2.rotateZ(pi/6);
    new G4PVPlacement(0,placement2,"large_rod_4World",large_rod_6,CTP404World,false,0);
    placement2.rotateZ(2*pi/6);
    new G4PVPlacement(0,placement2,"large_rod_4World",large_rod_7,CTP404World,false,0);
    placement2.rotateZ(2*pi/6);
    new G4PVPlacement(0,placement2,"large_rod_4World",large_rod_8,CTP404World,false,0);


    G4Tubs* wire_ramp  = new G4Tubs("wire_ramp", 0, 1*mm, 6.4/2*cm, 0,2*pi); //6.4 come from the 23 degree wire ramp and the thickness Z of the catphan of 2.5mm (Hyp = 2.5/sin(23))
    G4LogicalVolume* wire_log = new G4LogicalVolume(wire_ramp,Aluminium ,"wire_log"); // Aluminum

    G4RotationMatrix* rotWire = new G4RotationMatrix();
    rotWire->rotateY((90-23)*pi/180.);
    new G4PVPlacement(rotWire,G4ThreeVector(0,4*cm,0),"wire_1_World",wire_log,CTP404World,false,0);
    new G4PVPlacement(rotWire,G4ThreeVector(0,-4*cm,0),"wire_1_World",wire_log,CTP404World,false,0);

    G4RotationMatrix* rotWire2 = new G4RotationMatrix();
    rotWire2->rotateX((90-23)*pi/180.);
    new G4PVPlacement(rotWire2,G4ThreeVector(4*cm,0,0),"wire_1_World",wire_log,CTP404World,false,0);
    new G4PVPlacement(rotWire2,G4ThreeVector(-4*cm,0,0),"wire_1_World",wire_log,CTP404World,false,0);


    //CTP528 High Resolution Module


    G4VisAttributes* att2  = new G4VisAttributes(G4Colour(1,0,1));
    att2->SetVisibility(true);

    G4Tubs* CTP528 = new G4Tubs("CTP528",0,150*mm/2,40/2*mm,0,2*pi);
    G4LogicalVolume* CTP528Log = new G4LogicalVolume(CTP528,water ,"CTP528Log"); 
    CTP528Log->SetVisAttributes(att2);    
    G4VPhysicalVolume* CTP528World = new G4PVPlacement(0,G4ThreeVector(0,0,-12.5*mm),"CTP528World",CTP528Log,CasingPhys,false,0);
    
    G4Sphere *bead = new G4Sphere ("acrilSphere1", 0, 0.28/2*mm, 0,2*pi,0,2*pi);
    G4LogicalVolume* beadLog = new G4LogicalVolume(bead,TungstenCarbide,"bead_log");  // Tungsten Carbide
    beadLog->SetVisAttributes(sph_att);
    new G4PVPlacement(0,G4ThreeVector(0,20*mm,0),"bead_1_world",beadLog,CTP528World,false,0);
    new G4PVPlacement(0,G4ThreeVector(0,-20*mm,0),"bead_1_world",beadLog,CTP528World,false,0);

    for(int i =1;i<21;i++){

      double GapSize = 0.5/float(i);
      G4ThreeVector placement4(0,45*mm,0);
      int nbLP = 5;
      G4RotationMatrix* rotLP = new G4RotationMatrix();

     


      if(i==1){
	nbLP =2;
	placement4.rotateZ(15*float(i)*pi/180);
	rotLP->rotateZ( (180- 15*float(i))*pi/180);}

      else if(i==2){
	nbLP =3;
	placement4.rotateZ(21*float(i)*pi/180);
	rotLP->rotateZ( (180- 21*float(i))*pi/180);}
      else if(i<6){
	nbLP = 4;
	placement4.rotateZ(16*float(i+1)*pi/180);
	rotLP->rotateZ( (180- 16*float(i+1))*pi/180);}

      else{	
	placement4.rotateZ(16*float(i+1)*pi/180);	
	rotLP->rotateZ( (180- 16*float(i+1))*pi/180);}
      
      G4Box* cont_box = new G4Box("cont_box", (nbLP-0.5)*GapSize*cm,2*mm,2*mm);
      G4LogicalVolume* cont_log = new G4LogicalVolume(cont_box,water,"cont_log"); //Water
      G4VPhysicalVolume* cont_world = new G4PVPlacement(rotLP,placement4,"cont_World",cont_log,CTP528World,false,0); // Container for the LP
      for(int j=0;j<nbLP;j++){

	G4Box* LinePair = new G4Box("LinePair",GapSize/2*cm,2*mm,2*mm);
	G4LogicalVolume* LinePair_Log = new G4LogicalVolume(LinePair,Aluminium,"LinePair_Log"); // Aluminium
	LinePair_Log->SetVisAttributes(sph_att);
	new G4PVPlacement(0,G4ThreeVector(-(nbLP-0.5)*GapSize*cm + GapSize/2*cm +  2*j*GapSize*cm,0,0),"LinePair_world",LinePair_Log,cont_world,false,0);
	}
    }
  

}
  return physWorld;
}


