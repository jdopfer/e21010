#include "projectutil.h"
#include <iostream>
#include <string>
#include <ausa/json/IO.h>
#include <ausa/sort/analyzer/AbstractSortedAnalyzer.h>
#include <ausa/util/StringUtil.h>
#include <ausa/util/FileUtil.h>
#include <ausa/setup/DoubleSidedSiliconDetector.h>
#include <ausa/setup/SingleSidedSiliconDetector.h>
#include <ausa/util/DynamicBranchVector.h>
#include <ausa/eloss/Ion.h>
#include <ausa/eloss/Default.h>
#include <Math/Vector3D.h>
#include <TROOT.h>
#include <libconfig.h++>
#include <filesystem>

using namespace std;
using namespace AUSA;
using namespace ROOT::Math;
using namespace AUSA::Sort;
using namespace AUSA::EnergyLoss;
using namespace libconfig;
using namespace EUtil;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Please provide a path to a config file as the first and only argument" << endl;
    return -1;
  }
  Config cfg;
  cfg.readFile(argv[1]);
  const Setting &root = cfg.getRoot();

  // load setup and target specs
  string setup_path, target_path;
  if (cfg.lookup("paths_relative_to_project_root")) {
    setup_path  = getProjectRoot() + "/" + cfg.lookup("setup").c_str();
    target_path = getProjectRoot() + "/" + cfg.lookup("target").c_str();
  } else {
    setup_path  = cfg.lookup("setup").c_str();
    target_path = cfg.lookup("target").c_str();
  }

  string pwd = std::filesystem::current_path();
  cout << "# Output created from within " << pwd << " with the following command" << endl << "# ";
  vector<string> args(argv, argv + argc);
  for (const string& s : args) {
    cout << s << " ";
  }
  cout << endl;

  cout << "# Setup: " << setup_path << endl;
  cout << "# Target: " << target_path << endl;
  auto setup = JSON::readSetupFromJSON(setup_path);
  auto target = JSON::readTargetFromJSON(target_path);

  vector<unique_ptr<EnergyLossRangeInverter>> rangeCalcs;
  const Setting &ions = root["ions"];
  int num_ions = ions.getLength();
  cout << "# Ions: ";
  for (Setting const &ion: ions) {
    cout << ion.c_str();
    if (ion.c_str() != ions[num_ions - 1]) cout << ", ";
    unique_ptr<EnergyLossRangeInverter> detectorCalc = defaultRangeInverter(ion, "Silicon");
    rangeCalcs.emplace_back(std::move(detectorCalc));
  }
  cout << endl << endl;

  for (int i = 0; i < root.lookup("thicknesses").lookup("names").getLength(); i++) {
    auto det = setup->getDSSD((string) root.lookup("thicknesses").lookup("names")[i]);
    double tMin = root.lookup("thicknesses").lookup("min")[i];
    double tStep = root.lookup("thicknesses").lookup("step")[i];
    double tMax = root.lookup("thicknesses").lookup("max")[i];

    int steps = (tMax - tMin)/tStep + 1;
    bool tMaxMissed = tMin + (steps-1)*tStep < tMax;

    // procure all pixel positions or (worst-case) strip positions to be evaluated
    vector<TVector3> pixel_positions;
    unsigned int pixel_evals = det->frontStripCount() * det->backStripCount();
    for (int j = 1; j < det->frontStripCount() + 1; j++) {
      for (int k = 1; k < det->backStripCount() + 1; k++) {
        pixel_positions.emplace_back(det->getPixelPosition(j, k));
      }
    }

    for (int j = 0; j < steps + tMaxMissed; j++) {
      double t = tMin + j*tStep > tMax ? tMax : tMin + j*tStep;

      for (int k = 0; k < rangeCalcs.size(); k++) {
        EnergyLossRangeInverter *detectorCalc = rangeCalcs[k].get();

        vector<double> Edep_max, thetas;
        double theta;
        for (auto &pos: pixel_positions) {
          // angle of incidence between particle and detector surface
          theta = (pos - target.getCenter()).Angle(-det->getNormal());
          thetas.emplace_back(theta);

          // threshold deposited energy (observed without applying energy corrections) in front detector just before particle punches through
          Edep_max.emplace_back(detectorCalc->getTotalEnergyCorrection(0.0, /* mm -> um */ 1e-3*t / cos(theta)));
        }

        cout << "######## ION: " << ions[k].c_str() << "\t DSSD: " << det->getName() << " ########" << endl;
        cout << "#t\tv\tEdep_max" << endl;
        for (int l = 0; l < pixel_evals; l++) {
          cout << t << "\t" << thetas[l] << "\t" << Edep_max[l] << endl;
        }
        cout << endl;
      }
    }
  }

  return 0;
}