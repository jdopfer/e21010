#ifndef IS507DETECTOR_H
#define IS507DETECTOR_H

#include "gCut.h"

#include <string>
#include <utility>

#include <ausa/setup/Setup.h>
#include <ausa/setup/SquareDSSD.h>
#include <ausa/setup/PadDetector.h>

#include <telescope/TelescopeTabulation.h>

using namespace std;
using namespace Telescope;

namespace IS507 {
  enum Type {
    NoType = 0,
    DSSSD,
    Pad,
    Clover
  };

  enum Calibration {
    NoCal = 0,
    Proton,
    Alpha,
    Gamma
  };

  class IS507Detector {
  public:
    IS507Detector() = default;
    IS507Detector(unsigned short id, string name, Type type, Calibration cal, const shared_ptr<AUSA::Setup>& setup)
    : id(id), name(std::move(name)), type(type), cal(cal), setup(setup) {
      switch (type) {
        case DSSSD:
          frontDeadLayer = AUSA::getFrontDeadLayer(*setup->getDSSD(this->name));
          thickness = AUSA::getThickness(*setup->getDSSD(this->name));
          backDeadLayer = AUSA::getBackDeadLayer(*setup->getDSSD(this->name));
          break;
        case Pad:
          frontDeadLayer = AUSA::getFrontDeadLayer(*setup->getSingleSided(this->name));
          thickness = AUSA::getThickness(*setup->getSingleSided(this->name));
          backDeadLayer = AUSA::getBackDeadLayer(*setup->getSingleSided(this->name));
          break;
        case Clover:
          // do nothing
          break;
        case NoType:
          cerr << "Warning: Detector " << this->name << " instantiated as 'NoType'." << endl;
          break;
      }
    };
    IS507Detector(unsigned short id, string name, Type type, Calibration cal, const shared_ptr<AUSA::Setup>& setup,
                  double betaCutoff) : IS507Detector(id, std::move(name), type, cal, setup) {
      this->betaCutoff = betaCutoff;
    };

    unsigned short getId() const { return id; };
    string getName() const { return name; };
    Type getType() const { return type; };
    Calibration getCalibration() const { return cal; };
    IS507Detector* getPartner() const { return partner; };
    double getFrontDeadLayer() const { return frontDeadLayer; }
    double getThickness() const { return thickness; }
    double getBackDeadLayer() const { return backDeadLayer; }
    double getBetaCutoff() const { return betaCutoff; }
    gCut* getBananaCut() const { return banana; }
    vector<TelescopeTabulation*> getTelescopeTabulations() const { return tTabulations; }

    void setId(unsigned short id) { this->id = id; };
    void setName(string name) { this->name = std::move(name); };
    void setType(Type type) { this->type = type; };
    void setCalibration(Calibration cal) { this->cal = cal; };
    void setPartner(IS507Detector* partner) { this->partner = partner; this->withPartner = true; };
    void setFrontDeadLayer(double fdl) { this->frontDeadLayer = fdl; }
    void setThickness(double t) { this->thickness = t; }
    void setBackDeadLayer(double bdl) { this->backDeadLayer = bdl; }
    void setBetaCutoff(double cutoff) { this->betaCutoff = cutoff; }
    void setBananaCut(gCut* cut) { this->banana = cut; }
    void addTelescopeTabulation(TelescopeTabulation* tT) { this->tTabulations.emplace_back(tT); }

    bool hasPartner() const { return withPartner; };

  private:
    unsigned short id{};
    string name;
    Type type{};
    Calibration cal{};
    IS507Detector* partner{};
    bool withPartner = false;

    shared_ptr<AUSA::Setup> setup;
    double frontDeadLayer{}, thickness{}, backDeadLayer{}; // = 0.0 by default
    double betaCutoff{}; // = 0.0 by default

    gCut* banana;
    vector<TelescopeTabulation*> tTabulations;
  };

  void makePartners(IS507Detector* det1, IS507Detector* det2) {
    det1->setPartner(det2);
    det2->setPartner(det1);
  }

}

#endif //IS507DETECTOR_H
