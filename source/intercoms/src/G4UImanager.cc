// This code implementation is the intellectual property of
// the RD44 GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4UImanager.cc,v 1.1 1999/01/07 16:09:27 gunter Exp $
// GEANT4 tag $Name: geant4-00-01 $
//
// 
// ---------------------------------------------------------------------

#include "G4UImanager.hh"
#include "G4UIcommandStatus.hh"
#include "G4UIcommandTree.hh"
#include "G4UIcommand.hh"
#include "G4UIsession.hh"
#include "G4UIbatch.hh"
#include "G4UIcontrolMessenger.hh"
#include "G4UnitsMessenger.hh"
#include "G4ios.hh"
#include "G4strstreambuf.hh"

#ifdef WIN32
#  include <Strstrea.h>
#else
#  include <strstream.h>
#endif

G4UImanager * G4UImanager::fUImanager = 0;
G4bool G4UImanager::fUImanagerHasBeenKilled = false;

G4UImanager * G4UImanager::GetUIpointer()
{
  if(!fUImanager) 
  {
    if(!fUImanagerHasBeenKilled)
    {
      fUImanager = new G4UImanager;
      fUImanager->CreateMessenger();
    }
  }
  return fUImanager;
}

G4UImanager::G4UImanager():savedCommand(NULL)
{
  treeTop = new G4UIcommandTree("/");
  G4String nullString;
  savedParameters = nullString;
  verboseLevel = 0;
  saveHistory = false;
  session = NULL;
  SetCoutDestination(session);
}

void G4UImanager::CreateMessenger()
{
  UImessenger = new G4UIcontrolMessenger;
  UnitsMessenger = new G4UnitsMessenger;
}

G4UImanager::~G4UImanager()
{
  SetCoutDestination(NULL);
  histVec.clear();
  if(saveHistory) historyFile.close();
  delete UImessenger;
  delete UnitsMessenger;
  delete treeTop;
  fUImanagerHasBeenKilled = true;
  fUImanager = NULL;
}

G4UImanager::G4UImanager(const G4UImanager &right) { }
const G4UImanager & G4UImanager::operator=(const G4UImanager &right)
{ return right; }
int G4UImanager::operator==(const G4UImanager &right) const
{ return false; }
int G4UImanager::operator!=(const G4UImanager &right) const
{ return true; }

G4String G4UImanager::GetCurrentValues(const char * aCommand)
{
  G4String theCommand = aCommand;
  savedCommand = treeTop->FindPath( theCommand );
  if( savedCommand == NULL ) 
  {
    G4cerr << "command not found" << endl;
    return G4String();
  }
  return savedCommand->GetCurrentValue();
}

G4String G4UImanager::GetCurrentStringValue(const char * aCommand, 
int parameterNumber, G4bool reGet)
{
  if(reGet || savedCommand == NULL)
  {
    savedParameters = GetCurrentValues( aCommand );
  }
  RWCTokenizer savedToken( savedParameters );
  G4String token;
  for(int i_thParameter=0;i_thParameter<parameterNumber;i_thParameter++)
  {
    token = savedToken();
    if( token.isNull() ) return G4String();
    if( token[(size_t)0] == '"' )
    {
      token.append(" ");
      token.append(savedToken("\""));
    }
  }
  return token;
}

G4String G4UImanager::GetCurrentStringValue(const char * aCommand, 
const char * aParameterName, G4bool reGet)
{
  if(reGet || savedCommand == NULL)
  {
    G4String parameterValues = GetCurrentValues( aCommand );
  }
  for(int i=0;i<savedCommand->GetParameterEntries();i++)
  {
    if( aParameterName ==
      savedCommand->GetParameter(i)->GetParameterName() )
      return GetCurrentStringValue(aCommand,i+1,false);
  }
  return G4String();
}

G4int G4UImanager::GetCurrentIntValue(const char * aCommand,
const char * aParameterName, G4bool reGet)
{
  G4String targetParameter = 
     GetCurrentStringValue( aCommand, aParameterName, reGet );
  G4int value;
  const char* t = targetParameter;
  istrstream is((char*)t);
  is >> value;
  return value;
}

G4int G4UImanager::GetCurrentIntValue(const char * aCommand,
int parameterNumber, G4bool reGet)
{
  G4String targetParameter = 
     GetCurrentStringValue( aCommand, parameterNumber, reGet );
  G4int value;
  const char* t = targetParameter;
  istrstream is((char*)t);
  is >> value;
  return value;
}

