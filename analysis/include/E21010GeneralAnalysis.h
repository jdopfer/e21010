#ifndef E21010GENERALANALYSIS_H
#define E21010GENERALANALYSIS_H

#include <libconfig.h++>
#include "projectutil.h"
#include "Hit.h"
#include "E21010Detector.h"
#include "analysisConfig.h"
#include "E21010GeneralAnalysis.h"

#include <TROOT.h>

#include <ausa/json/IO.h>
#include <ausa/setup/Target.h>
#include <ausa/eloss/Default.h>
#include <ausa/util/FileUtil.h>
#include <ausa/sort/SortedReader.h>
#include <ausa/util/DynamicBranchVector.h>
#include <ausa/output/OutputConvenience.h>
#include <ausa/sort/analyzer/AbstractSortedAnalyzer.h>
#include <telescope/TelescopeTabulation.h>
#include <ausa/constants/Mass.h>
#include <ausa/eloss/Ion.h>

#define NAN_UINT 100
#define NAN_TVECTOR3 TVector3(NAN, NAN, NAN)

using namespace std;
using namespace libconfig;
using namespace EUtil;
using namespace IS507;

using namespace AUSA;
using namespace AUSA::Sort;
using namespace AUSA::EnergyLoss;
using namespace Telescope;
using namespace AUSA::Constants;

