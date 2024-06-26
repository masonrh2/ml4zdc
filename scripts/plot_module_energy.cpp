#include "ROOT/RDF/RInterface.hxx"
#include "ROOT/RDataFrame.hxx"

#include "../include/ZDCModule.hpp"
#include "../include/Axis.hpp"

#include "TLegend.h"
#include "TCanvas.h"
#include "THStack.h"

struct SimulationConfig {
  std::string tag;
  std::string path;
  float maxTruthEnergy;
  float maxZDCModuleAmp;
};

std::array<Color_t, N_SIM_MODULES_USED> MODULE_COLORS = {
  kRed - 4,
  kOrange + 7,
  kOrange - 2,
  kGreen + 1,
  kAzure + 10,
  kAzure - 3,
};

std::vector<SimulationConfig> const SIM_CONFIGS = {
  {"1n", "../data/SingleNeutronNew.2024.05.19_NTUP.root", 1e3, 2e3},
  {"40n", "../data/40Neutrons_NTUP.root", 1e4, 25e3},
  {"topo_1n", "../data/zdcTopoAnalysis_1N.root", 1e3, 2e3},
  {"topo_5n", "../data/zdcTopoAnalysis_5N.root", 2e3, 5e3},
};

std::string const OUT_FILE_PATH = "../plots/module_energy.root";

void writePlot(SimulationConfig const& config) {
  axis::Axis const recoModuleAxis {128, 0, config.maxZDCModuleAmp};
  axis::Axis const truthModuleAxis {128, 0, config.maxTruthEnergy};
  auto dataframe = ROOT::RDataFrame("zdcTree", config.path);
  std::array<std::array<ROOT::RDF::RResultPtr<TH1D>, N_SIM_MODULES_USED>, 2> hTruthEnergy {};
  // std::array<ROOT::RDF::RResultPtr<TH1D>, 2> hZDCRecoEnergy {};
  std::array<std::array<ROOT::RDF::RResultPtr<TH1D>, N_RECO_MODULES>, 2> hZDCModuleRecoEnergy {};
  // std::array<ROOT::RDF::RResultPtr<TH1D>, 2> hRPDSumSumADC {};
  std::array<std::array<ROOT::RDF::RResultPtr<TH2D>, N_RECO_MODULES>, 2> hModuleCorrelation {};
  for (auto const& side : SIDES) {
    ROOT::RDF::RInterface<ROOT::Detail::RDF::RLoopManager, void> dataframeUnpacked = dataframe;
    for (unsigned int mod = 0; mod < N_SIM_MODULES_USED; mod++) {
      dataframeUnpacked = dataframeUnpacked.Define(
        SIM_MODULE_NAMES.at(mod) + "_truth",
        getVectorUnpackerScalerFunc(getModuleIndex(side, static_cast<SimModule>(mod)), /* Mev to GeV */ 1e-3),
        {MODULE_TRUTH_ENERGIES_BRANCH}
      );
    }
    for (unsigned int mod = 0; mod < N_RECO_MODULES; mod++) {
      auto const recoMod = static_cast<RecoModule>(mod);
      auto moduleName = RECO_MODULE_NAMES.at(mod).c_str();
      auto dfValidMod = dataframeUnpacked.Filter(
        Reco::ZDCModule::checkValid(side, recoMod), {"zdc_ZdcModuleStatus"}
      ).Define(
        RECO_MODULE_NAMES.at(mod) + "_reco", Reco::ZDCModule::getEnergy(side, recoMod), {"zdc_ZdcModuleAmp"}
      );
      hZDCModuleRecoEnergy.at(side).at(mod) = dfValidMod.Histo1D<float>(
        {
          Form("side%c_%s_reco", getSideLabel(side), moduleName),
          Form(";%s Reco. Amplitude [ADC]; Count", moduleName),
          BINS(recoModuleAxis)
        }, RECO_MODULE_NAMES.at(mod) + "_reco"
      );
      hModuleCorrelation.at(side).at(mod) = dfValidMod.Histo2D<float, float>(
        {
          Form("side%c_%s_correlation", getSideLabel(side), moduleName),
          Form(";%s Truth Energy [GeV];%s Reco. Amplitude [ADC]; Count", moduleName, moduleName),
          BINS(truthModuleAxis), BINS(recoModuleAxis)
        }, SIM_MODULE_NAMES.at(mod) + "_truth", RECO_MODULE_NAMES.at(mod) + "_reco"
      );
    }
    for (unsigned int mod = 0; mod < N_SIM_MODULES_USED; mod++) {
      hTruthEnergy.at(side).at(mod) = dataframeUnpacked.Histo1D<float>(
        {
          Form("side%c_%s_truth", getSideLabel(side), SIM_MODULE_NAMES.at(mod).c_str()),
          Form(";%s Truth Energy [GeV];Count", SIM_MODULE_NAMES.at(mod).c_str()),
          BINS(truthModuleAxis)
        },
        SIM_MODULE_NAMES.at(mod) + "_truth"
      );
    }
  }

  for (auto const side : SIDES) {
    {
      auto canvas = std::make_unique<TCanvas>(Form("%s_side%c_moduleTruthEnergies", config.tag.c_str(), getSideLabel(side)));
        THStack *stack = new THStack();
        TLegend *legend = new TLegend(
          0.7,
          0.55,
          1 - gPad->GetRightMargin(),
          1 - gPad->GetTopMargin()
        );
        for (unsigned int i = 0; i < N_SIM_MODULES_USED; i++) {
          auto const mod = MODULE_PLOT_ORDER.at(i);
          auto & hist = hTruthEnergy.at(side).at(mod);
          auto const& name = SIM_MODULE_NAMES.at(mod);
          hist->SetLineColor(MODULE_COLORS.at(i));
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
    {
      auto canvas = std::make_unique<TCanvas>(Form("%s_side%c_moduleRecoEnergies", config.tag.c_str(), getSideLabel(side)));
        THStack *stack = new THStack();
        TLegend *legend = new TLegend(
          0.7,
          0.55,
          1 - gPad->GetRightMargin(),
          1 - gPad->GetTopMargin()
        );
        for (unsigned int mod = 0; mod < N_RECO_MODULES; mod++) {
          auto & hist = hZDCModuleRecoEnergy.at(side).at(mod);
          auto const& name = RECO_MODULE_NAMES.at(mod);
          hist->SetLineColor(MODULE_COLORS.at(mod));
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
    for (unsigned int mod = 0; mod < N_RECO_MODULES; mod++) {
      auto & hist = hModuleCorrelation.at(side).at(mod);
      auto canvas = std::make_unique<TCanvas>(Form("%s_side%c_%s_correlation", config.tag.c_str(), getSideLabel(side), RECO_MODULE_NAMES.at(mod).c_str()));
      canvas->SetLogz();
      hist->Draw("COLZ");
      canvas->Write();
    }
  }
}

void plot_module_energy() {
  ROOT::EnableImplicitMT();
  TFile *outFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
  for (auto const& config : SIM_CONFIGS) {
    outFile->mkdir(config.tag.c_str())->cd();
    writePlot(config);
  }
  outFile->Close();
}
