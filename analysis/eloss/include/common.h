//
// Created by erik on 3/21/23.
//

#ifndef BANANA_DRAWER_COMMON_H
#define BANANA_DRAWER_COMMON_H

#include <string>
#include <libconfig.h++>
#include <ausa/json/IO.h>
#include <iostream>
#include <string>
#include <TROOT.h>
#include "projectutil.h"
#include <iostream>
#include <string>
#include <ausa/sort/analyzer/AbstractSortedAnalyzer.h>
#include <ausa/util/StringUtil.h>
#include <ausa/util/FileUtil.h>
#include <ausa/setup/DoubleSidedSiliconDetector.h>
#include <ausa/setup/SingleSidedSiliconDetector.h>
#include <ausa/util/DynamicBranchVector.h>
#include <ausa/eloss/Ion.h>
#include <ausa/eloss/Default.h>
#include <Math/Vector3D.h>
#include <libconfig.h++>
#include <filesystem>

using namespace std;
using namespace libconfig;
using namespace EUtil;
using namespace AUSA;
using namespace AUSA::EnergyLoss;

vector<string> args;
Config cfg;
string dE_detector;
double fdlF, ft, fdlB, bdlF, bt;
string setup_path, target_path;
shared_ptr<Setup> setup;
shared_ptr<Target> target;
typedef pair<unique_ptr<EnergyLossRangeInverter>, vector<shared_ptr<EnergyLossRangeInverter>>> range_pair;
vector<range_pair> rangeCalcs;
shared_ptr<Telescope> tlscp;
shared_ptr<const Detector> fptr, bptr;
shared_ptr<const DoubleSidedDetector> fdsd, bdsd;
shared_ptr<const SingleSidedDetector> fssd, bssd;
bool f_is_dsd = false, b_is_dsd = false;
bool telescope_found = false;
vector<TVector3> pixel_positions;
shared_ptr<const Detector> det;
double implantation_depth;
TVector3 implantation_offset;
TVector3 origin;

