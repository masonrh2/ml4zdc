#include "ROOT/RDataFrame.hxx" // for full docs, see https://root.cern/doc/master/classROOT_1_1RDataFrame.html

#include "../include/Axis.h"
#include "../include/ZDCModule.h"

std::string const SIM_FILE_PATH = "../data/SingleNeutronNew_2024-05-19_NTUP.root";
// std::string const SIM_FILE_PATH = "../data/ZDC_sim_1n_100k.root";
std::string const OUT_FILE_PATH = "../plots/projection.root";

namespace particleBranches {
  std::string const x = "zdc_ZdcTruthParticlePosx";
  std::string const y = "zdc_ZdcTruthParticlePosy";
  std::string const z = "zdc_ZdcTruthParticlePosz";
  std::string const px = "zdc_ZdcTruthParticlePx";
  std::string const py = "zdc_ZdcTruthParticlePy";
  std::string const pz = "zdc_ZdcTruthParticlePz";
  std::string const e = "zdc_ZdcTruthParticleEnergy";
}

/**
 * @brief get a function that picks an element at a specified index given a vector
 */
inline std::function<float(ROOT::VecOps::RVec<float> const& vec)> getVectorUnpackerFunc(unsigned int const index) {
  return [index] (ROOT::VecOps::RVec<float> const& vec) {
    return vec.at(index);
  };
}

typedef unsigned int ParticleType;
namespace particleTypes {
  ParticleType constexpr toSideC = 0; // particle is heading towards side C
  ParticleType constexpr toSideA = 1; // particle is heading towards side A
  ParticleType constexpr spurious = 2; // not sure what this is, but it's not a neutron and shouldn't be in the TTree! >:(
}

float constexpr SPURIOUS_PARTICLE_Z_MOMENTUM_THRESHOLD = 10000; // MeV

ROOT::VecOps::RVec<ParticleType> classifyParticles(ROOT::VecOps::RVec<float> const& particleZMomentum) {
  std::vector<ParticleType> particleTypes(particleZMomentum.size());
  for (unsigned int i = 0; i < particleZMomentum.size(); i++) {
    float const& pz = particleZMomentum.at(i);
    ParticleType type;
    if (std::abs(pz) < SPURIOUS_PARTICLE_Z_MOMENTUM_THRESHOLD) {
      type = particleTypes::spurious;
    } else if (pz < 0) {
      type = particleTypes::toSideC;
    } else if (pz > 0) {
      type = particleTypes::toSideA;
    }
    particleTypes.at(i) = type;
  }
  return particleTypes;
}

unsigned int getParticleSelectorFunc(ROOT::VecOps::RVec<ParticleType> const& particleTypes, unsigned int const side) {
  std::vector<unsigned int> indices;
  for (unsigned int i = 0; i < particleTypes.size(); i++) {
    ParticleType const& type = particleTypes.at(i);
    if (side == Side::C && type == particleTypes::toSideC) {
      indices.push_back(i);
    } else if (side == Side::A && type == particleTypes::toSideA) {
      indices.push_back(i);
    }
  }
  if (indices.size() == 0) {
    throw std::runtime_error("no particles match side " + std::to_string(getSideLabel(side)) + " selection!");
  } else if (indices.size() > 1) {
    throw std::runtime_error(std::to_string(indices.size()) + " > 1 particles match side " + std::to_string(getSideLabel(side)) + " selection!");
  }
  return indices.at(0);
}

struct RPDPos {
  float const x;
  float const y;
  float const z;
};

std::array<RPDPos, 2> constexpr RUN3_RPD_POSITION = {{
  {-2.012, 21.388, -141489.5},
  {1.774, 21.344, 141459.0},
}};

std::pair<float, float> project(unsigned int const side, float const x0, float const y0, float const z0, float const px, float const py, float const pz) {
 float const horizontalAngle = std::atan2(px, pz);
 float const verticalAngle = std::atan2(py, pz);
 RPDPos const& pos = RUN3_RPD_POSITION.at(side);
 float const dz = pos.z - z0;
 float const xAtRPDATLAS = std::tan(horizontalAngle)*dz; // in ATLAS coordinates
 float const yAtRPDATLAS = std::tan(verticalAngle)*dz; // in ATLAS coordinates
 float const xAtRPD = xAtRPDATLAS - pos.x; // RPD coordinates
 float const yAtRPD = yAtRPDATLAS - pos.y; // RPD coordinates
 return {xAtRPD, yAtRPD};
}

std::function<ROOT::VecOps::RVec<float>(
  ROOT::VecOps::RVec<ParticleType> const& particleTypes,
  ROOT::VecOps::RVec<float> const& particleX,
  ROOT::VecOps::RVec<float> const& particleY,
  ROOT::VecOps::RVec<float> const& particleZ,
  ROOT::VecOps::RVec<float> const& particleXMomentum,
  ROOT::VecOps::RVec<float> const& particleYMomentum,
  ROOT::VecOps::RVec<float> const& particleZMomentum
)> getProjectionFunc(unsigned int const side) {
  return [side] (
    ROOT::VecOps::RVec<ParticleType> const& particleTypes,
    ROOT::VecOps::RVec<float> const& particleX,
    ROOT::VecOps::RVec<float> const& particleY,
    ROOT::VecOps::RVec<float> const& particleZ,
    ROOT::VecOps::RVec<float> const& particleXMomentum,
    ROOT::VecOps::RVec<float> const& particleYMomentum,
    ROOT::VecOps::RVec<float> const& particleZMomentum
  ) {
    unsigned int const index = getParticleSelectorFunc(particleTypes, side);
    auto const [xAtRPD, yAtRPD] = project(
      side,
      particleX.at(index),
      particleY.at(index),
      particleZ.at(index),
      particleXMomentum.at(index),
      particleYMomentum.at(index),
      particleZMomentum.at(index)
    );
    std::vector<float> posAtRPPD = {xAtRPD, yAtRPD};
    return posAtRPPD;
  };
}

/**
 * @brief 
 */
inline void plot_projection() {
  // ROOT will use multiple threads where possible
  // this is great for large files, probably not much benefit for 100k events, though
  ROOT::EnableImplicitMT();

  ROOT::RDataFrame dataframe("zdcTree", SIM_FILE_PATH);

  std::array<ROOT::RDF::RResultPtr< ::TH2D>, 2> hProjection {};

  for (auto const& side : SIDES) {
    // define new "branches" for em and bran energies, unpacking them from the vector branch
    auto dataframeUnpacked = dataframe.Define(
      "particle_types", classifyParticles, {particleBranches::pz}
    ).Define(
      "projection", getProjectionFunc(side), {
        "particle_types",
        particleBranches::x,
        particleBranches::y,
        particleBranches::z,
        particleBranches::px,
        particleBranches::py,
        particleBranches::pz,
      }
    ).Define(
      "projectionx", getVectorUnpackerFunc(0), {"projection"}
    ).Define(
      "projectiony", getVectorUnpackerFunc(1), {"projection"}
    );
    // make histogram from these new branches
    hProjection.at(side) = dataframeUnpacked.Histo2D<float, float>(
      {Form("side%c_projection", getSideLabel(side)), ";x [mm];y [mm];Count", BINS(axis::xAtRPD.withBins(256)), BINS(axis::yAtRPD.withBins(256))},
      "projectionx", "projectiony"
    );
  }

  TFile* plotFile = TFile::Open(OUT_FILE_PATH.c_str(), "RECREATE");
  for (auto const& side : SIDES) {
    hProjection.at(side)->Write();
  }
  plotFile->Close();
}
