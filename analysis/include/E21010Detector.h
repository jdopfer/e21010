#ifndef E21010DETECTOR_H
#define E21010DETECTOR_H

#include "gCut.h"

#include <string>
#include <utility>

#include <gsl/gsl_matrix.h>

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
    HPGe
  };

  enum Calibration {
    NoCal = 0,
    Proton,
    Alpha,
    Gamma
  };

  class E21010Detector {
  public:
    E21010Detector() = default;

    E21010Detector(unsigned short id, const string& name, Type type, Calibration cal, const shared_ptr<AUSA::Setup>& setup)
    : id(id), name(name), type(type), cal(cal), setup(setup) {
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
        case HPGe:
          // do nothing
          break;
        case NoType:
          cerr << "Warning: Detector " << this->name << " instantiated as 'NoType'." << endl;
          break;
      }
    };

    E21010Detector(unsigned short id, const string& name, Type type, Calibration cal, const shared_ptr<AUSA::Setup>& setup,
                   double betaCutoff) : E21010Detector(id, name, type, cal, setup) {
      setBetaCutoff(betaCutoff);
    };

    E21010Detector(unsigned short id, const string& name, Type type, Calibration cal, const shared_ptr<AUSA::Setup>& setup,
                   const string& solidAngleMatrixPath) : E21010Detector(id, name, type, cal, setup) {
      setSolidAngleMatrixFromFile(solidAngleMatrixPath);
    };

    E21010Detector(unsigned short id, const string& name, Type type, Calibration cal, const shared_ptr<AUSA::Setup>& setup,
                   double betaCutoff, const string& solidAngleMatrixPath)
                   : E21010Detector(id, name, type, cal, setup) {
      setBetaCutoff(betaCutoff);
      setSolidAngleMatrixFromFile(solidAngleMatrixPath);
    };


    unsigned short getId() const { return id; };
    string getName() const { return name; };
    Type getType() const { return type; };
    Calibration getCalibration() const { return cal; };
    E21010Detector* getPartner() const { return partner; };
    double getFrontDeadLayer() const { return frontDeadLayer; }
    double getThickness() const { return thickness; }
    double getBackDeadLayer() const { return backDeadLayer; }
    double getBetaCutoff() const { return betaCutoff; }
    gCut* getBananaCut() const { return banana; }
    vector<TelescopeTabulation*> getTelescopeTabulations() const { return tTabulations; }
    gsl_matrix* getSolidAngleMatrix() const { return solidAngleMatrix; }

    void setId(unsigned short id) { this->id = id; };
    void setName(string name) { this->name = std::move(name); };
    void setType(Type type) { this->type = type; };
    void setCalibration(Calibration cal) { this->cal = cal; };
    void setPartner(E21010Detector* partner) { this->partner = partner; this->withPartner = true; };
    void setFrontDeadLayer(double fdl) { this->frontDeadLayer = fdl; }
    void setThickness(double t) { this->thickness = t; }
    void setBackDeadLayer(double bdl) { this->backDeadLayer = bdl; }
    void setBetaCutoff(double cutoff) { this->betaCutoff = cutoff; }
    void setBananaCut(gCut* cut) { this->banana = cut; }
    void addTelescopeTabulation(TelescopeTabulation* tT) { this->tTabulations.emplace_back(tT); }
    void setSolidAngleMatrixFromFile(const string& solidAngleMatrixPath) {
      UInt_t rows, cols;
      if (type == DSSSD) {
        rows = setup->getDSSD(name)->frontStripCount();
        cols = setup->getDSSD(name)->backStripCount();
      } else if (type != NoType) {
        rows = setup->getSingleSided(name)->segmentCount();
        cols = 1;
      } else {
        cerr << "Detector " << name << ": instantiated as 'NoType', do not know how to interpret "
             << "solid angle matrix from file " << solidAngleMatrixPath << endl
             << "Aborting." << endl;
        abort();
      }
      solidAngleMatrix  = gsl_matrix_calloc(rows, cols);
      FILE *input = fopen(solidAngleMatrixPath.c_str(), "r");
      int success = gsl_matrix_fscanf(input, solidAngleMatrix);
      if (success != 0) {
        cerr << "Detector " << name << ": Could not read solid angle matrix from file " << solidAngleMatrixPath << endl
             << "Aborting." << endl;
        abort();
      }
      fclose(input);
    }

    bool hasPartner() const { return withPartner; };

  private:
    unsigned short id{};
    string name;
    Type type{};
    Calibration cal{};
    E21010Detector* partner{};
    bool withPartner = false;

    shared_ptr<AUSA::Setup> setup;
    double frontDeadLayer{}, thickness{}, backDeadLayer{}; // = 0.0 by default
    double betaCutoff{}; // = 0.0 by default

    gCut* banana;
    vector<TelescopeTabulation*> tTabulations;
    gsl_matrix *solidAngleMatrix;
  };

  void makePartners(E21010Detector* det1, E21010Detector* det2) {
    det1->setPartner(det2);
    det2->setPartner(det1);
  }

}

#endif //E21010DETECTOR_H
