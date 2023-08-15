#include "common.h"

using namespace std;
using namespace AUSA;
using namespace ROOT::Math;
using namespace AUSA::Sort;
using namespace AUSA::EnergyLoss;
using namespace libconfig;
using namespace EUtil;

int main(int argc, char *argv[]) {
  init(argc, argv);
  Setting &root = cfg.getRoot();
  const Setting &ions = root["ions"];

  EnergyLossRangeInverter *detectorCalc;
  vector<shared_ptr<EnergyLossRangeInverter>> targetCalcs;
  double fThreshold, bThreshold;
  for (int j = 0; j < rangeCalcs.size(); j++) {
    detectorCalc = rangeCalcs[j].first.get();
    targetCalcs = rangeCalcs[j].second;


    vector<double> fEdep_max, fEpThresh, bEpThresh, thetas;
    for (auto &pos: pixel_positions) {
      // angle of incidence between particle and detector surface
      double theta = (pos - origin).Angle(-det->getNormal());
      thetas.emplace_back(theta);

      double Etmp = detectorCalc->getTotalEnergyCorrection(0.0, (ft + fdlF) / cos(theta));
      for (auto &intersection: target->getIntersections(pos, origin)) {
        auto &calc = targetCalcs[intersection.index];
        Etmp += calc->getTotalEnergyCorrection(Etmp, intersection.transversed);
      }
      // actual original particle energy at punch-through energy
      fEdep_max.emplace_back(Etmp);

      // find ADC threshold of telescope's front detector
      fThreshold = 100.;
      for (int k = 0; k < root.lookup("front_detector_thresholds").lookup("names").getLength(); k++) {
        if (fptr->getName() == root.lookup("front_detector_thresholds").lookup("names")[k].c_str()) {
          fThreshold = root.lookup("front_detector_thresholds").lookup("thresholds")[k];
          break;
        }
      }
      Etmp = fThreshold;
      Etmp += detectorCalc->getTotalEnergyCorrection(Etmp, fdlF / cos(theta));
      for (auto &intersection: target->getIntersections(pos, origin)) {
        auto &calc = targetCalcs[intersection.index];
        Etmp += calc->getTotalEnergyCorrection(Etmp, intersection.transversed);
      }
      // actual original particle energy which can barely punch through all the obstacles between the target and the front detector's ADC threshold
      fEpThresh.emplace_back(Etmp);

      // find ADC threshold of telescope's back detector
      bThreshold = 100.;
      for (int k = 0; k < root.lookup("back_detector_thresholds").lookup("names").getLength(); k++) {
        if (bptr->getName() == root.lookup("back_detector_thresholds").lookup("names")[k].c_str()) {
          bThreshold = root.lookup("back_detector_thresholds").lookup("thresholds")[k];
          break;
        }
      }
      Etmp = bThreshold;
      Etmp += detectorCalc->getTotalEnergyCorrection(Etmp, (bdlF + fdlB + ft + fdlF) / cos(theta));
      for (auto &intersection: target->getIntersections(pos, origin)) {
        auto &calc = targetCalcs[intersection.index];
        Etmp += calc->getTotalEnergyCorrection(Etmp, intersection.transversed);
      }
      // actual original particle energy which can barely punch through all the obstacles between the target and the back detector's ADC threshold
      bEpThresh.emplace_back(Etmp);
    }

    double Epmin = root.lookup("energy_ranges").lookup("lower")[j];
    double Epmax = root.lookup("energy_ranges").lookup("upper")[j];
    double Epstep = root.lookup("energy_ranges").lookup("step")[j];

    double fEdep, bEdep;
    cout << "# ION: " << ions[j].c_str() << "\t ENERGY=[" << Epmin << ", " << Epmin + Epstep << ", ... , " << Epmax << "] keV" << endl;
    print_telescope(fptr->getName(), fThreshold, fdlF, ft, fdlB,
                    bptr->getName(), bThreshold, bdlF, bt);
    cout << "# v\tEp\tfEdep\tbEdep" << endl;
    for (int k = 0; k < thetas.size(); k++) {
      double theta = thetas[k];
      auto &pixel = pixel_positions[k];
      for (int l = 0; l < int((Epmax - Epmin) / Epstep); l++) {
        double Ep = Epmin + l * Epstep;

        if (Ep <= fEpThresh[k]) {
          fEdep = 0.;
        } else if (Ep <= fEdep_max[k]) {
          fEdep = Ep;
          for (auto &intersection: target->getIntersections(origin, pixel)) {
            auto &calc = targetCalcs[intersection.index];
            fEdep -= calc->getTotalEnergyLoss(fEdep, intersection.transversed);
          }
          fEdep -= detectorCalc->getTotalEnergyLoss(fEdep, fdlF / cos(theta));
        } else {
          double Epl = Ep;
          for (auto &intersection: target->getIntersections(origin, pixel)) {
            auto &calc = targetCalcs[intersection.index];
            Epl -= calc->getTotalEnergyLoss(Epl, intersection.transversed);
          }
          Epl -= detectorCalc->getTotalEnergyLoss(Epl, fdlF / cos(theta));

          fEdep = detectorCalc->getTotalEnergyLoss(Epl, ft / cos(theta));
        }

        if (Ep <= bEpThresh[k]) {
          bEdep = 0.;
        } else {
          bEdep = Ep;
          for (auto &intersection: target->getIntersections(origin, pixel)) {
            auto &calc = targetCalcs[intersection.index];
            bEdep -= calc->getTotalEnergyLoss(bEdep, intersection.transversed);
          }
          bEdep -= detectorCalc->getTotalEnergyLoss(bEdep, (fdlF + ft + fdlB + bdlF) / cos(theta));
        }

        cout << thetas[k] << "\t" << Ep << "\t" << fEdep << "\t" << bEdep << endl;

      }
    }
    cout << endl << endl << endl << endl;
  }
  return 0;
}