G4double G4UImanager::GetCurrentDoubleValue(const char * aCommand,
const char * aParameterName, G4bool reGet)
{
  G4String targetParameter = 
     GetCurrentStringValue( aCommand, aParameterName, reGet );
  G4double value;
  const char* t = targetParameter;
  istrstream is((char*)t);
  is >> value;
  return value;
}

G4double G4UImanager::GetCurrentDoubleValue(const char * aCommand,
int parameterNumber, G4bool reGet)
{
  G4String targetParameter = 
     GetCurrentStringValue( aCommand, parameterNumber, reGet );
  G4double value;
  const char* t = targetParameter;
  istrstream is((char*)t);
  is >> value;
  return value;
}

void G4UImanager::AddNewCommand(G4UIcommand * newCommand)
{
  treeTop->AddNewCommand( newCommand );
}

void G4UImanager::RemoveCommand(G4UIcommand * aCommand)
{
  treeTop->RemoveCommand( aCommand );
}

void G4UImanager::ExecuteMacroFile(G4String fileName)
{
  G4UIsession* batchSession = new G4UIbatch(fileName,session);
  session = batchSession;
  G4UIsession* previousSession = session->SessionStart();
  delete session;
  session = previousSession;
}

int G4UImanager::ApplyCommand(char * aCommand)
{
  G4String theCommand = aCommand;
  return ApplyCommand(theCommand);
}

int G4UImanager::ApplyCommand(G4String aCommand)
{
  if(verboseLevel) G4cout << aCommand << endl;

  G4String commandString;
  G4String commandParameter;
  int i = aCommand.index(" ");
  if( i != RW_NPOS )
  {
    commandString = aCommand(0,i);
    commandParameter = aCommand(i+1,aCommand.length()-(i+1));
  }
  else
  {
    commandString = aCommand;
  }

  G4UIcommand * targetCommand = treeTop->FindPath( commandString );
  if( targetCommand == NULL )
  {
    // G4cout << commandString << " NOT FOUND." << endl;
    return fCommandNotFound;
  }

  if(!(targetCommand->IsAvailable())) 
  { return fIllegalApplicationState; }
 
  if(saveHistory) historyFile << aCommand << endl; 
  histVec.insert(aCommand);
  return targetCommand->DoIt( commandParameter );
}

void G4UImanager::StoreHistory(G4String fileName)
{ StoreHistory(true,fileName); }

void G4UImanager::StoreHistory(G4bool historySwitch,G4String fileName)
{
  if(historySwitch)
  {
    if(saveHistory)
    { historyFile.close(); }
    const char* theFileName = fileName;
    historyFile.open((char*)theFileName);
    saveHistory = true;
  }
  else
  {
    historyFile.close();
    saveHistory = false;
  }
  saveHistory = historySwitch;
}

void G4UImanager::PauseSession(G4String msg)
{
  if(session) session->PauseSessionStart(msg); 
}

void G4UImanager::ListCommands(G4String direct)
{
  G4UIcommandTree* comTree = FindDirectory(direct);
  if(comTree)
  { comTree->List(); }
  else
  { G4cout << direct << " is not found." << endl; }
}

G4UIcommandTree* G4UImanager::FindDirectory(const char* dirName)
{
  G4String aDirName = dirName;
  G4String targetDir = aDirName.strip(G4String::both);
  if( targetDir( targetDir.length()-1 ) != '/' )
  { targetDir += "/"; }
  G4UIcommandTree* comTree = treeTop;
  if( targetDir == "/" )
  { return comTree; }
  int idx = 1;
  while( idx < targetDir.length()-1 )
  {
    int i = targetDir.index("/",idx);
    comTree = comTree->GetTree(targetDir(0,i+1));
    if( comTree == NULL )
    { return NULL; }
    idx = i+1;
  }
  return comTree;
}

void G4UImanager::Interact()
{
  Interact(G4String("G4> "));
}

void G4UImanager::Interact(char * pC)
{
  G4String cc = pC;
  Interact(cc);
}

void G4UImanager::Interact(G4String pC)
{
  G4cerr << "G4UImanager::Interact() is out of date and is not used anymore." << endl;
  G4cerr << "This method will be removed shortly!!!" << endl;
  G4cerr << "In case of main() use" << endl;
  G4cerr << "    G4UIsession * session = new G4UIterminal;" << endl;
  G4cerr << "    session->SessionStart();" << endl;
  G4cerr << "In other cases use" << endl;
  G4cerr << "    G4StateManager::GetStateManager()->Pause();" << endl;
}



 void G4UImanager::SetCoutDestination(G4UIsession *const value)
{
#ifdef G4STREAM
    G4coutbuf.SetDestination(value);
    G4cerrbuf.SetDestination(value);
#endif
}

