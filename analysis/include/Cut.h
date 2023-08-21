#ifndef E21010_V3_CUT_H
#define E21010_V3_CUT_H

#include <iostream>
#include <string>
#include <TROOT.h>

using namespace std;

enum cut_logic {
  include_region,
  exclude_region
};

class Cut {
public:
  explicit Cut(cut_logic cutLogic) : cutLogic(cutLogic) {};
  virtual bool isInside(double x, double y) const = 0;
  bool isSatisfied(double  x, double y) const {
    return cutLogic == include_region ? isInside(x, y) : !isInside(x, y);
  }

protected:
  cut_logic cutLogic;
};

#endif //E21010_V3_CUT_H
