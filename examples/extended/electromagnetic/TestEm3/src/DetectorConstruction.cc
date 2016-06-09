//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: DetectorConstruction.cc,v 1.7 2004/01/21 17:29:27 maire Exp $
// GEANT4 tag $Name: geant4-06-00-patch-01 $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4UnitsTable.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
:defaultMaterial(0),solidWorld(0),logicWorld(0),physiWorld(0),
 solidCalor(0),logicCalor(0),physiCalor(0),
 solidLayer(0),logicLayer(0),physiLayer(0),
 magField(0)
{
  for (G4int i=0; i<MaxAbsor; i++)
     {
      AbsorMaterial[i]=0; AbsorThickness[i]=0.;
      solidAbsor[i]=0;logicAbsor[i]=0;physiAbsor[i]=0;
     }
  //    
  // default parameter values of the calorimeter
  NbOfAbsor = 2;
  AbsorThickness[0] = 2.3*mm;
  AbsorThickness[1] = 5.7*mm;
  NbOfLayers        = 50;
  CalorSizeYZ       = 40.*cm;
  ComputeCalorParameters();
  
  // materials
  DefineMaterials();
  SetWorldMaterial("Galactic");
  SetAbsorMaterial(0,"Lead");
  SetAbsorMaterial(1,"liquidArgon");  
  
  // create UserLimits
  userLimits = new G4UserLimits();

  // create commands for interactive definition of the calorimeter
  detectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete userLimits;
  delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  return ConstructCalorimeter();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
 //This function illustrates the possible ways to define materials

  //
  // define Elements
  //
  G4double z,a;
  
  G4Element* H  = new G4Element("Hydrogen",  "H" , z= 1.,  a= 1.008*g/mole);
  G4Element* C  = new G4Element("Carbon",    "C" , z= 6.,  a= 12.01*g/mole);
  G4Element* N  = new G4Element("Nitrogen",  "N" , z= 7.,  a= 14.01*g/mole);
  G4Element* O  = new G4Element("Oxygen",    "O" , z= 8.,  a= 16.00*g/mole);
  G4Element* Si = new G4Element("Silicon",   "Si", z= 14., a= 28.09*g/mole);
  G4Element* Ge = new G4Element("Germanium", "Ge", z= 32., a= 72.59*g/mole);
  G4Element* I  = new G4Element("Iodine",    "I" , z= 53., a= 126.90*g/mole);
  G4Element* Cs = new G4Element("Cesium",    "Cs", z= 55., a= 132.90*g/mole);
  G4Element* Bi = new G4Element("Bismuth",   "Bi", z= 83., a= 208.98*g/mole);

  //
  // define an Element from isotopes, by relative abundance
  //
  G4int iz, n;                       //iz=number of protons  in an isotope;
                                     // n=number of nucleons in an isotope;
  G4int   ncomponents;				     
  G4double abundance;				     

  G4Isotope* U5 = new G4Isotope("U235", iz=92, n=235, a=235.01*g/mole);
  G4Isotope* U8 = new G4Isotope("U238", iz=92, n=238, a=238.03*g/mole);

  G4Element* U  = new G4Element("enriched Uranium", "U", ncomponents=2);
  U->AddIsotope(U5, abundance= 90.*perCent);
  U->AddIsotope(U8, abundance= 10.*perCent);

  //
  // define simple materials
  //
  G4double density;
  
  new G4Material("liquidH2",    z=1.,  a= 1.008*g/mole,  density= 70.8*mg/cm3);
  new G4Material("Aluminium",   z=13., a= 26.98*g/mole,  density= 2.700*g/cm3);
  new G4Material("liquidArgon", z=18., a= 39.95*g/mole,  density= 1.390*g/cm3);
  new G4Material("Titanium",    z=22., a= 47.867*g/mole, density= 4.54*g/cm3);
  new G4Material("Iron",        z=26., a= 55.85*g/mole,  density= 7.870*g/cm3);
  new G4Material("Copper",      z=29., a= 63.55*g/mole,  density= 8.960*g/cm3);
  new G4Material("Tungsten",    z=74., a= 183.85*g/mole, density= 19.30*g/cm3);
  new G4Material("Gold",        z=79., a= 196.97*g/mole, density= 19.32*g/cm3);
  new G4Material("Lead",        z=82., a= 207.19*g/mole, density= 11.35*g/cm3);
  new G4Material("Uranium",     z=92., a= 238.03*g/mole, density= 18.95*g/cm3);

  //
  // define a material from elements.   case 1: chemical molecule
  //
  G4int natoms;
  
  G4Material* H2O = 
  new G4Material("Water", density= 1.000*g/cm3, ncomponents=2);
  H2O->AddElement(H, natoms=2);
  H2O->AddElement(O, natoms=1);
  H2O->GetIonisation()->SetMeanExcitationEnergy(75.0*eV);
  H2O->SetChemicalFormula("H_2O");

  G4Material* Sci = 
  new G4Material("Scintillator", density= 1.032*g/cm3, ncomponents=2);
  Sci->AddElement(C, natoms=9);
  Sci->AddElement(H, natoms=10);

  G4Material* Sili = 
  new G4Material("Silicon", density= 2.330*g/cm3, ncomponents=1);
  Sili->AddElement(Si, natoms=1);

  G4Material* SiO2 = 
  new G4Material("quartz", density= 2.200*g/cm3, ncomponents=2);
  SiO2->AddElement(Si, natoms=1);
  SiO2->AddElement(O , natoms=2);

  G4Material* G10 = 
  new G4Material("NemaG10", density= 1.700*g/cm3, ncomponents=4);
  G10->AddElement(Si, natoms=1);
  G10->AddElement(O , natoms=2);
  G10->AddElement(C , natoms=3);
  G10->AddElement(H , natoms=3);

  G4Material* CsI = 
  new G4Material("CsI", density= 4.534*g/cm3, ncomponents=2);
  CsI->AddElement(Cs, natoms=1);
  CsI->AddElement(I , natoms=1);
  CsI->GetIonisation()->SetMeanExcitationEnergy(553.1*eV);

  G4Material* BGO = 
  new G4Material("BGO", density= 7.10*g/cm3, ncomponents=3);
  BGO->AddElement(O , natoms=12);
  BGO->AddElement(Ge, natoms= 3);
  BGO->AddElement(Bi, natoms= 4);

  //
  // define a material from elements.   case 2: mixture by fractional mass
  //
  G4double fractionmass;
  
  G4Material* Air = 
  new G4Material("Air", density= 1.290*mg/cm3, ncomponents=2);
  Air->AddElement(N, fractionmass=0.7);
  Air->AddElement(O, fractionmass=0.3);

  G4Material* Air20 = 
  new G4Material("Air20", density= 1.205*mg/cm3, ncomponents=2,
                          kStateGas, 293.*kelvin, 1.*atmosphere);
  Air20->AddElement(N, fractionmass=0.7);
  Air20->AddElement(O, fractionmass=0.3);

  //
  // define a material from elements and others materials (mixture of mixtures)
  //

  G4Material* Aerog = 
  new G4Material("Aerogel", density= 0.200*g/cm3, ncomponents=3);
  Aerog->AddMaterial(SiO2, fractionmass=62.5*perCent);
  Aerog->AddMaterial(H2O , fractionmass=37.4*perCent);
  Aerog->AddElement (C   , fractionmass= 0.1*perCent);

  //
  // examples of gas in non STP conditions
  //
  G4double temperature, pressure;
  
  G4Material* CO2 = 
  new G4Material("CarbonicGas", density= 27.*mg/cm3, ncomponents=2,
                 kStateGas, temperature= 325.*kelvin, pressure= 50.*atmosphere);
  CO2->AddElement(C, natoms=1);
  CO2->AddElement(O, natoms=2);

  G4Material* steam = 
  new G4Material("WaterSteam", density= 0.3*mg/cm3, ncomponents=1,
                  kStateGas, temperature= 500.*kelvin, pressure= 2.*atmosphere);
  steam->AddMaterial(H2O, fractionmass=1.);

  //
  // examples of vacuum
  //

  density     = universe_mean_density;    //from PhysicalConstants.h
  pressure    = 3.e-18*pascal;
  temperature = 2.73*kelvin;
  new G4Material("Galactic", z=1., a=1.008*g/mole, density,
                             kStateGas,temperature,pressure);

  density     = 1.e-5*g/cm3;
  pressure    = 2.e-2*bar;
  temperature = STP_Temperature;         //from PhysicalConstants.h
  G4Material* beam = 
  new G4Material("Beam", density, ncomponents=1,
                         kStateGas,temperature,pressure);
  beam->AddMaterial(Air, fractionmass=1.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ComputeCalorParameters()
{
  // Compute derived parameters of the calorimeter
     LayerThickness = 0.;
     for (G4int iAbs=0; iAbs<NbOfAbsor; iAbs++)
     LayerThickness += AbsorThickness[iAbs];
     CalorThickness = NbOfLayers*LayerThickness;
     
     WorldSizeX = 1.2*CalorThickness; WorldSizeYZ = 1.2*CalorSizeYZ;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructCalorimeter()
{
  // complete the Calor parameters definition
  ComputeCalorParameters();

  // Cleanup old geometry
  G4GeometryManager::GetInstance()->OpenGeometry();
  G4PhysicalVolumeStore::GetInstance()->Clean();
  G4LogicalVolumeStore::GetInstance()->Clean();
  G4SolidStore::GetInstance()->Clean();

  //
  // World
  //

  solidWorld = new G4Box("World",				//its name
                   WorldSizeX/2,WorldSizeYZ/2,WorldSizeYZ/2);	//its size

  logicWorld = new G4LogicalVolume(solidWorld,		//its solid
                                   defaultMaterial,	//its material
                                   "World");		//its name

  physiWorld = new G4PVPlacement(0,			//no rotation
  				 G4ThreeVector(),	//at (0,0,0)
                                 logicWorld,		//its logical volume
                                 "World",		//its name
                                 0,			//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number
  //
  // Calorimeter
  //

  solidCalor = new G4Box("Calorimeter",				     //its name
    		       CalorThickness/2,CalorSizeYZ/2,CalorSizeYZ/2);//size

  logicCalor = new G4LogicalVolume(solidCalor,		//its solid
      				   defaultMaterial,	//its material
      				   "Calorimeter");	//its name

  physiCalor = new G4PVPlacement(0,			//no rotation
                                 G4ThreeVector(),	//at (0,0,0)
                                 logicCalor,		//its logical volume
                                 "Calorimeter",		//its name
                                 logicWorld,		//its mother  volume
                                 false,			//no boolean operation
                                 0);			//copy number

  //
  // Layers
  //

  solidLayer = new G4Box("Layer",		                      //its name
                       LayerThickness/2,CalorSizeYZ/2,CalorSizeYZ/2); //size

  logicLayer = new G4LogicalVolume(solidLayer,		//its solid
                                   defaultMaterial,	//its material
                                   "Layer");		//its name
  if (NbOfLayers > 1)
    physiLayer = new G4PVReplica("Layer",		//its name
      				 logicLayer,		//its logical volume
      				 logicCalor,		//its mother
                                 kXAxis,		//axis of replication
                                 NbOfLayers,		//number of replica
                                 LayerThickness);	//witdth of replica
  else
    physiLayer = new G4PVPlacement(0,			//no rotation
                                   G4ThreeVector(),	//at (0,0,0)
                                   logicLayer,		//its logical volume
                                   "Layer",		//its name
                                   logicCalor,		//its mother  volume
                                   false,		//no boolean operation
                                   0);			//copy number

  //
  // Absorbers
  //

  G4double xfront = -0.5*LayerThickness;
  for (G4int k=0; k<NbOfAbsor; k++)
     { solidAbsor[k] = new G4Box("Absorber",		//its name
                          AbsorThickness[k]/2,CalorSizeYZ/2,CalorSizeYZ/2);

      logicAbsor[k] = new G4LogicalVolume(solidAbsor[k],    //its solid
      			                  AbsorMaterial[k], //its material
      			                  AbsorMaterial[k]->GetName());

      logicAbsor[k]->SetUserLimits(userLimits);

      G4double xcenter = xfront+0.5*AbsorThickness[k];
      xfront += AbsorThickness[k];
      physiAbsor[k] = new G4PVPlacement(0,		   //no rotation
      		    	G4ThreeVector(xcenter,0.,0.),      //its position
                        logicAbsor[k],     		   //its logical volume	
                    	AbsorMaterial[k]->GetName(),	   //its name
                        logicLayer,        		   //its mother
                        false,             		   //no boulean operat
                        k);               		   //copy number

     }
     
  PrintCalorParameters();

  //always return the physical World
  //
  return physiWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintCalorParameters()
{
  G4cout << "\n-------------------------------------------------------------"
         << "\n ---> The calorimeter is " << NbOfLayers << " layers of:";
  for (G4int i=0; i<NbOfAbsor; i++)
     {
      G4cout << "\n \t" << std::setw(12) << AbsorMaterial[i]->GetName() <<": "
              << std::setw(6) << G4BestUnit(AbsorThickness[i],"Length");
     }
  G4cout << "\n-------------------------------------------------------------\n";
  
  G4cout << "\n" << defaultMaterial << G4endl;    
  for (G4int j=0; j<NbOfAbsor; j++)
     G4cout << "\n" << AbsorMaterial[j] << G4endl;

  G4cout << "\n-------------------------------------------------------------\n";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetWorldMaterial(G4String material)
{
  // search the material by its name
  G4Material* pttoMaterial = G4Material::GetMaterial(material);
  if (pttoMaterial) defaultMaterial = pttoMaterial;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNbOfLayers(G4int ival)
{
  // set the number of Layers
  //
  if (ival < 1)
    { G4cout << "\n --->warning from SetNbOfLayers: "
             << ival << " must be at least 1. Command refused" << G4endl;
      return;
    }
  NbOfLayers = ival;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetNbOfAbsor(G4int ival)
{
  // set the number of Absorbers
  //
  if (ival < 1 || ival > MaxAbsor)
    { G4cout << "\n ---> warning from SetNbOfAbsor: "
             << ival << " must be at least 1 and and most " << MaxAbsor
	     << ". Command refused" << G4endl;
      return;
    }
  NbOfAbsor = ival;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorMaterial(G4int ival,G4String material)
{
  // search the material by its name
  //
  if (ival >= NbOfAbsor)
    { G4cout << "\n --->warning from SetAbsorMaterial: absor number "
             << ival << " out of range. Command refused" << G4endl;
      return;
    }

  G4Material* pttoMaterial = G4Material::GetMaterial(material);
  if (pttoMaterial) AbsorMaterial[ival] = pttoMaterial;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetAbsorThickness(G4int ival,G4double val)
{
  // change Absorber thickness
  //
  if (ival >= NbOfAbsor)
    { G4cout << "\n --->warning from SetAbsorThickness: absor number "
             << ival << " out of range. Command refused" << G4endl;
      return;
    }
  if (val <= DBL_MIN)
    { G4cout << "\n --->warning from SetAbsorThickness: thickness "
             << val  << " out of range. Command refused" << G4endl;
      return;
    }
  AbsorThickness[ival] = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetCalorSizeYZ(G4double val)
{
  // change the transverse size
  //
  if (val <= DBL_MIN)
    { G4cout << "\n --->warning from SetCalorSizeYZ: thickness "
             << val  << " out of range. Command refused" << G4endl;
      return;
    }
  CalorSizeYZ = val;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"

void DetectorConstruction::SetMagField(G4double fieldValue)
{
  //apply a global uniform magnetic field along Z axis
  //
  G4FieldManager* fieldMgr
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();

  if(magField) delete magField;		//delete the existing magn field

  if(fieldValue!=0.)			// create a new one if non nul
  { magField = new G4UniformMagField(G4ThreeVector(0.,0.,fieldValue));
    fieldMgr->SetDetectorField(magField);
    fieldMgr->CreateChordFinder(magField);
  } else {
    magField = 0;
    fieldMgr->SetDetectorField(magField);
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMaxStepSize(G4double val)
{
  // set the maximum allowed step size
  //
  if (val <= DBL_MIN)
    { G4cout << "\n --->warning from SetMaxStepSize: maxStep "
             << val  << " out of range. Command refused" << G4endl;
      return;
    }
  userLimits->SetMaxAllowedStep(val);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(ConstructCalorimeter());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
