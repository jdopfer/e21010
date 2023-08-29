#ifndef ANALYSISCONFIG_H
#define ANALYSISCONFIG_H

#define TWO_P_DEFAULT "20Ne"

#include <string>
#include <cstdio>
#include <libconfig.h++>
#include <ausa/json/IO.h>
#include "projectutil.h"
#include "runTabulations.h"

using namespace std;
using namespace libconfig;
using namespace EUtil;
using namespace AUSA;

string setup_path, target_path, input_path, output_dir;
string specificAnalysis;
bool exclude_hpges, exclude_U5, include_dsd_rim, include_beta_region, include_spurious_zone;
bool auto_2p_daughter;
double implantation_depth;
string twoPDaughter;

Config cfg;
string path;

void prepareFileIO(const string& configfile) {
  path = configfile;
  cfg.readFile(path.c_str());

  if (cfg.lookup("paths_relative_to_project_root")) {
    setup_path  = getProjectRoot() + "/" + cfg.lookup("setup_file").c_str();
    target_path = getProjectRoot() + "/" + cfg.lookup("target_file").c_str();
    input_path  = getProjectRoot() + "/" + cfg.lookup("data_input").c_str();
    if (cfg.exists("auto_data_output_dir") && cfg.lookup("auto_data_output_dir")) {
      output_dir  = getProjectRoot() + "/data/" + getStem(configfile);
    } else {
      output_dir  = getProjectRoot() + "/" + cfg.lookup("data_output_dir").c_str();
    }
  } else {
    setup_path  = cfg.lookup("setup_file").c_str();
    target_path = cfg.lookup("target_file").c_str();
    input_path  = cfg.lookup("data_input").c_str();
    if (cfg.exists("auto_data_output_dir") && cfg.lookup("auto_data_output_dir")) {
      output_dir  = "data/" + getStem(configfile);
    } else {
      output_dir  = cfg.lookup("data_output_dir").c_str();
    }
  }
}

void prepareAnalysis(unsigned int run_number) {
  specificAnalysis = cfg.lookup("specificAnalysis").c_str();

  auto target = JSON::readTargetFromJSON(target_path);
  implantation_depth = cfg.exists("implantation_depth") ? cfg.lookup("implantation_depth") : 1e6*target.getThickness()/2.; // nm
  implantation_depth /= 1e6; // mm

  auto_2p_daughter = cfg.exists("auto_2p_daughter") && cfg.lookup("auto_2p_daughter");
  if (auto_2p_daughter) {
    twoPDaughter = get2PDaughter(run_number);
    if (twoPDaughter == IS_NOT_POSSIBLE) {
      cerr << "Cannot have 2p daughter in run number " << run_number << endl
           << "Continuing with default value \"" << TWO_P_DEFAULT << "\" (which is probably harmless!)" << endl
           << "Turn off \"auto_2p_daughter\" in " << path << " to disable this warning." << endl;
      twoPDaughter = TWO_P_DEFAULT;
    }
  } else {
    twoPDaughter = cfg.exists("twoPDaughter") ? cfg.lookup("twoPDaughter") : TWO_P_DEFAULT;
  }

  exclude_hpges = cfg.exists("exclude_hpges") && cfg.lookup("exclude_hpges");
  exclude_U5 = cfg.exists("exclude_U5") && cfg.lookup("exclude_U5");
  include_dsd_rim = cfg.exists("include_dsd_rim") && cfg.lookup("include_dsd_rim");
  include_beta_region = cfg.exists("include_beta_region") && cfg.lookup("include_beta_region");
  include_spurious_zone = cfg.exists("include_spurious_zone") && cfg.lookup("include_spurious_zone");
}

void printConfig() {
  if (cfg.lookup("verbose")) {
    cout << "---------------------------- Configuration ------------------------------" << endl
         << "Specific analysis:     " << specificAnalysis                               << endl
         << "Setup:                 " << setup_path                                     << endl
         << "Target:                " << target_path                                    << endl
         << "Input:                 " << input_path                                     << endl
         << "Output:                " << output_dir                                     << endl
         << "Implantation depth:    " << implantation_depth*1e6 << " nm"                << endl
         << "Two-proton daughter:   " << twoPDaughter                                   << endl
         << "Exclude HPGes:         " << exclude_hpges                                  << endl
         << "Exclude U5:            " << exclude_U5                                     << endl
         << "Include DSD rims:      " << include_dsd_rim                                << endl
         << "Include beta region:   " << include_beta_region                            << endl
         << "Include spurious zone: " << include_spurious_zone                          << endl
         << "-------------------------------------------------------------------------" << endl << endl;
  }
}

#endif //ANALYSISCONFIG_H
