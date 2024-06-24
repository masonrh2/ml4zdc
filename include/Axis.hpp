#pragma once

#include "ZDCModule.hpp"

namespace axis {
  struct Axis {
    int const nbins;
    float const xmin;
    float const xmax;
    Axis withBins(int const newnbins) const {
      return {newnbins, xmin, xmax};
    }
  };

  namespace singleNeutron {
    Axis constexpr EMTruth {64, 0, 2e5};
    Axis constexpr HAD1Truth {64, 0, 2e5};
    Axis constexpr HAD2Truth {64, 0, 2e5};
    Axis constexpr HAD3Truth {64, 0, 2e5};
    Axis constexpr RPDTruth {64, 0, 1e5};
    Axis constexpr BRANTruth {64, 0, 1e6};
    std::array<Axis, N_SIM_MODULES_USED> constexpr moduleAxes = {EMTruth, HAD1Truth, HAD2Truth, HAD3Truth, RPDTruth, BRANTruth};
  }
  namespace fortyNeutron {
    Axis constexpr EMTruth {64, 0, 1e6};
    Axis constexpr HAD1Truth {64, 0, 1e6};
    Axis constexpr HAD2Truth {64, 0, 1e6};
    Axis constexpr HAD3Truth {64, 0, 1e6};
    Axis constexpr RPDTruth {64, 0, 5e5};
    Axis constexpr BRANTruth {64, 0, 5e6};
    std::array<Axis, N_SIM_MODULES_USED> constexpr moduleAxes = {EMTruth, HAD1Truth, HAD2Truth, HAD3Truth, RPDTruth, BRANTruth};
  }

  Axis constexpr xAtRPD {64, -25, 25};
  Axis constexpr yAtRPD {64, -25, 25};
}

#define BINS(axis) axis.nbins, axis.xmin, axis.xmax
