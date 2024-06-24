#include "ROOT/RDF/RInterface.hxx"
#include "ROOT/RDataFrame.hxx"

#include "../include/ZDCModule.hpp"
#include "../include/Axis.hpp"

#include "TLegend.h"
#include "TCanvas.h"
#include "THStack.h"
#include "TColor.h"
#include "TStyle.h"

template <unsigned int NColors>
std::array<int, NColors> getColorsFromPalette(int const palette) {
  // courtesy
  auto curPalette = TColor::GetPalette();

  gStyle->SetPalette(palette);
  auto allColors = TColor::GetPalette();
  std::array<int, NColors> colors;
  for (unsigned int i = 0; i < NColors; i++) {
    colors.at(i) = allColors.At(static_cast<float>(i) / (NColors - 1) * (allColors.GetSize() - 1));
  }

  // courtesy
  TColor::SetPalette(curPalette.GetSize(), curPalette.GetArray());

  return colors;
}

struct SimulationConfig {
  std::string tag;
  std::string path;
  float maxEnergy;
};

std::vector<SimulationConfig> const SIM_CONFIGS = {
  {"1n", "../data/SingleNeutronNew.2024.05.19_NTUP.root", 1e3},
  {"40n", "../data/40Neutrons_NTUP.root", 1e4},
};

std::string const OUT_FILE_PATH = "../plots/plot_module_energy.root";

void writePlot(SimulationConfig const& config) {
  axis::Axis const sharedAxis {128, 0, config.maxEnergy};
  auto dataframe = ROOT::RDataFrame("zdcTree", config.path);
  std::array<std::array<ROOT::RDF::RResultPtr< ::TH1D>, N_SIM_MODULES_USED>, 2> hDistribution {};
  for (auto const& side : SIDES) {
    ROOT::RDF::RInterface<ROOT::Detail::RDF::RLoopManager, void> dataframeUnpacked = dataframe;
    for (unsigned int mod = 0; mod < N_SIM_MODULES_USED; mod++) {
      dataframeUnpacked = dataframeUnpacked.Define(
        MODULE_NAMES.at(mod) + "_truth",
        getVectorUnpackerScalerFunc(getModuleIndex(side, static_cast<SimModule>(mod)), /* Mev to GeV */ 1e-3),
        {MODULE_TRUTH_ENERGIES_BRANCH}
      );
    }
    for (unsigned int i = 0; i < N_SIM_MODULES_USED; i++) {
      hDistribution.at(side).at(i) = dataframeUnpacked.Histo1D<float>(
        {
          Form("side%c_%s", getSideLabel(side), MODULE_NAMES.at(i).c_str()),
          Form(";%s Truth Energy [GeV];Count", MODULE_NAMES.at(i).c_str()),
          BINS(sharedAxis)
        },
        MODULE_NAMES.at(i) + "_truth"
      );
    }
  }
  auto const moduleColors = getColorsFromPalette<N_SIM_MODULES_USED>(kRainBow);

  for (auto const side : SIDES) {
    auto canvas = std::make_unique<TCanvas>(Form("%s_side%c_moduleEnergies", config.tag.c_str(), getSideLabel(side)));
    THStack *stack = new THStack();
    TLegend *legend = new TLegend(
      0.7,
      0.55,
      1 - gPad->GetRightMargin(),
      1 - gPad->GetTopMargin()
    );
    for (unsigned int i = 0; i < N_SIM_MODULES_USED; i++) {
      auto const mod = MODULE_PLOT_ORDER.at(i);
      auto & hist = hDistribution.at(side).at(mod);
      auto const& name = MODULE_NAMES.at(mod);
      hist->SetLineColor(moduleColors.at(i));
      stack->Add(hist.GetPtr(), "HIST");
      legend->AddEntry(hist.GetPtr(), name.c_str(), "l");
    }
    stack->Draw("NOSTACK");
    legend->Draw();
    canvas->Write();
    
    canvas->SetLogy();
    canvas->Write(Form("%s_logy", canvas->GetName()));
    
    stack->Draw();
    legend->Draw();
    canvas->SetLogy(false);
    canvas->Write(Form("%s_stack", canvas->GetName()));
    
    canvas->SetLogy();
    canvas->Write(Form("%s_stack_logy", canvas->GetName()));
  }
}

void plot_module_energy() {
  ROOT::EnableImplicitMT();
  TFile *outFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
  for (auto const& config : SIM_CONFIGS) {
    writePlot(config);
  }
  outFile->Close();
}
