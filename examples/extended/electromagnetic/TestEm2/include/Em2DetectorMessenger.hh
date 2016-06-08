// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: Em2DetectorMessenger.hh,v 1.2 1999/12/15 14:48:59 gunter Exp $
// GEANT4 tag $Name: geant4-03-00 $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#ifndef Em2DetectorMessenger_h
#define Em2DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class Em2DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWith3Vector;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class Em2DetectorMessenger: public G4UImessenger
{
  public:
    Em2DetectorMessenger(Em2DetectorConstruction* );
   ~Em2DetectorMessenger();
    
    void SetNewValue(G4UIcommand*, G4String);
    
  private:
    Em2DetectorConstruction* Em2Detector;
    
    G4UIdirectory*             Em2detDir;
    G4UIcmdWithAString*        MaterCmd;
    G4UIcmdWith3Vector*        LBinCmd;
    G4UIcmdWith3Vector*        RBinCmd;
    G4UIcmdWithADoubleAndUnit* FieldCmd;
    G4UIcmdWithoutParameter*   UpdateCmd;
};

#endif