void init(int argc, char* argv[]) {
  if (argc < 3) {
    cout << "Usage: " << argv[0]
         << " <config file à la provided params.cfg> <Delta-E detector name> [fdlF=<thickness>] [ft=<thickness>] [fdlB=<thickness>] [bdlF=<thickness>] [bt=<thickness>]" << endl << endl
         << "Particle first passes through target/catcher foil\n"
         << "Particle then passes through the front detector's deadlayer on the front-side with thickness 'fdlF'\n"
         << "Particle then passes through the active layer of the front detector 'ft'\n"
         << "Particle then passes through the front detector's deadlayer on the back-side 'fdlB'\n"
         << "Particle then passes through the back detector's deadlayer on the front-side 'bdlF'\n"
         << "Particle then passes through the active layer of the back detector 'bt'\n"
         << "If the particle proceeds beyond this final active detector layer, we are out of luck..." << endl << endl
         << "Units of thicknesses are μm" << endl
         << "If not provided as command line arguments, thicknesses will be the ones specified in the setup file" << endl
         << "The detector telescope of interest must be defined in the \"telescopes\" array in the setup file" << endl;
    exit(-1);
  }

  // parse command line arguments
  for (int i = 0; i < argc; i++) {
    args.emplace_back(argv[i]);
  }
  cfg.readFile(args[1]);
  Setting &root = cfg.getRoot();
  dE_detector = args[2];
  fdlF = ft = fdlB = bdlF = bt = -42.;
  for (int i = 3; i < size(args); i++) {
    if (args[i].find("fdlF=") != string::npos) fdlF = stod(args[i].substr(5), nullptr) * 1e-3;
    if (args[i].find("ft=") != string::npos) ft = stod(args[i].substr(3), nullptr) * 1e-3;
    if (args[i].find("fdlB=") != string::npos) fdlB = stod(args[i].substr(5), nullptr) * 1e-3;
    if (args[i].find("bdlF=") != string::npos) bdlF = stod(args[i].substr(5), nullptr) * 1e-3;
    if (args[i].find("bt=") != string::npos) bt = stod(args[i].substr(3), nullptr) * 1e-3;
  }

  // load AUSALIB setup and target specs
  if (cfg.lookup("paths_relative_to_project_root")) {
    setup_path = getProjectRoot() + "/" + cfg.lookup("setup").c_str();
    target_path = getProjectRoot() + "/" + cfg.lookup("target").c_str();
  } else {
    setup_path = cfg.lookup("setup").c_str();
    target_path = cfg.lookup("target").c_str();
  }
  setup = JSON::readSetupFromJSON(setup_path);
  target = make_shared<Target>(JSON::readTargetFromJSON(target_path));
  implantation_depth = cfg.exists("implantation_depth") ? cfg.lookup("implantation_depth") : 1e6*target->getThickness()/2.; // nm
  implantation_depth /= 1e6; // mm

  // logging
  string pwd = std::filesystem::current_path();
  time_t now = time(nullptr);
  char* datetime = ctime(&now);
  cout << "# " << datetime
       << "# Output created from within " << pwd << " with the following command" << endl << "# ";
  for (const string &s: args) {
    cout << s << " ";
  }
  cout << endl
       << "# Setup: " << setup_path << endl
       << "# Target: " << target_path << endl
       << "# Implantation depth is " << implantation_depth*1e6 << " nm in target of thickness " << 1e6*target->getThickness() << " nm" << endl;
  implantation_offset = {0., 0., implantation_depth - target->getThickness()/2.};
  origin = target->getCenter() - implantation_offset;

  // instantiate range inverters with the specified ions
  const Setting &ions = root["ions"];
  for (Setting const &ion: ions) {
    unique_ptr<EnergyLossRangeInverter> detectorCalc = defaultRangeInverter(ion, "Silicon");
    vector<shared_ptr<EnergyLossRangeInverter>> targetCalcs;
    for (auto &layer: target->getLayers()) {
      targetCalcs.emplace_back(defaultRangeInverter(Ion::predefined(ion), layer.getMaterial()));
    }
    rangeCalcs.emplace_back(make_pair(std::move(detectorCalc), std::move(targetCalcs)));
  }

  // find relevant telescope and set various detector layer thicknesses
  for (int i = 0; i < setup->telescopeCount(); i++) {
    tlscp = setup->getTelescope(i);
    fptr = tlscp->getFrontPtr();
    if (fptr->getName() != dE_detector) continue;
    bptr = tlscp->getBackPtr();

    if (dynamic_cast<const DoubleSidedDetector *>(fptr.get())) {
      f_is_dsd = true;
      fdsd = dynamic_pointer_cast<const DoubleSidedDetector>(fptr);
      fdlF = fdlF < 0. ? getFrontDeadLayer(*fdsd) : fdlF;
      ft   = ft   < 0. ? getThickness(*fdsd)      : ft;
      fdlB = fdlB < 0. ? getBackDeadLayer(*fdsd)  : fdlB;
    } else {
      fssd = dynamic_pointer_cast<const SingleSidedDetector>(fptr);
      fdlF = fdlF < 0. ? getFrontDeadLayer(*fssd) : fdlF;
      ft   = ft   < 0. ? getThickness(*fssd)      : ft;
      fdlB = fdlB < 0. ? getBackDeadLayer(*fssd)  : fdlB;
    }

    if (dynamic_cast<const DoubleSidedDetector *>(bptr.get())) {
      b_is_dsd = true;
      bdsd = dynamic_pointer_cast<const DoubleSidedDetector>(bptr);
      bdlF = bdlF < 0. ? getFrontDeadLayer(*bdsd) : bdlF;
      bt   = bt   < 0. ? getThickness(*bdsd)      : bt;
    } else {
      bssd = dynamic_pointer_cast<const SingleSidedDetector>(bptr);
      bdlF = bdlF < 0. ? getFrontDeadLayer(*bssd) : bdlF;
      bt   = bt   < 0. ? getThickness(*bssd)      : bt;
    }

    telescope_found = true;
    break;
  }

  if (!telescope_found) {
    cout << "# " << dE_detector << " not found as Delta-E detector in telescope in " << setup_path << endl;
    exit(-2);
  }

  // procure all pixel positions or (worst-case) strip positions to be evaluated
  if (f_is_dsd) {
    det = fdsd;
    for (int i = 1; i < fdsd->frontStripCount() + 1; i++) {
      for (int j = 1; j < fdsd->backStripCount() + 1; j++) {
        pixel_positions.emplace_back(fdsd->getPixelPosition(i, j));
      }
    }
  } else if (b_is_dsd) {
    det = bdsd;
    for (int i = 1; i < bdsd->frontStripCount() + 1; i++) {
      for (int j = 1; j < bdsd->backStripCount() + 1; j++) {
        pixel_positions.emplace_back(bdsd->getPixelPosition(i, j));
      }
    }
  } else {
    det = fssd;
    for (int i = 0; i < fssd->segmentCount(); i++) {
      pixel_positions.emplace_back(fssd->getPosition(i));
    }
  }
}

void print_telescope(const string& fName, double fThresh, double fDlF, double fT, double fDlB,
                     const string& bName, double bThresh, double bDlF, double bT) {
  cout << "# TELESCOPE: "
       << fptr->getName() << "(" << fThresh << " keV; " << fDlF * 1e3 << " um; " << fT * 1e3 << " um; " << fDlB * 1e3 << " um), "
       << bptr->getName() << "(" << bThresh << " keV; " << bDlF * 1e3 << " um; " << bT * 1e3 << " um)" << endl;
}

#endif //BANANA_DRAWER_COMMON_H
