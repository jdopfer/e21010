#include "common.h"

using namespace std;
using namespace AUSA;
using namespace ROOT::Math;
using namespace AUSA::Sort;
using namespace AUSA::EnergyLoss;
using namespace libconfig;
using namespace EUtil;

int main(int argc, char* argv[]) {
  init(argc, argv);
  Setting &root = cfg.getRoot();
  const Setting &ions = root["ions"];

  EnergyLossRangeInverter *detectorCalc;
  vector<shared_ptr<EnergyLossRangeInverter>> targetCalcs;
  double threshold;
  for (int j = 0; j < rangeCalcs.size(); j++) {
    detectorCalc = rangeCalcs[j].first.get();
    targetCalcs = rangeCalcs[j].second;

    vector<double> fEdep_max, Eparticle_lower, Eparticle_upper, thetas, fEdep_lower;
    double theta, Etmp;
    for (auto &pos: pixel_positions) {
      // angle of incidence between particle and detector surface
      theta = (pos - origin).Angle(-det->getNormal());
      thetas.emplace_back(theta);

      // threshold deposited energy (observed without applying energy corrections) in front detector just before particle punches through
      fEdep_max.emplace_back(detectorCalc->getTotalEnergyCorrection(0.0, ft / cos(theta)));

      Etmp = detectorCalc->getTotalEnergyCorrection(0.0, (ft + fdlF) / cos(theta)); // ft = thickness of active volume of front detector, fdlF = thickness of deadlayer on frontside of front detector
      for (auto &intersection: target->getIntersections(pos, origin)) {
        auto &calc = targetCalcs[intersection.index];
        Etmp += calc->getTotalEnergyCorrection(Etmp, intersection.transversed); // iterate backwards through thicknesses of target's layers
      }
      // actual original particle energy corresponding to fEdep_max -- commences the dead area of the particle energy spectrum at the given angle of incidence in the given telescope
      Eparticle_lower.emplace_back(Etmp);

      // find ADC threshold of telescope's Pad detector
      threshold = 200.;
      for (int k = 0; k < root.lookup("back_detector_thresholds").lookup("names").getLength(); k++) {
        if (bptr->getName() == root.lookup("back_detector_thresholds").lookup("names")[k].c_str()) {
          threshold = root.lookup("back_detector_thresholds").lookup("thresholds")[k];
          break;
        }
      }

      Etmp = threshold;
      Etmp += detectorCalc->getTotalEnergyCorrection(Etmp, (bdlF + fdlB + ft + fdlF) / cos(theta)); // bdlF = thickness of deadlayer on frontside of back detector, fdlF = thickness of deadlayer on backside of front detector
      for (auto &intersection: target->getIntersections(pos, origin)) {
        auto &calc = targetCalcs[intersection.index];
        Etmp += calc->getTotalEnergyCorrection(Etmp, intersection.transversed);
      }
      // actual original particle energy which can barely punch through all the obstacles between the target and the back detector's ADC threshold -- concludes the dead area of the particle energy spectrum at the given angle of incidence in the given telescope
      Eparticle_upper.emplace_back(Etmp);

      double Epu = Etmp;
      for (auto &intersection: target->getIntersections(origin, pos)) {
        auto &calc = targetCalcs[intersection.index];
        Etmp -= calc->getTotalEnergyLoss(Etmp, intersection.transversed);
      }
      Etmp -= detectorCalc->getTotalEnergyLoss(Etmp, (ft + fdlF) / cos(theta));
      Etmp = Epu - Etmp;

      // deposited energy in front detector (always less than fEdep_max) which corresponds to the conclusion of the dead area of the telescope
      // in the ranges [fEdep_lower, fEdep_max] one cannot generally determine if the particle in question is above or below punch through
      fEdep_lower.emplace_back(Etmp);
    }

    cout << "# ION: " << ions[j].c_str() << endl;
    print_telescope(fptr->getName(), NAN, fdlF, ft, fdlB,
                    bptr->getName(), threshold, bdlF, bt);
    cout << "# v\tEdep_max\tEparticle_da_lower\tEparticle_da_upper\tEdep_da_lower" << endl;
    for (int k = 0; k < size(thetas); k++) {
      cout << thetas[k] << "\t" << fEdep_max[k] << "\t" << Eparticle_lower[k] << "\t" << Eparticle_upper[k] << "\t" << fEdep_lower[k] << endl;
    }
    cout << endl << endl << endl << endl;
  }
  return 0;
}