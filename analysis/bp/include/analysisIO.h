#ifndef ANALYSISIO_H
#define ANALYSISIO_H

#include <string>
#include <cstdio>
#include <libconfig.h++>
#include <ausa/json/IO.h>
#include "projectutil.h"

using namespace std;
using namespace libconfig;
using namespace EUtil;
using namespace AUSA;

string setup_path, target_path, input_path, output_dir;
string specificAnalysis;
bool exclude_clovers, exclude_U5, include_dsd_rim, include_beta_region, include_spurious_zone;
double implantation_depth;
string twoPDaughter;
void prepareFileIO(const string& configfile) {
  Config cfg;
  cfg.readFile(configfile.c_str());

  specificAnalysis = cfg.lookup("specificAnalysis").c_str();

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

  auto target = JSON::readTargetFromJSON(target_path);
  implantation_depth = cfg.exists("implantation_depth") ? cfg.lookup("implantation_depth") : 1e6*target.getThickness()/2.; // nm
  implantation_depth /= 1e6; // mm

  twoPDaughter = cfg.exists("twoPDaughter") ? cfg.lookup("twoPDaughter") : "20Ne"; // nm

  exclude_clovers = cfg.exists("exclude_clovers") && cfg.lookup("exclude_clovers");
  exclude_U5 = cfg.exists("exclude_U5") && cfg.lookup("exclude_U5");
  include_dsd_rim = cfg.exists("include_dsd_rim") && cfg.lookup("include_dsd_rim");
  include_beta_region = cfg.exists("include_beta_region") && cfg.lookup("include_beta_region");
  include_spurious_zone = cfg.exists("include_spurious_zone") && cfg.lookup("include_spurious_zone");

  if (cfg.lookup("verbose")) {
    cout << "---------------------------- Configuration ------------------------------" << endl
         << "Specific analysis:     " << specificAnalysis                               << endl
         << "Setup:                 " << setup_path                                     << endl
         << "Target:                " << target_path                                    << endl
         << "Input:                 " << input_path                                     << endl
         << "Output:                " << output_dir                                     << endl
         << "Implantation depth:    " << implantation_depth*1e6 << " nm"                << endl
         << "Two-proton daughter:   " << twoPDaughter                                   << endl
         << "Exclude Clovers:       " << exclude_clovers                                << endl
         << "Exclude U5:            " << exclude_U5                                     << endl
         << "Include DSD rims:      " << include_dsd_rim                                << endl
         << "Include beta region:   " << include_beta_region                            << endl
         << "Include spurious zone: " << include_spurious_zone                          << endl
         << "-------------------------------------------------------------------------" << endl << endl;
  }
}

#endif //ANALYSISIO_H
