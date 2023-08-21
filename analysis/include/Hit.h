#ifndef HIT_H
#define HIT_H

#include <TVector3.h>
#include <TLorentzVector.h>
#include "E21010Detector.h"

using namespace IS507;

struct Hit {
  E21010Detector* det;

  unsigned short id;

  TVector3 dir, pos;
  double theta, phi, angle;

  unsigned short FI, BI;
  double FE, BE;
  double FT, BT;
  double Edep; // average of FE and BE

  double E, Ea, Eg;
};

#endif //HIT_H