class E21010GeneralAnalysis : public AbstractSortedAnalyzer {
public:
  E21010GeneralAnalysis(const shared_ptr<Setup> &setupSpec, const shared_ptr<Target> &target, TFile *output,
                        double implantation_depth, string twoPdaughter="20Ne",
                        bool exclude_hpges = false, bool exclude_U5 = false, bool include_dsd_rim = false,
                        bool include_beta_region = false, bool include_spurious_zone=false)
      : setupSpec(setupSpec), target(target), implantation_depth(implantation_depth),
        exclude_hpges(exclude_hpges), exclude_U5(exclude_U5), include_dsd_rim(include_dsd_rim),
        include_beta_region(include_beta_region), include_spurious_zone(include_spurious_zone), twoPD_string(twoPdaughter) {

    origin = target->getCenter() - (implantation_depth - target->getThickness()/2.)*target->getNormal();

    string samPrefix = getProjectRoot() + "/data/solid_angle/";
    string samSuffix = "_solid_angle.dat";
    auto U1 = new E21010Detector(0, "U1", DSSSD, Alpha, setupSpec,
                                 350., samPrefix + "U1" + samSuffix);
    auto U2 = new E21010Detector(1, "U2", DSSSD, Alpha, setupSpec,
                                 350., samPrefix + "U2" + samSuffix);
    auto U3 = new E21010Detector(2, "U3", DSSSD, Alpha, setupSpec,
                                 350., samPrefix + "U3" + samSuffix);
    auto U4 = new E21010Detector(3, "U4", DSSSD, Alpha, setupSpec, 800.);
    auto U5 = new E21010Detector(4, "U5", DSSSD, Alpha, setupSpec, 1700.);
    auto U6 = new E21010Detector(5, "U6", DSSSD, Alpha, setupSpec, 350.);
    auto P1 = new E21010Detector(6, "P1", Pad, Alpha, setupSpec);
    auto P2 = new E21010Detector(7, "P2", Pad, Alpha, setupSpec);
    auto P3 = new E21010Detector(8, "P3", Pad, Alpha, setupSpec);
    auto P4 = new E21010Detector(9, "P4", Pad, Alpha, setupSpec);
    auto P5 = new E21010Detector(10, "P5", Pad, Alpha, setupSpec); // dead!
    auto P6 = new E21010Detector(11, "P6", Pad, Alpha, setupSpec);
    auto G1 = new E21010Detector(12, "G1", HPGe, Gamma, setupSpec);
    auto G2 = new E21010Detector(13, "G2", HPGe, Gamma, setupSpec);

    makePartners(U1, P1);
    makePartners(U2, P2);
    makePartners(U3, P3);
    //makePartners(U4, P4);
    makePartners(U6, P6);
    U1->setBananaCut(new gCut(getProjectRoot() + "/telescope/gcuts.root", "banana0", include_region));
    U2->setBananaCut(new gCut(getProjectRoot() + "/telescope/gcuts.root", "banana1", include_region));
    U3->setBananaCut(new gCut(getProjectRoot() + "/telescope/gcuts.root", "banana2", include_region));
    //U4->setBananaCut(new gCut(getProjectRoot() + "/telescope/gcuts.root", "banana3", include_region));
    U6->setBananaCut(new gCut(getProjectRoot() + "/telescope/gcuts.root", "banana5", include_region));

    pU1P1 = new TelescopeTabulation(setupSpec, target, "U1", "P1", "p");
    pU2P2 = new TelescopeTabulation(setupSpec, target, "U2", "P2", "p");
    pU3P3 = new TelescopeTabulation(setupSpec, target, "U3", "P3", "p");
    //pU4P4 = new TelescopeTabulation(setupSpec, target, "U4", "P4", "p");
    pU6P6 = new TelescopeTabulation(setupSpec, target, "U6", "P6", "p");
    aU1P1 = new TelescopeTabulation(setupSpec, target, "U1", "P1", "a");
    aU2P2 = new TelescopeTabulation(setupSpec, target, "U2", "P2", "a");
    aU3P3 = new TelescopeTabulation(setupSpec, target, "U3", "P3", "a");
    //aU4P4 = new TelescopeTabulation(setupSpec, target, "U4", "P4", "a");
    aU6P6 = new TelescopeTabulation(setupSpec, target, "U6", "P6", "a");
    for (auto &tT : {pU1P1, pU2P2, pU3P3, 
                     //pU4P4, 
                     pU6P6, 
                     aU1P1, aU2P2, aU3P3, 
                     //aU4P4, 
                     aU6P6}) tT->setImplantationDepth(implantation_depth);
    U1->addTelescopeTabulation(pU1P1); U1->addTelescopeTabulation(aU1P1);
    U2->addTelescopeTabulation(pU2P2); U2->addTelescopeTabulation(aU2P2);
    U3->addTelescopeTabulation(pU3P3); U3->addTelescopeTabulation(aU3P3);
    //U4->addTelescopeTabulation(pU4P4); U4->addTelescopeTabulation(aU4P4);
    U6->addTelescopeTabulation(pU6P6); U6->addTelescopeTabulation(aU6P6);

    detectors.insert({U1, U2, U3, U4, U5, U6, P1, P2, P3, P4, P5, P6, G1, G2});

    output->cd(); // ensure output file is used for mid-analysis dumping and post-analysis saving
    tree = new TTree("a", "a");

    NUM = 0;
    tree->Branch("num", &NUM);
    tree->Branch("mul", &mul);

    v_id = make_unique<DynamicBranchVector<unsigned short>>(*tree, "id", "mul");

    v_dir = make_unique<DynamicBranchVector<TVector3>>(*tree, "dir");
    v_pos = make_unique<DynamicBranchVector<TVector3>>(*tree, "pos");

    v_theta = make_unique<DynamicBranchVector<double>>(*tree, "theta", "mul");
    v_phi = make_unique<DynamicBranchVector<double>>(*tree, "phi", "mul");
    v_angle = make_unique<DynamicBranchVector<double>>(*tree, "angle", "mul"); // angle of incidence w.r.t. detector surface

    v_Edep = make_unique<DynamicBranchVector<double>>(*tree, "Edep", "mul");
    v_fEdep = make_unique<DynamicBranchVector<double>>(*tree, "fEdep", "mul");
    v_bEdep = make_unique<DynamicBranchVector<double>>(*tree, "bEdep", "mul");

    v_FI = make_unique<DynamicBranchVector<unsigned short>>(*tree, "FI", "mul");
    v_BI = make_unique<DynamicBranchVector<unsigned short>>(*tree, "BI", "mul");
    v_FE = make_unique<DynamicBranchVector<double>>(*tree, "FE", "mul");
    v_BE = make_unique<DynamicBranchVector<double>>(*tree, "BE", "mul");
    v_FT = make_unique<DynamicBranchVector<double>>(*tree, "FT", "mul");
    v_BT = make_unique<DynamicBranchVector<double>>(*tree, "BT", "mul");

    v_E = make_unique<DynamicBranchVector<double>>(*tree, "E", "mul");
    v_Ea = make_unique<DynamicBranchVector<double>>(*tree, "Ea", "mul");
    v_Eg = make_unique<DynamicBranchVector<double>>(*tree, "Eg", "mul");

    v_Q2p = make_unique<DynamicBranchVector<double>>(*tree, "Q2p", "mul");
    v_Theta = make_unique<DynamicBranchVector<double>>(*tree, "Theta", "mul");

//    tree->Branch("TPATTERN", &TPATTERN);
    //tree->Branch("TPROTONS", &TPROTONS);
    tree->Branch("CLOCK", &CLOCK);

    pSiCalc = defaultRangeInverter("p", "Silicon");
    for (auto &layer: target->getLayers()) {
      pTargetCalcs.push_back(defaultRangeInverter(Ion::predefined("p"), layer.getMaterial()));
    }

    aSiCalc = defaultRangeInverter("a", "Silicon");
    for (auto &layer: target->getLayers()) {
      aTargetCalcs.push_back(defaultRangeInverter(Ion::predefined("a"), layer.getMaterial()));
    }

    twoPDaughter = Ion(twoPD_string);
  }

