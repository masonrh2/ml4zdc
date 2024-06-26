#pragma once

#include <array>

namespace status {

namespace ZDCSum {
  enum {
    AllFitSuccessBit,
    N_BITS
  };
  std::array<std::string, N_BITS> const bitNames {
    "AllFitSuccess"
  };
} // namespace ZDCSum

namespace ZDCModule {
  enum {
    PulseBit,
    LowGainBit,
    FailBit,
    HGOverflowBit,
    HGUnderflowBit,
    PSHGOverUnderflowBit,
    LGOverflowBit,
    LGUnderflowBit,
    PrePulseBit,
    PostPulseBit,
    FitFailedBit,
    BadChisqBit,
    BadT0Bit,
    ExcludeEarlyLGBit,
    ExcludeLateLGBit,
    preExpTailBit,
    FitMinAmpBit,
    RepassPulseBit,
    // ArmSumIncludeBit      = 18, // this is not saved in aux data (or ntuple)!
    N_BITS
  };
  std::array<std::string, N_BITS> const bitNames {
    "Pulse",
    "LowGain",
    "Fail",
    "HGOverflow",
    "HGUnderflow",
    "PSHGOverUnderflow",
    "LGOverflow",
    "LGUnderflow",
    "PrePulse",
    "PostPulse",
    "FitFailed",
    "BadChisq",
    "BadT0",
    "ExcludeEarlyLG",
    "ExcludeLateLG",
    "preExpTail",
    "FitMinAmp",
    "RepassPulse",
    // "ArmSumInclude",
  };
} // namespace ZDCModule

namespace RPD {
  enum {
    ValidBit, // analysis and output are valid
    OutOfTimePileupBit, // OOT detected, pileup subtraction attempted
    OverflowBit, // overflow detected => invalid
    PrePulseBit, // pulse detected before expected range => invalid
    PostPulseBit, // pulse detected after expected range => invalid
    NoPulseBit, // no pulse detected => invalid
    BadAvgBaselineSubtrBit, // subtraction of avg. of baseline samples yielded too many negatives => invalid
    InsufficientPileupFitPointsBit, // baseline samples indicate pileup, but there are not enough points to perform fit -> nominal baseline used without pileup subtraction
    PileupStretchedExpFitFailBit, // fit to stretched exponential failed -> fallback to exponential fit
    PileupStretchedExpGrowthBit, // fit to stretched exponential does not decay -> fallback to exponential fit
    PileupBadStretchedExpSubtrBit, // subtraction of stretched exponential fit yielded too many negatives -> fallback to exponential fit
    PileupExpFitFailBit, // fit to exponential failed => invalid IF stretched exponential fit is also bad
    PileupExpGrowthBit, // fit to exponential does not decay => invalid IF stretched exponential fit is also bad
    PileupBadExpSubtrBit, // subtraction of stretched exponential yielded too many negatives => invalid IF stretched exponential fit is also bad
    PileupStretchedExpPulseLikeBit, // fit to stretched exponential probably looks more like a pulse than pileup
    N_BITS
  };
  std::array<std::string, N_BITS> const bitNames {
    "Valid",
    "OutOfTimePileup",
    "Overflow",
    "PrePulse",
    "PostPulse",
    "NoPulse",
    "BadAvgBaselineSubtr",
    "InsufficientPileupFitPoints",
    "PileupStretchedExpFitFail",
    "PileupStretchedExpGrowth",
    "PileupBadStretchedExpSubtr",
    "PileupExpFitFail",
    "PileupExpGrowth",
    "PileupBadExpSubtr",
    "PileupStretchedExpPulseLike",
  };
} // namespace RPD

namespace Centroid {
  enum {
    ValidBit                     =  0, // analysis and output are valid
    HasCentroidBit               =  1, // centroid was calculated but analysis is invalid
    ZDCInvalidBit                =  2, // ZDC analysis on this side failed => analysis is invalid
    InsufficientZDCEnergyBit     =  3, // ZDC energy on this side is below minimum => analysis is invalid
    ExcessiveZDCEnergyBit        =  4, // ZDC energy on this side is above maximum => analysis is invalid
    EMInvalidBit                 =  5, // EM analysis on this side failed => analysis is invalid
    InsufficientEMEnergyBit      =  6, // EM energy on this side is below minimum => analysis is invalid
    ExcessiveEMEnergyBit         =  7, // EM energy on this side is above maximum => analysis is invalid
    RPDInvalidBit                =  8, // RPD analysis on this side was invalid => calculation stopped and analysis is invalid
    PileupBit                    =  9, // pileup was detected in RPD on this side
    ExcessivePileupBit           = 10, // pileup was detected in RPD on this side and a channel exceeded the fractional limit => analysis is invalid
    ZeroSumBit                   = 11, // sum of subtracted RPD amplitudes on this side was not positive => calculation stopped and analysis is invalid
    ExcessiveSubtrUnderflowBit   = 12, // a subtracted RPD amplitude on this side was negative and exceeded the fractional limit => analysis is invalid

    Row0ValidBit                 = 13, // row 0 x centroid is valid
    Row1ValidBit                 = 14, // row 1 x centroid is valid
    Row2ValidBit                 = 15, // row 2 x centroid is valid
    Row3ValidBit                 = 16, // row 3 x centroid is valid
    Col0ValidBit                 = 17, // column 0 y centroid is valid
    Col1ValidBit                 = 18, // column 1 y centroid is valid
    Col2ValidBit                 = 19, // column 2 y centroid is valid
    Col3ValidBit                 = 20, // column 3 y centroid is valid
    N_BITS
  };
  std::array<std::string, N_BITS> const bitNames {
    "Valid",
    "HasCentroid",
    "ZDCInvalid",
    "InsufficientZDCEnergy",
    "ExcessiveZDCEnergy",
    "EMInvalid",
    "InsufficientEMEnergy",
    "ExcessiveEMEnergy",
    "RPDInvalid",
    "Pileup",
    "ExcessivePileup",
    "ZeroSum",
    "ExcessiveSubtrUnderflow",
    "Row0Valid",
    "Row1Valid",
    "Row2Valid",
    "Row3Valid",
    "Col0Valid",
    "Col1Valid",
    "Col2Valid",
    "Col3Valid",
  };
} // namespace Centroid

} // namespace status
