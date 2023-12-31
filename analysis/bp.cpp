#include <libconfig.h++>

#include "projectutil.h"
#include "Hit.h"
#include "analysisConfig.h"
#include "E21010GeneralAnalysis.h"

#include <gsl/gsl_vector.h>

#include <TROOT.h>
#include <Math/Interpolator.h>

#include <ausa/json/IO.h>
#include <ausa/setup/Target.h>
#include <ausa/eloss/Default.h>
#include <ausa/util/FileUtil.h>
#include <ausa/sort/SortedReader.h>

using namespace std;
using namespace libconfig;
using namespace EUtil;
using namespace IS507;

using namespace AUSA;
using namespace AUSA::Sort;
using namespace AUSA::EnergyLoss;

using namespace ROOT::Math;
using namespace ROOT::Math::Interpolation;

class DeltaEContainedAnalysis : public E21010GeneralAnalysis {
public:
  DeltaEContainedAnalysis(const shared_ptr<Setup>& setupSpec, const shared_ptr<Target>& target, TFile *output, double implantation_depth,
                          string twoPdaughter="20Ne",
                          bool exclude_hpges=false, bool exclude_U5=false, bool include_dsd_rim=false,
                          bool include_beta_region=false, bool include_spurious_zone=false)
          : E21010GeneralAnalysis(setupSpec, target, output, implantation_depth, twoPdaughter,
                                  exclude_hpges, exclude_U5, include_dsd_rim,
                                  include_beta_region, include_spurious_zone) {}

  void specificAnalysis() override {
    if (hits.empty()) return;

    bool veto = false;
    for (auto &hit : hits) {
      auto det = hit.det;
      switch (det->getType()) {
        case DSSSD:
          for (auto &other_hit : hits) {
            if (other_hit.det == det->getPartner()) {
              veto = true;
              break;
            }
          }
          if (!veto) {
            treatDsssdHit(&hit);
            addDssdHit(&hit);
          }
          break;
        default:
          // do nothing
          break;
      }
    }
  }
};

class BananaExplorativeAnalysis : public E21010GeneralAnalysis {
public:
  BananaExplorativeAnalysis(const shared_ptr<Setup>& setupSpec, const shared_ptr<Target>& target, TFile *output, double implantation_depth,
                            string twoPdaughter="20Ne",
                            bool exclude_hpges=false, bool exclude_U5=false, bool include_dsd_rim=false,
                            bool include_beta_region=false, bool include_spurious_zone=false)
          : E21010GeneralAnalysis(setupSpec, target, output, implantation_depth, twoPdaughter,
                                  exclude_hpges, exclude_U5, include_dsd_rim,
                                  include_beta_region, include_spurious_zone) {}

  void specificAnalysis() override {
    if (hits.empty()) return;

    unordered_set<Hit*> telescope_front_candidates;
    unordered_set<Hit*> telescope_back_candidates;
    for (auto &hit : hits) {
      auto det = hit.det;
      switch (det->getType()) {
        case DSSSD:
          if (det->hasPartner()) {
            telescope_front_candidates.emplace(&hit);
          } else { // the detector is U5
            treatDsssdHit(&hit);
            addDssdHit(&hit);
          }
          break;
        case Pad:
          telescope_back_candidates.emplace(&hit);
          break;
        default:
          // if case is 'HPGe', the scalar Eg1 and/or Eg2 already contains the gamma energy and the boolean 'g' is set to 'true'
          // do nothing
          break;
      }
    }

    unordered_set<Hit*> telescope_front_successes;
    unordered_set<Hit*> telescope_back_successes;
    for (auto front_hit : telescope_front_candidates) {
      auto front_det = front_hit->det;
      for (auto back_hit : telescope_back_candidates) {
        auto back_det = back_hit->det;
        if (front_det->getPartner() == back_det) {
          bool success = treatTelescopeHit(front_hit, back_hit);
          if (success) {
            telescope_front_successes.emplace(front_hit);
            telescope_back_successes.emplace(back_hit);
            // hits get paired multiple times here -- more advanced analyses accommodate this
            addTelescopeHit(front_hit, back_hit);
          }
        }
      }
    }

    for (auto hit : telescope_front_successes) {
      telescope_front_candidates.erase(hit);
    }
    // what remains in the set here are untreated DSSSD hits
    for (auto hit : telescope_front_candidates) {
      treatDsssdHit(hit);
      addDssdHit(hit);
    }
  }
};