  virtual void specificAnalysis() = 0;

  void setup(const SortedSetupOutput &output) override {
    AbstractSortedAnalyzer::setup(output);
//    tpattern = output.getScalerOutput("TPATTERN"); // [unpack_event] event->vme.trlo.events().status.tpat
//    tprotons = output.getScalerOutput("TPROTONS"); // [time_stamp_diff] _tss_T1.get_time_since(event->vme.stamp_t1, event->vme.trlo.events().time_lo.val)
    clock = output.getScalerOutput("CLOCK");
  }


  void analyze() override {
    clear();
    CLOCK = clock.getValue();
    findHits();
    specificAnalysis();
    if (mul > 0) { tree->Fill(); }
    NUM++;
  }

  void findHits() {
    for (const auto &det: detectors) {
      if (exclude_hpges && det->getType() == HPGe) continue;
      if (exclude_U5 && det->getName() == "U5") continue;

      auto type = det->getType();
      switch (type) {
        case DSSSD:
          findDssdHit(det);
          break;
        case Pad:
          findPadHit(det);
          break;
        case HPGe:
          findGermaniumHit(det);
          break;
        case NoType:
          cerr << "Warning: Detector " << det->getName() << " of type 'NoType' encountered." << endl;
          break;
      }
    }
  }

  void findDssdHit(E21010Detector *detector) {
    unsigned short id = detector->getId();
    auto &o = output.getDssdOutput(detector->getName());
    auto &d = o.detector();
    auto m = AUSA::mul(o);

    for (int i = 0; i < m; i++) {
      Hit hit;
      hit.det = detector;
      hit.id = id;

      hit.Edep = energy(o, i);
      if (!include_beta_region && hit.Edep <= detector->getBetaCutoff()) continue;

      auto FI = fSeg(o, i);
      auto BI = bSeg(o, i);
      if (!include_dsd_rim && (FI == 1 || FI == 16 || BI == 1 || BI == 16)) continue;
      if (!include_spurious_zone) {
          double Ethr = INFINITY;
          for (auto &tab : detector->getTelescopeTabulations()) {
            if (tab->getIon() != "p") continue;
            if (Ethr > tab->getEnergyDepositionAtReachThrough(FI, BI)) {
              Ethr = tab->getEnergyDepositionAtReachThrough(FI, BI);
            }
          }
        // Particle energy cannot be uniquely assigned when the deposited energy in a Delta-E detector
        // of a Delta-E:E telescope is between the punch through and reach through thresholds.
        if (hit.Edep >= Ethr) continue;
      }

      hit.FI = short(FI);
      hit.BI = short(BI);
      hit.FE = fEnergy(o, i);
      hit.BE = bEnergy(o, i);
      hit.FT = fTime(o, i);
      hit.BT = bTime(o, i);

      TVector3 pos = d.getUniformPixelPosition(FI, BI);
      hit.pos = pos;
      TVector3 dir = (hit.pos - origin).Unit();
      hit.dir = dir;

      hit.theta = dir.Theta();
      hit.phi = dir.Phi();
      hit.angle = dir.Angle(-d.getNormal());

      hits.emplace_back(std::move(hit));
    }
  }

