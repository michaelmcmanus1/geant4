// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4PCSGSolid.ddl,v 1.1 1999/01/07 16:10:50 gunter Exp $
// GEANT4 tag $Name: geant4-00-01 $
//
//  
// class G4CSGSolid
//
// History:
// 19.06.98 A.Kimura Converted G4CSGSolid.hh

#ifndef G4PCSGSOLID_DDL
#define G4PCSGSOLID_DDL

#include "G4PVSolid.hh"

class G4PCSGSolid : public G4PVSolid {
public:
    G4PCSGSolid(const G4String& pName);

    virtual ~G4PCSGSolid();
};

#endif