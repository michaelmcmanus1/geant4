// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4MPVEntry.cc,v 1.2 1999/04/14 12:49:02 maire Exp $
// GEANT4 tag $Name: geant4-00-01 $
//
// 
////////////////////////////////////////////////////////////////////////
// G4MPVEntry Class Implementation
////////////////////////////////////////////////////////////////////////
//
// File:        G4MPVEntry.cc
// Version:     1.0
// Created:     1996-02-08
// Author:      Juliet Armstrong
// Updated:     1997-03-25 by Peter Gumplinger
//              > cosmetics (only)
// mail:        gum@triumf.ca
//
////////////////////////////////////////////////////////////////////////

#include "G4MPVEntry.hh"

/////////////////////////
// Class Implementation
/////////////////////////

        //////////////
        // Operators
        //////////////

// Overload the == operator
// ------------------------
// Well defined == semantics required by RWTPtrSortedVector
//
G4bool G4MPVEntry::operator ==(const G4MPVEntry &right) const  
{
	if (thePhotonMomentum == right.thePhotonMomentum) 
		return true;
	else
		return false; 
}

// Overload the < operator
// -----------------------
// Well defined < semantics required by RWTPtrSortedVector
//
G4bool G4MPVEntry::operator <(const G4MPVEntry &right) const  
{
	if (thePhotonMomentum < right.thePhotonMomentum) 
		return true;
	else
		return false;
}

// Overload the = operator
// -----------------------
// Well defined = semantics required by RWTPtrSortedVector
//
G4MPVEntry& G4MPVEntry::operator =(const G4MPVEntry& right)
{
        if (this == &right) return *this;
	
        thePhotonMomentum = right.thePhotonMomentum;
        theProperty = right.theProperty;
        return *this;
}

        /////////////////
        // Constructors
        /////////////////

G4MPVEntry::G4MPVEntry(G4double aPhotonMomentum, G4double aProperty)
{
        thePhotonMomentum = aPhotonMomentum;
        theProperty = aProperty;
}

G4MPVEntry::G4MPVEntry(const G4MPVEntry &right)
{
        thePhotonMomentum = right.thePhotonMomentum;
        theProperty = right.theProperty;
}


        ////////////////
        // Destructors
        ////////////////

G4MPVEntry::~G4MPVEntry(){}

        ////////////
        // Methods
        ////////////

void G4MPVEntry::DumpEntry()
{
	G4cout << "(" 
	     << thePhotonMomentum 
	     << ", " 
	     << theProperty
 	     << ")" 
	     << endl;	
}