  void findPadHit(E21010Detector *detector) {
    // P5 is dead :-(
    if (detector->getName() == "P5") return;

    unsigned short id = detector->getId();
    auto &o = output.getSingleOutput(detector->getName());
    auto &d = o.detector();
    auto m = AUSA::mul(o);

    for (int i = 0; i < m; i++) {
      Hit hit;
      hit.det = detector;
      hit.id = id;

      hit.Edep = o.energy(i);

      auto FI = o.segment(i);
      hit.FI = short(FI);
      hit.FE = hit.Edep;
      hit.FT = o.time(i);

      TVector3 pos = d.getPosition(FI);
      hit.pos = pos;
      TVector3 dir = (hit.pos - origin).Unit();
      hit.dir = dir;

      hit.theta = dir.Theta();
      hit.phi = dir.Phi();
      hit.angle = dir.Angle(-d.getNormal());

      hits.emplace_back(std::move(hit));
    }
  }

  void findGermaniumHit(E21010Detector *detector) {
    unsigned short id = detector->getId();
    auto &o = output.getSingleOutput(detector->getName());
    auto &d = o.detector();
    auto m = AUSA::mul(o);

    for (int i = 0; i < m; i++) {
      Hit hit;
      hit.det = detector;
      hit.id = id;

      hit.Edep = o.energy(i);

      auto FI = o.segment(i);
      hit.FI = short(FI);
      hit.FE = hit.Edep;
      hit.FT = o.time(i);

      TVector3 pos = d.getPosition(FI);
      hit.pos = pos;
      TVector3 dir = (hit.pos - origin).Unit();
      hit.dir = dir;

      hit.theta = dir.Theta();
      hit.phi = dir.Phi();
      hit.angle = dir.Angle(-d.getNormal());

      hits.emplace_back(std::move(hit));
    }
  }

  void treatDsssdHit(Hit *hit) {
    auto det = hit->det;

    double angle = hit->angle;
    double fdl = det->getFrontDeadLayer() / abs(cos(angle));

    double E = hit->Edep;
    E += pSiCalc->getTotalEnergyCorrection(E, fdl);
    auto &from = hit->pos;
    for (auto &intersection: target->getIntersections(from, origin)) {
      auto &calc = pTargetCalcs[intersection.index];
      E += calc->getTotalEnergyCorrection(E, intersection.transversed);
    }
    hit->E = E;

    double Ea = hit->Edep;
    Ea += aSiCalc->getTotalEnergyCorrection(Ea, fdl);
    for (auto &intersection: target->getIntersections(from, origin)) {
      auto &calc = aTargetCalcs[intersection.index];
      Ea += calc->getTotalEnergyCorrection(Ea, intersection.transversed);
    }
    hit->Ea = Ea;
  }

  void treatPadHit(Hit *hit) {
    auto det = hit->det;

    double angle = hit->angle;
    double fdl = det->getFrontDeadLayer() / abs(cos(angle));

    double E = hit->Edep;
    E += pSiCalc->getTotalEnergyCorrection(E, fdl);
    auto &from = hit->pos;
    for (auto &intersection: target->getIntersections(from, origin)) {
      auto &calc = pTargetCalcs[intersection.index];
      E += calc->getTotalEnergyCorrection(E, intersection.transversed);
    }
    hit->E = E;
  }

  bool GammaGate(Hit *hit, double Emin, double Emax) {
    auto det = hit->det;
    double E = hit->Edep;
    if (Emin <= E && E <= Emax) {
      return true;
    } else {
      return false;
    }
  }