class FinalAnalysis : public E21010GeneralAnalysis {
public:
  FinalAnalysis(const shared_ptr<Setup>& setupSpec, const shared_ptr<Target>& target, TFile *output, double implantation_depth,
                string twoPdaughter="20Ne",
                            bool exclude_hpges=false, bool exclude_U5=false, bool include_dsd_rim=false,
                            bool include_beta_region=false, bool include_spurious_zone=false)
          : E21010GeneralAnalysis(setupSpec, target, output, implantation_depth, twoPdaughter,
                                  exclude_hpges, exclude_U5, include_dsd_rim,
                                  include_beta_region, include_spurious_zone) {}

  void specificAnalysis() override {
    if (hits.empty()) return;

    unordered_set<Hit*> telescope_front_candidates;
    unordered_set<Hit*> telescope_back_candidates;
    for (auto &hit : hits) {
      auto det = hit.det;
      switch (det->getType()) {
        case DSSSD:
          if (det->hasPartner()) {
            telescope_front_candidates.emplace(&hit);
          } else { // the detector is U5
            treatDsssdHit(&hit);
            addDssdHit(&hit);
          }
          break;
        case Pad:
          telescope_back_candidates.emplace(&hit);
          break;
        default:
          // if case is 'HPGe', the scalar Eg1 and/or Eg2 already contains the gamma energy and the boolean 'g' is set to 'true'
          // do nothing
          break;
      }
    }

    unordered_set<Hit*> telescope_front_successes;
    unordered_set<Hit*> telescope_back_successes;
    for (auto front_hit : telescope_front_candidates) {
      if (telescope_front_successes.count(front_hit) > 0) continue;
      auto front_det = front_hit->det;
      for (auto back_hit : telescope_back_candidates) {
        if (telescope_back_successes.count(back_hit) > 0) continue;
        auto back_det = back_hit->det;
        if (front_det->getPartner() == back_det) {
          bool success = treatTelescopeHit(front_hit, back_hit);
          if (success) {
            if (front_det->getBananaCut()->isSatisfied(back_hit->Edep, front_hit->Edep)) {
              telescope_front_successes.emplace(front_hit);
              telescope_back_successes.emplace(back_hit);
              addTelescopeHit(front_hit, back_hit);
            }
          }
        }
      }
    }

    for (auto hit : telescope_front_successes) {
      telescope_front_candidates.erase(hit);
    }
    // what remains in the set here are untreated DSSSD hits
    for (auto hit : telescope_front_candidates) {
      treatDsssdHit(hit);
      addDssdHit(hit);
    }
  }
};

class GammaGatedProtons : public E21010GeneralAnalysis {
public:
  GammaGatedProtons(const shared_ptr<Setup>& setupSpec, const shared_ptr<Target>& target, TFile *output, double implantation_depth,
                    string twoPdaughter="20Ne",
                bool exclude_hpges=false, bool exclude_U5=false, bool include_dsd_rim=false,
                bool include_beta_region=false, bool include_spurious_zone=false,
                double Emin=0., double Emax=INFINITY)
          : E21010GeneralAnalysis(setupSpec, target, output, implantation_depth, twoPdaughter,
                                  exclude_hpges, exclude_U5, include_dsd_rim,
                                  include_beta_region, include_spurious_zone) {
    this->Emin = Emin;
    this->Emax = Emax;
  }

