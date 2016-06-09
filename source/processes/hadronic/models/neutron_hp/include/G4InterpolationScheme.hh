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
// $Id: G4InterpolationScheme.hh,v 1.5.6.1 2004/02/08 09:37:30 hpw Exp $
// GEANT4 tag $Name: geant4-06-00-patch-01 $
//
#ifndef G4InterpolationScheme_h
#define G4InterpolationScheme_h 1

#include "globals.hh"

// carthesian interpolation
// unit base interpolation
// corresponding point interpolation

enum G4InterpolationScheme 
{
  START,  HISTO,  LINLIN,  LINLOG,  LOGLIN,  LOGLOG, RANDOM, 
  CSTART_HP, CHISTO, CLINLIN, CLINLOG, CLOGLIN, CLOGLOG, CRANDOM, 
  USTART, UHISTO, ULINLIN, ULINLOG, ULOGLIN, ULOGLOG, URANDOM 
};

#endif
