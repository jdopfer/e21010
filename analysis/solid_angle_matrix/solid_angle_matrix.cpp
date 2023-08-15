#include "projectutil.h"
#include <iostream>
#include <string>
#include <ausa/json/IO.h>
#include <ausa/sort/analyzer/AbstractSortedAnalyzer.h>
#include <ausa/util/FileUtil.h>
#include <ausa/util/DynamicBranchVector.h>
#include <TROOT.h>
#include <libconfig.h++>
#include <gsl/gsl_matrix.h>
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>

using namespace std;
using namespace AUSA;
using namespace libconfig;
using namespace rapidjson;

int main(int argc, char* argv[]) {
    Config cfg;
    cfg.readFile(EUtil::getProjectRoot() + "/analysis/solid_angle_matrix/solid_angle_matrix.cfg");
    const Setting& root = cfg.getRoot();

    // load setup and target specs
    string setup_path, target_path, matcher_path;
    if (cfg.lookup("paths_relative_to_project_root")) {
        setup_path  = EUtil::getProjectRoot() + "/" + cfg.lookup("setup").c_str();
        target_path = EUtil::getProjectRoot() + "/" + cfg.lookup("target").c_str();
        matcher_path = EUtil::getProjectRoot() + "/" + cfg.lookup("matcher").c_str();
    } else {
        setup_path  = cfg.lookup("setup").c_str();
        target_path = cfg.lookup("target").c_str();
        matcher_path = cfg.lookup("matcher").c_str();
    }
    auto setup = JSON::readSetupFromJSON(setup_path);
    auto target = JSON::readTargetFromJSON(target_path);
    cout << "# Setup: " << setup_path << endl;
    cout << "# Target: " << target_path << endl;

    // 2023-02-17: https://rapidjson.org/md_doc_stream.html
    FILE* fp = fopen(matcher_path.c_str(), "rb");
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document matcher_cfg;
    matcher_cfg.ParseStream(is);
    cout << "# Matcher: " << matcher_path << endl;

    const Setting &detectors = root["detectors"];
    map <string, pair<vector<int>, vector<int>>> disabled_strips;
    // this is not pretty, but neither is rapidjson's documentation...
    for (Setting const& detector : detectors) {
        auto det_string = detector.c_str();
        vector<int> disabled_front, disabled_back;
        for (auto const& side : {"front", "back"}) {
            if (matcher_cfg.HasMember("DSD")) {
                if (matcher_cfg["DSD"].HasMember(det_string)) {
                    if (matcher_cfg["DSD"][det_string].HasMember(side)) {
                        if (matcher_cfg["DSD"][det_string][side].HasMember("disable")) {
                            for (auto& el : matcher_cfg["DSD"][det_string][side]["disable"].GetArray()) {
                                if (side == (string) "front") {
                                    disabled_front.emplace_back(el.GetInt());
                                } else {
                                    disabled_back.emplace_back(el.GetInt());
                                }
                            }
                        }
                    }
                }
            }
        }
        disabled_strips.insert(make_pair(det_string, make_pair(disabled_front, disabled_back)));
    }

    for (Setting const& detector : detectors) {
        auto detptr = setup->getDSSD(detector.c_str());
        UInt_t fN = detptr->frontStripCount();
        gsl_matrix* solid_angles = gsl_matrix_alloc(fN, fN);
        auto disabled_front = disabled_strips[detector.c_str()].first;
        auto disabled_back = disabled_strips[detector.c_str()].second;
        for (size_t i = 0; i < solid_angles->size1; i++) {
            for (size_t j = 0; j < solid_angles->size2; j++) {
                double psa;
                if (std::find(disabled_front.begin(), disabled_front.end(), i+1) != disabled_front.end()
                    || std::find(disabled_back.begin(), disabled_back.end(), j+1) != disabled_back.end()) {
                    psa = 0.;
                } else {
                    psa = detptr->getPixelSolidAngle(i+1, j+1);
                }
                gsl_matrix_set(solid_angles, i, j, psa);
            }
        }


        cout << "# Solid angles of pixels of " << detector.c_str() << endl;
        cout << "# ";
        for (int j = 0; j < solid_angles->size2; j++) {
            cout << j + 1 << "\t";
        }
        cout << endl;
        for (size_t i = 0; i < solid_angles->size1; i++) {
            for (size_t j = 0; j < solid_angles->size2; j++) {
                cout << gsl_matrix_get(solid_angles, i, j);
                if (j + 1 != solid_angles->size2) {
                    cout << "\t";
                } else {
                    cout << "\t# " << i+1 << endl;
                }
            }
        }
    }


    return 0;
}