  void specificAnalysis() override {
    if (hits.empty()) return;

    bool success_any = false;
    bool good_gamma;
    for (auto &E : {Eg1, Eg2}) {
      good_gamma = GammaGate(E, Emin, Emax);
      if (good_gamma) success_any = true;
    }
    if (!success_any) return;

    unordered_set<Hit*> telescope_front_candidates;
    unordered_set<Hit*> telescope_back_candidates;
    for (auto &hit : hits) {
      auto det = hit.det;
      switch (det->getType()) {
        case DSSSD:
          if (det->hasPartner()) {
            telescope_front_candidates.emplace(&hit);
          } else { // the detector is U5
            treatDsssdHit(&hit);
            addDssdHit(&hit);
          }
          break;
        case Pad:
          telescope_back_candidates.emplace(&hit);
          break;
        default:
          // if case is 'HPGe', the scalar Eg1 and/or Eg2 already contains the gamma energy and the boolean 'g' is set to 'true'
          // do nothing
          break;
      }
    }

    unordered_set<Hit*> telescope_front_successes;
    unordered_set<Hit*> telescope_back_successes;
    for (auto front_hit : telescope_front_candidates) {
      if (telescope_front_successes.count(front_hit) > 0) continue;
      auto front_det = front_hit->det;
      for (auto back_hit : telescope_back_candidates) {
        if (telescope_back_successes.count(back_hit) > 0) continue;
        auto back_det = back_hit->det;
        if (front_det->getPartner() == back_det) {
          bool success = treatTelescopeHit(front_hit, back_hit);
          if (success) {
            if (front_det->getBananaCut()->isSatisfied(back_hit->Edep, front_hit->Edep)) {
              telescope_front_successes.emplace(front_hit);
              telescope_back_successes.emplace(back_hit);
              addTelescopeHit(front_hit, back_hit);
            }
          }
        }
      }
    }

    for (auto hit : telescope_front_successes) {
      telescope_front_candidates.erase(hit);
    }
    // what remains in the set here are untreated DSSSD hits
    for (auto hit : telescope_front_candidates) {
      treatDsssdHit(hit);
      addDssdHit(hit);
    }
  }
private:
  double Emin, Emax;
};

class TwoProton : public E21010GeneralAnalysis {
public:
  TwoProton(const shared_ptr<Setup>& setupSpec, const shared_ptr<Target>& target, TFile *output, double implantation_depth,
            string twoPdaughter="20Ne",
                    bool exclude_hpges=false, bool exclude_U5=false, bool include_dsd_rim=false,
                    bool include_beta_region=false, bool include_spurious_zone=false)
          : E21010GeneralAnalysis(setupSpec, target, output, implantation_depth, twoPdaughter,
                                  exclude_hpges, exclude_U5, include_dsd_rim,
                                  include_beta_region, include_spurious_zone) {
    unsigned int num_thetas = 181; // TODO: read number of files in folder dynamically
    theta = gsl_vector_calloc(num_thetas);
    omega = gsl_vector_calloc(num_thetas);
    string saPrefix = getProjectRoot() + "/data/oa_vs_sa/";
    string saSuffix = ".dat";
    string path = saPrefix + "theta" + saSuffix;
    FILE *input = fopen(path.c_str(), "r");
    int success = gsl_vector_fscanf(input, theta);
    if (success != 0) {
      cerr << "Could not read 2p theta distribution from file " << path << endl
           << "Aborting." << endl;
      abort();
    }
    fclose(input);
    path = saPrefix + "omega" + saSuffix;
    input = fopen(path.c_str(), "r");
    success = gsl_vector_fscanf(input, omega);
    if (success != 0) {
      cerr << "Could not read 2p omega distribution from file " << path << endl
           << "Aborting." << endl;
      abort();
    }
    fclose(input);

    interp.SetData(num_thetas, theta->data, omega->data);
  }

