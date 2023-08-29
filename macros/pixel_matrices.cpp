//
// Created by erik on 8/9/23.
//
#include <TFile.h>
#include <TTree.h>
#include <ausa/util/DynamicBranchVector.h>
#include <memory>
#include <iostream>
#include <gsl/gsl_matrix.h>
#include <ausa/json/IO.h>
#include <fstream>
#include "projectutil.h"

using namespace std;
using namespace AUSA;
using namespace EUtil;

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

void print_matrix(const string& filename, gsl_matrix* matrix) {
  ofstream out(filename);
  cout.rdbuf(out.rdbuf());
  cout << "# ";
  for (int j = 0; j < matrix->size2; j++) {
    cout << j + 1 << "\t";
  }
  cout << endl;
  for (size_t i = 0; i < matrix->size1; i++) {
    for (size_t j = 0; j < matrix->size2; j++) {
      cout << gsl_matrix_get(matrix, i, j);
      if (j + 1 != matrix->size2) {
        cout << "\t";
      } else {
        cout << "\t# " << i+1 << endl;
      }
    }
  }
}

int main(int argc, char **argv) {
  std::unique_ptr<TFile> unpacked( TFile::Open(argv[1]) );
  auto t1 = unpacked->Get<TTree>("a");

  auto setup = JSON::readSetupFromJSON(argv[2]);

  gsl_matrix* x  = gsl_matrix_calloc(16, 16);
  gsl_matrix* y  = gsl_matrix_calloc(16, 16);
  gsl_matrix* z  = gsl_matrix_calloc(16, 16);
  gsl_matrix* r2 = gsl_matrix_calloc(16, 16);
  gsl_matrix* N  = gsl_matrix_calloc(16, 16);

  typedef struct detector {
    string name;
    UInt_t id;
    string cut;
  } detector;

  const vector<detector> detectors = {
      {"U1", 0, "bEdep > 1000"}, {"U2", 1, "bEdep > 1000"}, {"U3", 2, "bEdep > 1000"},
      {"U4", 3, "E > 800"}, {"U5", 4, "E > 2000"}, {"U6", 5, "bEdep > 1000"},
  };

  shared_ptr<DoubleSidedDetector> detp;
  TVector3 pos;
  string path = getProjectRoot() + "/data/pixel_matrices";
  string prefix, suffix;
  system(("mkdir -p " + path).c_str());
  for (const auto &det : detectors) {
    detp = setup->getDSSD(det.name);
    for (int i = 0; i < 16; i++) {
      for (int j = 0; j < 16; j++) {
        pos = detp->getPixelPosition(i+1, j+1);
        gsl_matrix_set(x, i, j, pos.X());
        gsl_matrix_set(y, i, j, pos.Y());
        gsl_matrix_set(z, i, j, pos.Z());
        gsl_matrix_set(r2, i, j, pos.Mag2());
        gsl_matrix_set(N, i, j, t1->GetEntries(("FI == " + to_string(i+1) +
                                            " && BI == " + to_string(j+1) +
                                            " && id == " + to_string(det.id) +
                                            " && "       + det.cut).c_str()));
      }
    }
    prefix = path + "/" + det.name;
    suffix = ".dat";
    print_matrix(prefix + "x" + suffix,  x);
    print_matrix(prefix + "y" + suffix,  y);
    print_matrix(prefix + "z" + suffix,  z);
    print_matrix(prefix + "r2" + suffix, r2);
    print_matrix(prefix + "N" + suffix,  N);
  }

  return 0;
}