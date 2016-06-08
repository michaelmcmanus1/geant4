// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4IonTable.cc,v 1.9 1999/05/06 16:37:40 kurasige Exp $
// GEANT4 tag $Name: geant4-00-01 $
//
// 
// --------------------------------------------------------------
//	GEANT 4 class implementation file 
//
//	For information related to this code contact:
//	CERN, IT Division, ASD Group
//	History: first implementation, based on object model of
//	27 June 1998  H.Kurashige
// ---------------------------------------------------------------
//      modified GetIon                 02 Aug., 98 H.Kurashige
//      added Remove()                  06 Nov.,98 H.Kurashige
//      use G4NucleiPropoerties to get nuceli Mass 17  Nov.,98 H.Kurashige
//      use G4GenericIon for process List
//      modify fomula of Ion mass       09 Dec., 98 H.Kurashige 
#include "G4IonTable.hh"
#include "G4ParticleTable.hh"
#include "G4Ions.hh"
#include "G4UImanager.hh"
#include "G4NucleiProperties.hh"

#include "G4ios.hh"

#ifdef WIN32
#  include <Strstrea.h>
#else
#  include <strstream.h>
#endif


G4IonTable::G4IonTable()
{
  fIonList = new G4IonList();
}

G4IonTable::~G4IonTable()
{
  G4int idx;
  G4String name;
 
  for (idx=(fIonList->entries()-1); idx >=0 ; idx--) {
    G4ParticleDefinition* particle = (*fIonList)(idx);
    name = particle->GetParticleName();

    // keep the particle object if static
    if        (name == "alpha") {
 
    } else if (name == "deuteron") {
 
    } else if (name ==  "triton") {
 
    } else if (name ==   "He3")  {

    } else if (name ==   "GenericIon")  {
 
    } else {
      // delete if not static objects
#ifdef G4VERBOSE
      if (GetVerboseLevel()>1) {
	G4cout << "G4IonTable:~IonTable() : delete ion of  " << name << endl;
      }
#endif
      delete particle;
    }
  }
  // remove all contents in the Ion List 
  fIonList->clear();

  delete fIonList;
}

G4int G4IonTable::GetVerboseLevel() const
{
  return G4ParticleTable::GetParticleTable()->GetVerboseLevel();
}

G4ParticleDefinition* G4IonTable::GetIon(G4int Z, G4int A, G4int J, G4int Q)
{
  static G4double electronMass =  0.0;
 
  // Search ions with A, Z 
  G4ParticleDefinition* ion;
  G4bool isFound = false;

  // check if electron exits and get their masses
  if (electronMass<=0.0) {
    G4ParticleDefinition* electron = G4ParticleTable::GetParticleTable()->FindParticle("e-");
    if (electron == 0) {
      G4Exception("G4IonTable: G4Electron is not defined !!"); 
    }
    electronMass = electron->GetPDGMass();
  }

  // -- loop over all particles in Ion table
  for (G4int idx= 0; idx < fIonList->entries() ; idx++) {
    ion = (*fIonList)(idx);
    // charge = charge /eplus
    G4int aCharge = int(ion->GetPDGCharge()/eplus);
    // A = baryon number
    G4int anAtomicMass =  ion->GetBaryonNumber();
    // spin 
    G4int anAngularMomentum  =  ion->GetPDGiSpin();
    if ( (A == anAtomicMass) && (Q == aCharge) && ( J ==  anAngularMomentum )){
      isFound = true;
      break;
    }
  }
  
  if (!isFound) {
    // create ion
    G4String name = GetIonName(Z, A, J, Q);
    if ( name(0) == '?') {
#ifdef G4VERBOSE
      if (GetVerboseLevel()>0) {
	G4cout << "G4IonTable::GetIon() : can not create ions " << endl;
	G4cout << " Z =" << Z << "  A = " << A <<  endl;
      }
#endif
      return 0;
    } 
    G4double mass =  GetNucleusMass(Z, A) + electronMass*G4double(Z-Q);
    G4double charge =  G4double(Q)*eplus;
    // create an ion
    //   spin, parity, isospin values are fixed
    //
    ion = new G4Ions(    name,         mass,       0.0*MeV,     charge, 
			    J,              +1,             0,          
			    0,               0,             0,             
		    "nucleus",       0,             A,           0,
			 true,            -1.0,          0);

#ifdef G4VERBOSE
    if (GetVerboseLevel()>1) {
      G4cout << "G4IonTable::GetIon() : create ion of " << name << endl;
    } 
#endif
    // create command string for addProcManager
    char cmdAdd[60];
    ostrstream osAdd(cmdAdd,60);
    osAdd << "/run/particle/addProcManager "<< name << '\0';
    // set /control/verbose 0
    G4int tempVerboseLevel = G4UImanager::GetUIpointer()->GetVerboseLevel();
    G4UImanager::GetUIpointer()->SetVerboseLevel(0);
    // issue /run/particle/addProcManage
    G4UImanager::GetUIpointer()->ApplyCommand(cmdAdd);
    // retreive  /control/verbose 
    G4UImanager::GetUIpointer()->SetVerboseLevel(tempVerboseLevel);
 
    // Set cut value same as "GenericIon"
    G4ParticleDefinition* genericIon=G4ParticleTable::GetParticleTable()->FindParticle("GenericIon");

    if (genericIon->GetEnergyCuts() != 0) {
      ion->SetCuts( genericIon->GetLengthCuts());
#ifdef G4VERBOSE
      if (GetVerboseLevel()> 1) {
	G4cout << "G4IonTable::GetIon() : cut value =" << genericIon->GetLengthCuts()/mm << "[mm]" <<endl;
      } 
#endif      
      // Build Physics Tables for the ion
      // create command string for buildPhysicsTable
      char cmdBld[60];
      ostrstream osBld(cmdBld,60);
      osBld << "/run/particle/buildPhysicsTable "<< name << '\0';
      // set /control/verbose 0
      tempVerboseLevel = G4UImanager::GetUIpointer()->GetVerboseLevel();
      G4UImanager::GetUIpointer()->SetVerboseLevel(0);
      // issue /run/particle/buildPhysicsTable
      G4UImanager::GetUIpointer()->ApplyCommand(cmdBld);
      // retreive  /control/verbose 
      G4UImanager::GetUIpointer()->SetVerboseLevel(tempVerboseLevel);
    }
  }
  return ion;  
}

