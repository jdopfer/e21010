//
// Created by erik on 8/9/23.
//
#include <TFile.h>
#include <TTree.h>
#include <ausa/util/DynamicBranchVector.h>
#include <memory>
#include <iostream>
#include <gsl/gsl_matrix.h>
#include "projectutil.h"

using namespace std;
using namespace AUSA;
using namespace EUtil;

/*
 * In ROOT forum threads, the attitude from ~2007-2017 has been that "merging trees horizontally" is not supported and
 * should not be supported. In a post from 2020 or 2021, which I, for the life of me, cannot find again, Rene Brun or
 * similar wrote that "merging trees horizontally" is not supported *yet*.
 * So keep an eye on the release notes of ROOT versions > 6.24.
 * Until then, this boilerplate code does the job, but the branches have to be hardcoded.
 */

UInt_t mul{};
unique_ptr<DynamicBranchVector<double>> E;
unique_ptr<DynamicBranchVector<unsigned short>> FI, BI, id;

void clear() {
  mul = 0;
  AUSA::clear(
      *E,
      *FI, *BI,
      *id
  );
}

int main(int argc, char **argv) {
  std::unique_ptr<TFile> unpacked( TFile::Open(argv[1]) );
  auto t1 = unpacked->Get<TTree>("a101");

  Long64_t N = t1->GetEntries();
  gsl_matrix* U1 = gsl_matrix_alloc(16, 16);
  gsl_matrix* U2 = gsl_matrix_alloc(16, 16);
  gsl_matrix* U3 = gsl_matrix_alloc(16, 16);
  gsl_matrix* U4 = gsl_matrix_alloc(16, 16);
  gsl_matrix* U5 = gsl_matrix_alloc(16, 16);
  gsl_matrix* U6 = gsl_matrix_alloc(16, 16);

  typedef struct detector {
    string name;
    UInt_t id;
    gsl_matrix* matrix;
  } detector;

  const vector<detector> detectors = {
      {"U1", 0, U1}, {"U2", 1, U2}, {"U3", 2, U3},
      {"U4", 3, U4}, {"U5", 4, U5}, {"U6", 5, U6},
  };

  double res;
  FILE *output;
  for (const auto &det : detectors) {
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        res = (double) t1->GetEntries(("FI == " + to_string(i+1) +
                                       " && BI == " + to_string(j+1) +
                                       " && id == " + to_string(det.id)).c_str());
        res /= (double) N;
        gsl_matrix_set(det.matrix, i, j, res);
      }
    }
    output = fopen((getProjectRoot() + "/data/solid_angle/" + det.name + "_solid_angle.dat").c_str(), "w");
    gsl_matrix_fprintf(output, det.matrix, "%g");
    fclose(output);
  }

  return 0;
}