  bool treatTelescopeHit(Hit *front_hit, Hit *back_hit) {
    if (front_hit->Edep == 0.0 || back_hit->Edep == 0.0) return false;

    auto fdet = front_hit->det;
    auto bdet = back_hit->det;

    double angle = front_hit->angle;
    auto fdlB = bdet->getFrontDeadLayer() / abs(cos(angle));
    auto bdlF = fdet->getBackDeadLayer() / abs(cos(angle));
    auto fdlF = fdet->getFrontDeadLayer() / abs(cos(angle));

    double E = back_hit->Edep;
    E += pSiCalc->getTotalEnergyCorrection(E, fdlB);
    E += pSiCalc->getTotalEnergyCorrection(E, bdlF);
    E += front_hit->Edep;
    E += pSiCalc->getTotalEnergyCorrection(E, fdlF);
    auto &from = front_hit->pos;
    for (auto &intersection: target->getIntersections(from, origin)) {
      auto &calc = pTargetCalcs[intersection.index];
      E += calc->getTotalEnergyCorrection(E, intersection.transversed);
    }

    double Ea = back_hit->Edep;
    Ea += aSiCalc->getTotalEnergyCorrection(Ea, fdlB);
    Ea += aSiCalc->getTotalEnergyCorrection(Ea, bdlF);
    Ea += front_hit->Edep;
    Ea += aSiCalc->getTotalEnergyCorrection(Ea, fdlF);
    for (auto &intersection: target->getIntersections(from, origin)) {
      auto &calc = aTargetCalcs[intersection.index];
      Ea += calc->getTotalEnergyCorrection(Ea, intersection.transversed);
    }

    if (!include_spurious_zone) {
      auto FI = front_hit->FI;
      auto BI = front_hit->BI;
      double Emin = INFINITY, Emax = -1.*INFINITY;
      for (auto &tab : fdet->getTelescopeTabulations()) {
        if (tab->getIon() != "p") continue;
        if (Emin > tab->getParticleEnergyAtPunchThrough(FI, BI)) {
          Emin = tab->getParticleEnergyAtPunchThrough(FI, BI);
        }
        if (Emax < tab->getParticleEnergyAtReachThrough(FI, BI)) {
          Emax = tab->getParticleEnergyAtReachThrough(FI, BI);
        }
      }
      // Particle cannot be observed in detector telescope between punch through and reach through thresholds.
      if (Emin <= E && E <= Emax) return false;
    }

    front_hit->E = E;
    front_hit->Ea = Ea;

    return true;
  }

  void addDssdHit(Hit *hit) {
    v_id->add(hit->id);

    v_dir->add(hit->dir);
    v_pos->add(hit->pos);

    v_theta->add(hit->theta);
    v_phi->add(hit->phi);
    v_angle->add(hit->angle);

    v_Edep->add(hit->Edep);
    v_fEdep->add(NAN);
    v_bEdep->add(NAN);

    v_FI->add(hit->FI);
    v_BI->add(hit->BI);
    v_FE->add(hit->FE);
    v_BE->add(hit->BE);
    v_FT->add(hit->FT);
    v_BT->add(hit->BT);

    v_E->add(hit->E);
    v_Ea->add(hit->Ea);
    v_Eg->add(NAN);

    v_Q2p->add(NAN);
    v_Theta->add(NAN);

    mul++;
  }

  void addGermaniumHit(Hit *hit) {
    v_id->add(hit->id);

    v_dir->add(hit->dir);
    v_pos->add(hit->pos);

    v_theta->add(hit->theta);
    v_phi->add(hit->phi);
    v_angle->add(hit->angle);

    v_Edep->add(hit->Edep);
    v_fEdep->add(NAN);
    v_bEdep->add(NAN);

    v_FI->add(hit->FI);
    v_BI->add(NAN_UINT);
    v_FE->add(hit->FE);
    v_BE->add(NAN_UINT);
    v_FT->add(hit->FT);
    v_BT->add(NAN_UINT);

    v_E->add(NAN);
    v_Ea->add(NAN);
    v_Eg->add(hit->Edep);

    v_Q2p->add(NAN);
    v_Theta->add(NAN);

    mul++;
  }

