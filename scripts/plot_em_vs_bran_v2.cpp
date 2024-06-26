#include "ROOT/RDataFrame.hxx" // for full docs, see https://root.cern/doc/master/classROOT_1_1RDataFrame.html

#include "../include/Axis.hpp"
#include "../include/ZDCModule.hpp"

std::string const SIM_FILE_PATH = "../data/SingleNeutronNew.2024.05.19_NTUP.root";
std::string const OUT_FILE_PATH = "../plots/em_vs_bran_v2.root";

namespace axisConfig = axis::singleNeutron;

/**
 * @brief plot EM module truth vs BRAN truth
 * this version is different and plots with RDataFrame, which is a new addition to ROOT and can be much faster
 * I tend to prefer this way, but it can sometimes be tricky to get what you want
 * run with, for example: root -b -q -l plot_em_vs_bran_v2.cpp
 */
inline void plot_em_vs_bran_v2() {
  // ROOT will use multiple threads where possible
  // this is great for large files, probably not much benefit for 100k events, though
  ROOT::EnableImplicitMT();

  ROOT::RDataFrame dataframe("zdcTree", SIM_FILE_PATH);

  std::array<ROOT::RDF::RResultPtr< ::TH2D>, 2> hEMVsBRAN {};

  for (auto const& side : SIDES) {
    // define new "branches" for em and bran energies, unpacking them from the vector branch
    auto dataframeUnpacked = dataframe.Define(
      "em_truth", getVectorUnpackerFunc(getModuleIndex(side, SimModule::EM)), {MODULE_TRUTH_ENERGIES_BRANCH}
    ).Define(
      "bran_truth", getVectorUnpackerFunc(getModuleIndex(side, SimModule::BRAN)), {MODULE_TRUTH_ENERGIES_BRANCH}
    );
    // make histogram from these new branches
    hEMVsBRAN.at(side) = dataframeUnpacked.Histo2D<float, float>(
      {Form("side%c_EMVsBRAN", getSideLabel(side)), ";EM Truth Energy [MeV];BRAN Truth Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)), BINS(axisConfig::BRANTruth.withBins(32))},
      "em_truth", "bran_truth"
    );
  }

  TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
  for (auto const& side : SIDES) {
    hEMVsBRAN.at(side)->Write();
  }
  plotFile->Close();
}
