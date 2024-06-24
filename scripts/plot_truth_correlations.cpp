#include "ROOT/RDataFrame.hxx" // for full docs, see https://root.cern/doc/master/classROOT_1_1RDataFrame.html

#include "../include/Axis.hpp"
#include "../include/ZDCModule.hpp"
#include "TAttMarker.h"
#include "TAttText.h"
#include "TCanvas.h"
#include "TProfile.h"
#include "TPaveText.h"

struct SimulationConfig {
  std::string tag;
  std::string path;
  std::array<axis::Axis, N_SIM_MODULES_USED> const& axes;
};

std::vector<SimulationConfig> const SIM_CONFIGS = {
  {"1n", "../data/SingleNeutronNew.2024.05.19_NTUP.root", axis::singleNeutron::moduleAxes},
  {"40n", "../data/40Neutrons_NTUP.root", axis::fortyNeutron::moduleAxes},
};

std::string const OUT_FILE_PATH = "../plots/truth_correlations.root";

inline void drawText(
  std::string const& text,
  float const textSize = 0.15,
  float const textAngle = 0,
  short const align = kHAlignCenter + kVAlignTop
) {
  TPaveText * pt = new TPaveText(
    gPad->GetLeftMargin(),
    gPad->GetBottomMargin(),
    1 - gPad->GetRightMargin(),
    1 - gPad->GetTopMargin(),
    "NBNDC"
  );
  pt->SetBorderSize(0);
  pt->SetTextColor(12); // some kind of grey
  pt->SetTextAngle(textAngle); // degrees, CCW from +x
  pt->SetFillColorAlpha(kBlack, 0);
  pt->SetTextAlign(align);
  pt->SetTextFont(102); // monospace (like courier new), bold
  pt->SetTextSize(textSize); // as fraction of pad size
  pt->AddText(text.c_str());
  pt->Draw();
}

void plot_single(SimulationConfig const& config) {
  ROOT::RDataFrame dataframe("zdcTree", config.path);

  std::array<std::array<std::array<ROOT::RDF::RResultPtr< ::TH2D>, N_SIM_MODULES_USED>, N_SIM_MODULES_USED>, 2> hCorrelation {};
  std::array<std::array<ROOT::RDF::RResultPtr< ::TH1D>, N_SIM_MODULES_USED>, 2> hDistribution {};

  for (auto const& side : SIDES) {
    ROOT::RDF::RInterface<ROOT::Detail::RDF::RLoopManager, void> dataframeUnpacked = dataframe;
    for (unsigned int mod = 0; mod < N_SIM_MODULES_USED; mod++) {
      dataframeUnpacked = dataframeUnpacked.Define(
        MODULE_NAMES.at(mod) + "_truth", getVectorUnpackerFunc(getModuleIndex(side, static_cast<SimModule>(mod))), {MODULE_TRUTH_ENERGIES_BRANCH}
      );
    }
    for (unsigned int i = 0; i < N_SIM_MODULES_USED; i++) {
      hDistribution.at(side).at(i) = dataframeUnpacked.Histo1D<float>(
        {
          Form("side%c_%s", getSideLabel(side), MODULE_NAMES.at(i).c_str()),
          Form(";%s Truth Energy [MeV];Count", MODULE_NAMES.at(i).c_str()),
          BINS(config.axes.at(i).withBins(48))
        },
        MODULE_NAMES.at(i) + "_truth"
      );
      for (unsigned int j = 0; j < N_SIM_MODULES_USED; j++) {
        if (i == j) continue;
        hCorrelation.at(side).at(i).at(j) = dataframeUnpacked.Histo2D<float, float>(
          {
            Form("side%c_%sVs%s", getSideLabel(side), MODULE_NAMES.at(i).c_str(), MODULE_NAMES.at(j).c_str()),
            Form(";%s Truth Energy [MeV];%s Truth Energy [MeV];Count", MODULE_NAMES.at(i).c_str(), MODULE_NAMES.at(j).c_str()),
            BINS(config.axes.at(i).withBins(48)), BINS(config.axes.at(j).withBins(48))
          },
          MODULE_NAMES.at(i) + "_truth", MODULE_NAMES.at(j) + "_truth"
        );
      }
    }
  }

  unsigned int const M = N_SIM_MODULES_USED + 1;
  for (auto const& side : SIDES) {
    TCanvas * canvas = new TCanvas(Form("%s_side%c_correlation", config.tag.c_str(), SIDE_LABELS.at(side)));
    canvas->Divide(M, M);
    for (unsigned int k = 0; k < M*M; k++) {
      canvas->cd(k + 1);
      // row in grid of plots
      unsigned int const i = k / M;
      // column in grid of plots
      unsigned int const j = k % M;
      if (i == 0 && j == 0) {
        // top left, do nothing
        continue;
      } else if (i == 0 || j == 0) {
        // top row or leftmost column, use as label
        unsigned int const mod = i + j - 1;
        drawText(MODULE_NAMES.at(MODULE_PLOT_ORDER.at(mod)), 0.3, 45, kHAlignCenter + kVAlignCenter);
        continue;
      } // else plot
      if (i == j) {
        // distribution plot
        // gPad->SetLogy();
        ROOT::RDF::RResultPtr< ::TH1D> & hist = hDistribution.at(side).at(MODULE_PLOT_ORDER.at(i - 1));
        hist->Draw();
      } else {
        // correlation plot
        gPad->SetLogz();
        ROOT::RDF::RResultPtr< ::TH2D> & hist = hCorrelation.at(side).at(MODULE_PLOT_ORDER.at(i - 1)).at(MODULE_PLOT_ORDER.at(j - 1));
        hist->Draw("COLZ");
        TProfile * prof = hist->ProfileX();
        prof->SetMarkerStyle(kFullCircle);
        prof->SetMarkerSize(0.5);
        prof->Draw("SAME");
        drawText(Form("%.2f", hist->GetCorrelationFactor()));
      }
    }
    canvas->Write();
  }
}

/**
 * @brief 
 */
 void plot_truth_correlations() {
  ROOT::EnableImplicitMT();
  TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
  for (auto const& config : SIM_CONFIGS) {
    plotFile->mkdir(config.tag.c_str())->cd();
    plot_single(config);
  }
  plotFile->Close();
}