  void addTelescopeHit(Hit *front_hit, Hit *back_hit) {
    v_id->add(front_hit->id);

    v_dir->add(front_hit->dir);
    v_pos->add(front_hit->pos);

    v_theta->add(front_hit->theta);
    v_phi->add(front_hit->phi);
    v_angle->add(front_hit->angle);

    v_Edep->add(NAN);
    v_fEdep->add(front_hit->Edep);
    v_bEdep->add(back_hit->Edep);

    v_FI->add(front_hit->FI);
    v_BI->add(front_hit->BI);
    v_FE->add(front_hit->FE);
    v_BE->add(front_hit->BE);
    v_FT->add(front_hit->FT);
    v_BT->add(front_hit->BT);

    v_E->add(front_hit->E);
    v_Ea->add(front_hit->Ea);
    v_Eg->add(NAN);
    
    v_Q2p->add(NAN);
    v_Theta->add(NAN);

    mul++;
  }

  void addTwoProtonHit(Hit *hit, double Q2p, double Theta) {
    v_id->add(hit->id);

    v_dir->add(hit->dir);
    v_pos->add(hit->pos);

    v_theta->add(hit->theta);
    v_phi->add(hit->phi);
    v_angle->add(hit->angle);

    v_Edep->add(hit->Edep);
    v_fEdep->add(NAN);
    v_bEdep->add(NAN);

    v_FI->add(hit->FI);
    v_BI->add(hit->BI);
    v_FE->add(hit->FE);
    v_BE->add(hit->BE);
    v_FT->add(hit->FT);
    v_BT->add(hit->BT);

    v_E->add(hit->E);
    v_Ea->add(NAN);
    v_Eg->add(NAN);
    
    v_Q2p->add(Q2p);
    v_Theta->add(Theta);

    mul++;
  }

  void terminate() override {
    AbstractSortedAnalyzer::terminate();
    gDirectory->WriteTObject(tree);
  }

  void clear() {
    mul = 0;
    AUSA::clear(
        *v_id,
        *v_dir, *v_pos,
        *v_theta, *v_phi, *v_angle,
        *v_Edep, *v_fEdep, *v_bEdep,
        *v_FI, *v_BI, *v_FE, *v_BE, *v_FT, *v_BT,
        *v_E, *v_Ea, *v_Eg,
        *v_Q2p, *v_Theta
    );
    hits.clear();
  }

  unordered_set<E21010Detector *> detectors;

  TTree *tree;
  int NUM;
  UInt_t mul{}, TPATTERN{}, TPROTONS{}, CLOCK{};
  unique_ptr<DynamicBranchVector<unsigned short>> v_id;
  unique_ptr<DynamicBranchVector<TVector3>> v_dir, v_pos;
  unique_ptr<DynamicBranchVector<double>> v_theta, v_phi, v_angle;
  unique_ptr<DynamicBranchVector<double>> v_Edep, v_fEdep, v_bEdep;
  unique_ptr<DynamicBranchVector<unsigned short>> v_FI, v_BI;
  unique_ptr<DynamicBranchVector<double>> v_FE, v_BE;
  unique_ptr<DynamicBranchVector<double>> v_FT, v_BT;
  unique_ptr<DynamicBranchVector<double>> v_E, v_Ea, v_Eg;
  unique_ptr<DynamicBranchVector<double>> v_Q2p;
  unique_ptr<DynamicBranchVector<double>> v_Theta;

  vector<Hit> hits;

  unique_ptr<EnergyLossRangeInverter> pSiCalc;
  unique_ptr<EnergyLossRangeInverter> aSiCalc;
  vector<unique_ptr<EnergyLossRangeInverter>> pTargetCalcs;
  vector<unique_ptr<EnergyLossRangeInverter>> aTargetCalcs;
  map<unsigned short, double> fdeadlayerF, fdeadlayerB, bdeadlayer;
  shared_ptr<Setup> setupSpec;
  shared_ptr<Target> target;

  SortedSignal tpattern, tprotons, clock;

  double implantation_depth;
  TVector3 origin;

  bool exclude_hpges, exclude_U5, include_dsd_rim, include_beta_region, include_spurious_zone;

  TelescopeTabulation *pU1P1, *pU2P2, *pU3P3, *pU4P4, *pU6P6;
  TelescopeTabulation *aU1P1, *aU2P2, *aU3P3, *aU4P4, *aU6P6;
  Ion twoPDaughter;
  string twoPD_string;
};

#endif //E21010GENERALANALYSIS_H
