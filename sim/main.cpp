#include <G4Sim/UserActions/EventAction.hh>
#include <G4Sim/Analysis/UnpackedAnalysisManager.hh>
#include <G4Sim/Analysis/GPSBookkeepingManager.hh>
#include <G4RunManager.hh>
#include <G4Sim/Geometry/GeometryConstruction.hh>
#include <G4UImanager.hh>
#include <G4VisExecutive.hh>
#include <G4Sim/UserActions/RunAction.hh>
#include <G4UIExecutive.hh>
#include "projectutil.h"

using namespace EUtil;

int main(int argc, char **argv) {
  auto pRunManager = new G4RunManager;
  auto mult = new MultipleAnalysisManager();
  mult->attach("sim", new UnpackedAnalysisManager());
  mult->attach("sim", new GPSBookkeepingManager());

  auto messenger = new AusalibMessenger(pRunManager, mult);

  auto UImanager = G4UImanager::GetUIpointer();
  G4cout << " UI session starts ..." << G4endl;
  auto ui = new G4UIExecutive(argc, argv);

  UImanager->ApplyCommand("/control/macroPath " + getProjectRoot() + "/sim/res/macros/");

  G4String fileName = argv[1];
  bool vis = false; G4VisManager* visManager;
  if (argc == 1) {
    UImanager->ApplyCommand("/control/execute res/macros/run.mac");
  } else {
    if (G4StrUtil::contains(fileName, "vis.mac")) {
      vis = true;
      visManager = new G4VisExecutive();
      visManager->Initialize();
    }
    UImanager->ApplyCommand("/control/execute " + fileName);
    if (vis) {
      ui->SessionStart();
      delete visManager;
    }
  }
  mult->finish();

  delete messenger;
  delete ui;
  delete pRunManager;
  return 0;
}