G4String G4IonTable::GetIonName(G4int Z, G4int A, G4int J, G4int Q) const 
{
  G4String name;
  if ( (0< Z) && (Z <=numberOfElements) ) {
    name = elementName[Z-1];
  } else {
    return "?";
  }
  char val[50];
  ostrstream os(val,50);
  os << A << '-' << J << "/2" << '[' << Q << ']' << '\0';
  name += val;
  return name;
}

G4double  G4IonTable::GetNucleusMass(G4int Z, G4int A) const
{
 static G4double protonMass = 0.0;
 static G4double neutronMass = 0.0;

 // check if proton/neutron/electron exits and get their masses
  if (protonMass<=0.0) {
    G4ParticleDefinition* proton = G4ParticleTable::GetParticleTable()->FindParticle("proton");
    G4ParticleDefinition* neutron = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
    if ((proton == 0)||(neutron == 0)) {
      G4Exception("G4IonTable: G4Proton or G4Neutron is not defined !!"); 
    }
    protonMass = proton->GetPDGMass();
    neutronMass = neutron->GetPDGMass();
  }

  // calculate nucleus mass
  G4ParticleDefinition* ion=0;
  G4double mass;
  if ( (Z<=2) ) {
    if ( (Z==1)&&(A==1) ) {
	  ion = G4ParticleTable::GetParticleTable()->FindParticle("proton"); // proton 
    } else if ( (Z==0)&&(A==1) ) {
	  ion = G4ParticleTable::GetParticleTable()->FindParticle("neutron"); // neutron 
    } else if ( (Z==1)&&(A==2) ) {
	  ion = G4ParticleTable::GetParticleTable()->FindParticle("deuteron"); // deuteron 
    } else if ( (Z==1)&&(A==3) ) {
	  ion = G4ParticleTable::GetParticleTable()->FindParticle("triton"); // tritoon 
    } else if ( (Z==2)&&(A==4) ) {
	  ion = G4ParticleTable::GetParticleTable()->FindParticle("alpha"); // alpha 
    } else if ( (Z==2)&&(A==3) ) {
	  ion = G4ParticleTable::GetParticleTable()->FindParticle("He3"); // He3 
	}
  }
  if (ion!=0) {
	mass = ion->GetPDGMass();
  }else {
    // This routine returns mass of nuclei (w/o including electron mass) 
    //   mass = Z*proton_mass + (A-Z)*neutron_mass - binding energy
     G4double bindingEnergy = G4NucleiPropertiesTable::GetBindingEnergy(Z, A);
     mass = G4double(Z)*protonMass + G4double(A-Z)*neutronMass - bindingEnergy;
  }
  return mass;
}

G4double  G4IonTable::GetIonMass(G4int Z, G4int A) const
{
   return GetNucleusMass(Z,A);
}


G4bool G4IonTable::IsIon(G4ParticleDefinition* particle) const
{
  return (particle->GetParticleType() == "nucleus");
}

void G4IonTable::Insert(G4ParticleDefinition* particle)
{
  if (IsIon(particle)) {
    fIonList->insert(particle);
  } else {
    //#ifdef G4VERBOSE
    //if (GetVerboseLevel()>0) {
    //  G4cout << "G4IonTable::Insert :" << particle->GetParticleName() ;
    //  G4cout << " is not ions" << endl; 
    //}
    //#endif
  }
}

void G4IonTable::Remove(G4ParticleDefinition* particle)
{
  if (IsIon(particle)) {
    fIonList->remove(particle);
  } else {
#ifdef G4VERBOSE
    if (GetVerboseLevel()>0) {
      G4cout << "G4IonTable::Remove :" << particle->GetParticleName() ;
      G4cout << " is not ions" << endl; 
    }
#endif
  }
}


void G4IonTable::DumpTable(const G4String &particle_name) const
{
  for (G4int idx= 0; idx < fIonList->entries() ; idx++) {
    if (( particle_name == "ALL" ) || (particle_name == "all")){
      ((*fIonList)(idx))->DumpTable();
    } else if ( particle_name == ((*fIonList)(idx))->GetParticleName() ) {
      ((*fIonList)(idx))->DumpTable();
    }
  }
}

const G4String G4IonTable::elementName[] = {
  "H",                                                                                                "He", 
  "Li", "Be",                                                             "B",  "C",  "N",  "O", "F", "Ne", 
  "Na", "Mg",                                                             "Al", "Si", "P", "S", "Cl", "Ar", 
  "K", "Ca", "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn", "Ga", "Ge", "As", "Se", "Br", "Kr", 
  "Rb", "Sr", "Y", "Zr", "Nb", "Mo","Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn", "Sb", "Te", "I",  "Xe", 
  "Cs", "Ba", 
              "La", "Ce", "Pr", "Nd", "Pr", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb", "Lu", 
                   "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", 
  "Fr", "Ra", 
              "Ac", "Th", "Pa",  "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No", "Lr",
              "Db", "Jl", "Rf", "Bh", "Hn", "Mt", "Xa"
  
};



































