namespace axis {
  struct Axis {
    int const nbins;
    float const xmin;
    float const xmax;
    Axis withBins(int const newnbins) const {
      return {newnbins, xmin, xmax};
    }
  };

  Axis constexpr EMTruth {64, 0, 2e5};
  Axis constexpr HAD1Truth {64, 0, 1e6};
  Axis constexpr HAD2Truth {64, 0, 1e6};
  Axis constexpr HAD3Truth {64, 0, 1e6};
  Axis constexpr RPDTruth {64, 0, 1e6};
  Axis constexpr BRANTruth {64, 0, 1e6};
}

#define BINS(axis) axis.nbins, axis.xmin, axis.xmax
