#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderArray.h"
#include "TH2.h" // 2-dimensional histograms, for full docs see https://root.cern.ch/doc/master/classTH2.html

#include "../include/ZDCModule.hpp"
#include "../include/Axis.hpp"

std::string const SIM_FILE_PATH = "../data/SingleNeutronNew.2024.05.19_NTUP.root";
std::string const OUT_FILE_PATH = "../plots/had1_vs_bran_v1.root";

namespace axisConfig = axis::singleNeutron;

/**
 * @brief plot EM module truth vs BRAN truth
 * this version is simpler and plots with an event loop, using TTreeReader
 * run with, for example: root -b -q -l plot_em_vs_bran_v1.cpp
 */
inline void plot_had1_vs_bran() {
  // this disables automatic object ownership, which can cause problems when you allocate new histograms
  // while simultaneously opening and closing TFiles
  // see https://root.cern/manual/object_ownership/
  TH1::AddDirectory(false);
  
  TFile* dataFile = TFile::Open(SIM_FILE_PATH.c_str());

  TTreeReader reader("zdcTree", dataFile);
  TTreeReaderArray<float> zdc_ZdcModuleTruthTotal(reader, "zdc_ZdcModuleTruthTotal");

  std::array<TH2D*, 2> hHAD1VsBRAN {};
  for (auto const& side : SIDES) {
    hHAD1VsBRAN.at(side) = new TH2D(Form("side%c_HAD1VsBRAN", getSideLabel(side)), ";HAD1 Truth Energy [MeV];BRAN Truth Energy [MeV];Count", BINS(axisConfig::EMTruth.withBins(32)), BINS(axisConfig::BRANTruth.withBins(32)));
  }

  while (reader.Next()) {
    for (auto const& side : SIDES) {
      hHAD1VsBRAN.at(side)->Fill(zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::HAD1)), zdc_ZdcModuleTruthTotal.At(getModuleIndex(side, SimModule::BRAN)));
    }
  }

  dataFile->Close();

  TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
  for (auto const& side : SIDES) {
    hHAD1VsBRAN.at(side)->Write();
  }
  plotFile->Close();

  for (unsigned int side : SIDES) {
    delete hHAD1VsBRAN.at(side);
  }
}