  void specificAnalysis() override {
    if (hits.empty()) return;

    int proton_events = 0;
    
    pair<vector<Hit*>, vector<Hit*>> telescope_candidates;
    for (auto &hit : hits) {
      auto det = hit.det;
      switch (det->getType()) {
        case DSSSD:
          if (det->hasPartner()) {
            telescope_candidates.first.emplace_back(&hit);
          }
          break;
        case Pad:
          if (det->hasPartner()) {
            telescope_candidates.second.emplace_back(&hit);
          }
          break;
        default:
          // if case is 'HPGe', the scalar Eg1 and/or Eg2 already contains the gamma energy and the boolean 'g' is set to 'true'
          // do nothing
          break;
      }
    }

    pair<vector<Hit*>, vector<Hit*>> telescope_successes;
    for (auto front_hit : telescope_candidates.first) {
      if (std::find(telescope_successes.first.begin(),
                    telescope_successes.first.end(), 
                    front_hit) != telescope_successes.first.end()) continue;
      auto front_det = front_hit->det;
      for (auto back_hit : telescope_candidates.second) {
        if (std::find(telescope_successes.second.begin(),
                      telescope_successes.second.end(),
                      back_hit) != telescope_successes.second.end()) continue;
        auto back_det = back_hit->det;
        if (front_det->getPartner() == back_det) {
          bool success = treatTelescopeHit(front_hit, back_hit);
          if (success) {
            if (front_det->getBananaCut()->isSatisfied(back_hit->Edep, front_hit->Edep)) {
              proton_events++;
              telescope_successes.first.emplace_back(front_hit);
              telescope_successes.second.emplace_back(back_hit);
            }
          }
        }
      }
    }

    unordered_set<Hit*> deltaE_contained;
    for (auto front_hit : telescope_candidates.first) {
      if (std::find(telescope_successes.first.begin(),
                    telescope_successes.first.end(),
                    front_hit) == telescope_successes.first.end()) {
        treatDsssdHit(front_hit);
        deltaE_contained.emplace(front_hit);
        proton_events++;
      }
    }

//    if (proton_events > 2) cout << proton_events << "\t" << endl;

    if (proton_events != 2) return;
    
    vector<Hit*> twoPHits;
    for (auto hit : telescope_successes.first) {
      twoPHits.emplace_back(hit);
    }
    for (auto hit : deltaE_contained) {
      twoPHits.emplace_back(hit);
    }



    double E1 = twoPHits[0]->E;
    double E2 = twoPHits[1]->E;
    Theta = twoPHits[0]->dir.Angle(twoPHits[1]->dir);
    Q2p = E1 + E2 + PROTON_MASS*(E1 + E2 + 2*sqrt(E1*E2)*cos(Theta))/twoPDaughter.getMass();
    Theta *= TMath::RadToDeg();
    Omega = interp.Eval(Theta);


    for (auto hit : twoPHits) {
      addTwoProtonHit(hit);
    }
  }

private:
  gsl_vector* theta;
  gsl_vector* omega;
  Interpolator interp = Interpolator(0, kAKIMA);
};

class Alphas : public E21010GeneralAnalysis {
public:
  Alphas(const shared_ptr<Setup>& setupSpec, const shared_ptr<Target>& target, TFile *output, double implantation_depth,
                          string twoPdaughter="20Ne",
                          bool exclude_hpges=false, bool exclude_U5=false, bool include_dsd_rim=false,
                          bool include_beta_region=false)
          : E21010GeneralAnalysis(setupSpec, target, output, implantation_depth, twoPdaughter,
                                  exclude_hpges, exclude_U5, include_dsd_rim,
                                  include_beta_region, true) {}

  void specificAnalysis() override {
    if (hits.empty()) return;
    
    for (auto &hit : hits) {
      auto det = hit.det;
      switch (det->getType()) {
        case DSSSD:
          if (det->getThickness() > 0.070) continue;
          for (auto &tab : det->getTelescopeTabulations()) {
            if (tab->getIon() != "p") continue;
            if (hit.Edep > 1.05*tab->getEnergyDepositionAtPunchThrough(hit.FI, hit.BI)) {
//            if ((det->getName() == "U1" || det->getName() == "U3") && hit.Edep > 2200 ||
//                (det->getName() == "U2" || det->getName() == "U6") && hit.Edep > 2500) {
              treatDsssdHit(&hit);
              addDssdHit(&hit);
            }
          }
          break;
        default:
          // if case is 'HPGe', the scalar Eg1 and/or Eg2 already contains the gamma energy and the boolean 'g' is set to 'true'
          // do nothing
          break;
      }
    }
  }
};

