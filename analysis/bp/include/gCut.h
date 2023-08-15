#ifndef IS507_V3_GCUT_H
#define IS507_V3_GCUT_H

#include <iostream>
#include <string>
#include <TGraph.h>
#include <TROOT.h>
#include <TFile.h>
#include "Cut.h"

using namespace std;

class gCut: public Cut {
public:
  gCut(const string &filename, const string &cutname, cut_logic cutLogic = include_region) : Cut(cutLogic) {
    TFile f(filename.c_str());
    graph = (TGraph*) gROOT->FindObject(cutname.c_str());
  }

  bool isInside(double x, double y) const override {
    return graph->IsInside(x, y);
  }

  void print() {
    for (int i = 0; i < graph->GetN(); i++) {
      cout << "(" << graph->GetPointX(i) << ", " << graph->GetPointY(i) << ")" << endl;
    }
  }

private:
  TGraph* graph;
};

#endif //IS507_V3_GCUT_H
