//
// erik 2023-08-31
//
#include <TFile.h>
#include <TTree.h>
#include <ausa/util/DynamicBranchVector.h>
#include <memory>
#include <iostream>
#include <gsl/gsl_matrix.h>
#include "projectutil.h"
#include <iomanip>

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

int main(int argc, char **argv) {
  unsigned int num_thetas = 181; // TODO: read number of files in folder dynamically
  gsl_vector* thetas = gsl_vector_calloc(num_thetas);
  gsl_vector* omegas = gsl_vector_calloc(num_thetas);

  for (int i = 0; i < num_thetas; i++) {
    gsl_vector_set(thetas, i, i);

    std::ostringstream file_suffix;
    file_suffix << std::setw(3) << std::setfill('0') << i;
    std::unique_ptr<TFile> sim(TFile::Open((getProjectRoot() + "/data/sorted_sim_oa/" + file_suffix.str() + "m.root").c_str()));
    auto t = sim->Get<TTree>("a101");
    UInt_t N = t->GetEntries();
    auto res = (double) t->GetEntries("mul > 1");
    res /= (double) N;
    gsl_vector_set(omegas, i, res);
    cout << gsl_vector_get(thetas, i) << ", " << gsl_vector_get(omegas, i) << endl;
    sim->Close();
  }

  system(("mkdir -p " + getProjectRoot() + "/data/oa_vs_sa").c_str());
  FILE *output;
  output = fopen((getProjectRoot() + "/data/oa_vs_sa/theta.dat").c_str(), "w");
  gsl_vector_fprintf(output, thetas, "%g");
  fclose(output);
  output = fopen((getProjectRoot() + "/data/oa_vs_sa/omega.dat").c_str(), "w");
  gsl_vector_fprintf(output, omegas, "%g");
  fclose(output);

  return 0;
}