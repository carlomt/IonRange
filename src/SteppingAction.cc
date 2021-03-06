//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file electromagnetic/TestEm7/src/SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
// $Id: SteppingAction.cc 101250 2016-11-10 08:54:02Z gcosmo $
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "Randomize.hh"
#include "EventAction.hh"
#include "G4UnitsTable.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* det, RunAction* RuAct, EventAction* EvAct)
  :G4UserSteppingAction(),fDetector(det), fRunAction(RuAct), fEventAction(EvAct)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4double edep = step->GetTotalEnergyDeposit();
  if (edep <= 0.) return;

  G4StepPoint* prePoint  = step->GetPreStepPoint();
  G4StepPoint* postPoint = step->GetPostStepPoint();

  G4int copyNb = prePoint->GetTouchableHandle()->GetCopyNumber();
  if (copyNb > 0) { fRunAction->FillTallyEdep(copyNb-1, edep); }

  G4double niel = step->GetNonIonizingEnergyDeposit();
  fRunAction->FillEdep(edep, niel);
  
  if (step->GetTrack()->GetTrackID() == 1) {
    fRunAction->AddPrimaryStep();
    /*
    G4cout << step->GetTrack()->GetMaterial()->GetName()
           << "  E1= " << step->GetPreStepPoint()->GetKineticEnergy()
           << "  E2= " << step->GetPostStepPoint()->GetKineticEnergy()
           << " Edep= " << edep 
           << " Q= " << step->GetTrack()->GetDynamicParticle()->GetCharge()
           << " Qp= " << step->GetPostStepPoint()->GetCharge()
           << G4endl;
    */
    // G4cout << "SteppingAction::UserSteppingAction step length     \t"
    // 	   // << G4BestUnit(step->GetStepLength(),"Length") << G4endl;
    // 	   << step->GetStepLength()/CLHEP::mm << " mm " << G4endl;

    // fRunAction->AddThisTotalRange(step->GetStepLength());
    // fEventAction->AddPrimaryTrackLength(step->GetStepLength());
  } 

  //Bragg curve
  //        
  // G4double xmax = fDetector->GetAbsorSizeX();
  // G4double ymax = fDetector->GetAbsorSizeYZ();
  // G4double zmax = fDetector->GetAbsorSizeYZ();
   
  G4double x1 = prePoint->GetPosition().x() ;// + xmax*0.5;
  G4double x2 = postPoint->GetPosition().x() ;// + xmax*0.5;
  G4double y1 = prePoint->GetPosition().y() ;//+ ymax*0.5;
  G4double y2 = postPoint->GetPosition().y();//+ ymax*0.5;
  G4double z1 = prePoint->GetPosition().z() ;//+ zmax*0.5;
  G4double z2 = postPoint->GetPosition().z();//+ zmax*0.5;
  // if(x1 >= 0.0 && x2 <= xmax)
    {
      G4double remEk = step->GetTrack()->GetKineticEnergy();
      G4double x  = x1 + G4UniformRand()*(x2-x1);
      G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
      analysisManager->FillH1(1, x/CLHEP::mm, edep/CLHEP::keV);  
      analysisManager->FillH1(2, x/CLHEP::mm, edep/CLHEP::keV);
      analysisManager->FillH1(4, x/CLHEP::mm, remEk/CLHEP::keV);
      analysisManager->FillH2(2, x/CLHEP::mm, remEk/CLHEP::keV, 1);
      // G4cout<< x/CLHEP::mm <<" \t "<< step->GetTrack()->GetKineticEnergy()/CLHEP::keV <<G4endl;
      
      // if (step->GetTrack()->GetTrackID() == 1)
      // 	{
      // if((y1 >= 0.0 && y2 <= ymax) && (z1 >= 0.0 && z2 <= zmax))
      	{
	  G4double y  = y1 + G4UniformRand()*(y2-y1);
	  G4double z  = z1 + G4UniformRand()*(z2-z1);
	  analysisManager->FillH2(0, y1/CLHEP::mm, z1/CLHEP::mm, edep/CLHEP::keV);
	  analysisManager->FillH2(1, x1/CLHEP::mm, y1/CLHEP::mm, edep/CLHEP::keV);

      // 	  G4double dx  = (x2-x1);
      // 	  G4double dy  = (y2-y1);
      // 	  G4double dz  = (z2-z1);
      // 	  G4double ss = std::sqrt(dx*dx + dy*dy + dz*dz);
      // 	  G4cout << "SteppingAction::UserSteppingAction step manually calc \t"
      // 		 << ss/CLHEP::mm << " mm " << G4endl;
      // 	  G4cout <<" prePoint:  ("<<x1<<", "<<y1<<", "<<z1<<")" <<G4endl;
      // 	  G4cout <<" postPoint: ("<<x2<<", "<<y2<<", "<<z2<<")" <<G4endl;
      // // 	  fRunAction->AddThisTotalRange(ss);
      // 	}
	}
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