// parallel -u ./bp ../../analysis/b2p.cfg ::: $( seq 30 35 ; seq 40 40 ; seq 45 52 ; seq 55 55 ; seq 57 59 ; seq 61 61 ; seq 63 74 )
// parallel -u ./bp ../../analysis/b2p.cfg ::: $( seq 79 82 ; seq 84 97 ; seq 103 118 )
int main(int argc, char *argv[]) {
  prepareFileIO(getProjectRoot() + "/analysis/" + getBasename(argv[1]));
  auto setup = JSON::readSetupFromJSON(setup_path);
  auto target = make_shared<Target>(JSON::readTargetFromJSON(target_path));

  vector<string> input;
  int run;

  run = stoi(argv[2]);
  prepareAnalysis(run);
  findFilesMatchingWildcard(Form(input_path.c_str(), run), input);

  system(("mkdir -p " + output_dir).c_str());
  for (auto &in : input) {
    clock_t start = clock();

    SortedReader reader{*setup};
    reader.add(in);
    reader.setVerbose(true);

    string stem = getStem(in);
    TString outfile = (output_dir + "/" + stem + "lio.root").c_str();

    TFile output(outfile, "RECREATE");
    shared_ptr<E21010GeneralAnalysis> analysis;
    if (specificAnalysis == "DeltaEContained") {
      analysis =
              make_shared<DeltaEContainedAnalysis>(setup, target, &output, implantation_depth, twoPDaughter,
                                                   exclude_hpges, exclude_U5, include_dsd_rim,
                                                   include_beta_region, include_spurious_zone);
    } else if (specificAnalysis == "BananaExplorative") {
      analysis =
              make_shared<BananaExplorativeAnalysis>(setup, target, &output, implantation_depth, twoPDaughter,
                                                     exclude_hpges, exclude_U5, include_dsd_rim,
                                                     include_beta_region, include_spurious_zone);
    } else if (specificAnalysis == "Final") {
      analysis =
              make_shared<FinalAnalysis>(setup, target, &output, implantation_depth, twoPDaughter,
                                         exclude_hpges, exclude_U5, include_dsd_rim,
                                         include_beta_region, include_spurious_zone);
    } else if (specificAnalysis == "GammaGatedProtons") {
      analysis =
              make_shared<GammaGatedProtons>(setup, target, &output, implantation_depth, twoPDaughter,
                                             exclude_hpges, exclude_U5, include_dsd_rim,
                                             include_beta_region, include_spurious_zone,
                                             Egmin, Egmax);
    } else if (specificAnalysis == "TwoProton") {
      analysis =
              make_shared<TwoProton>(setup, target, &output, implantation_depth, twoPDaughter,
                                     exclude_hpges, exclude_U5, include_dsd_rim,
                                     include_beta_region, include_spurious_zone);
    } else if (specificAnalysis == "Alphas") {
      analysis =
              make_shared<Alphas>(setup, target, &output, implantation_depth, twoPDaughter,
                                  exclude_hpges, exclude_U5, include_dsd_rim,
                                  include_beta_region);
    } else {
      cerr << "Unknown analysis type. Aborting!" << endl;
      abort();
    }

    printConfig();

    cout << "Reading from: " << in << endl;
    cout << "Printing to:  " << outfile << endl;

    reader.attach(analysis);
    reader.run();

    clock_t stop = clock();
    double elapsed = (double) (stop - start) / CLOCKS_PER_SEC;
    printf("\nFile: %s.\tTime elapsed: %.5f\n", (stem + "lio.root").c_str(), elapsed);
  }

  return 0;
}