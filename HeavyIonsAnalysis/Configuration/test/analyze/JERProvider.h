#ifndef JERPROVIDER_H
#define JERPROVIDER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>
#include "TMath.h"
#include <stdexcept>

// ---------------------------------------------------------------------------------------------------------
// JER PROVIDER CLASS (Updated for Summer23/2026 Run 3 Continuous Calibration Formats)
// ---------------------------------------------------------------------------------------------------------
class JERProvider {
public:
    struct ResRecord {
        float etaMin, etaMax;
        float ptMin, ptMax;
        float p0, p1, p2, p3; // Parameters for the resolution formula
    };

    struct SFRecord {
        float etaMin, etaMax;
        float ptMin, ptMax;
        float p0, p1, p2, p3, p4, p5; // Parameters for the continuous SF formula
    };

    struct BinnedSFRecord {
    float etaMin, etaMax;
    float ptMin, ptMax;
    float sf;
    };

    struct SFUncRecord {
        float etaMin, etaMax;
        std::vector<float> pts;
        std::vector<float> uncUps;
        std::vector<float> uncDowns;
    };

    std::vector<SFRecord> sfRecords;
    std::vector<BinnedSFRecord> binnedSFRecords;
    std::vector<ResRecord> resRecords;
    std::vector<SFUncRecord> sfUncRecords;
    TRandom3 rand;

    JERProvider(int seed = 12345) { rand.SetSeed(seed); }

    // 1. Load Resolution text file (No Rho dependence)
    void LoadResolution(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) { std::cerr << "JERProvider Error: Cannot open Resolution file " << filename << std::endl; return; }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '{' || line[0] == '[') continue; // Skip headers
            std::stringstream ss(line);
            
            // Format: EtaMin EtaMax nParams PtMin PtMax p0 p1 p2 p3
            float etaMin, etaMax, ptMin, ptMax, p0, p1, p2, p3;
            int nParams;
            
            if (ss >> etaMin >> etaMax >> nParams >> ptMin >> ptMax >> p0 >> p1 >> p2 >> p3) {
                ResRecord r = {etaMin, etaMax, ptMin, ptMax, p0, p1, p2, p3};
                resRecords.push_back(r);
            }
        }
        std::cout << "JERProvider: Loaded " << resRecords.size() << " Resolution records." << std::endl;
    }

    // 2. Load Scale Factor text file (Continuous Formula)
    void LoadSF(std::string filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Cannot open JER SF file: " + filename);
    }

    sfRecords.clear();
    binnedSFRecords.clear();

    std::string line;
    while (std::getline(file, line)) {
        const auto first = line.find_first_not_of(" \t\r");
        if (first == std::string::npos || line[first] == '{' ||
            line[first] == '[' || line[first] == '#') continue;

        std::stringstream ss(line);
        std::vector<float> fields;
        for (float value; ss >> value;) {
            fields.push_back(value);
        }

        if (fields.size() == 8 && fields[4] == 3) {
            binnedSFRecords.push_back({
                fields[0], fields[1], fields[2], fields[3], fields[7]
            });
        } else if (fields.size() == 11 && fields[2] == 8) {
            sfRecords.push_back({
                fields[0], fields[1], fields[3], fields[4],
                fields[5], fields[6], fields[7], fields[8], fields[9], fields[10]
            });
        } else {
            throw std::runtime_error("Unexpected JER SF row in " + filename + ": " + line);
        }
    }

    if (binnedSFRecords.empty() && sfRecords.empty()) {
        throw std::runtime_error("No nominal JER SF records in " + filename);
    }

    std::cout << "JERProvider: Loaded " << binnedSFRecords.size()
              << " binned and " << sfRecords.size() << " formula SF records.\n";
    }

    // 3. Load Scale Factor Uncertainty text file
    void LoadSFUncertainty(std::string filename) {
        std::ifstream file(filename);
        if (!file.is_open()) { std::cerr << "JERProvider Error: Cannot open SF Unc file " << filename << std::endl; return; }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '{' || line[0] == '[') continue;
            std::stringstream ss(line);
            
            float etaMin, etaMax;
            int nParams; // Total number of following items (N points * 3)
            
            if (ss >> etaMin >> etaMax >> nParams) {
                SFUncRecord r;
                r.etaMin = etaMin;
                r.etaMax = etaMax;
                int nPoints = nParams / 3;
                
                for (int i = 0; i < nPoints; ++i) {
                    float pt, up, down;
                    if (ss >> pt >> up >> down) {
                        r.pts.push_back(pt);
                        r.uncUps.push_back(up);
                        r.uncDowns.push_back(down);
                    }
                }
                sfUncRecords.push_back(r);
            }
        }
        std::cout << "JERProvider: Loaded " << sfUncRecords.size() << " SF uncertainty records." << std::endl;
    }

    // Calculate Resolution (sigma_pt / pt) using Run 3 Formula
    float GetResolution(float pt, float eta) {
        for (const auto& r : resRecords) {
            if (eta >= r.etaMin && eta < r.etaMax) {
                float pt_eval = std::max(r.ptMin, std::min(pt, r.ptMax));
                
                // Formula: sqrt(p0^2 / pt^2 + p1^2 * pt^p3 + p2^2)
                float term1 = (r.p0 * r.p0) / (pt_eval * pt_eval);
                float term2 = (r.p1 * r.p1) * std::pow(pt_eval, r.p3);
                float term3 = (r.p2 * r.p2);
                
                float res_sq = term1 + term2 + term3;
                return (res_sq > 0.0f) ? std::sqrt(res_sq) : 0.0f;
            }
        }
        return 0.1f; // Fallback
    }

    // Calculate Nominal SF and apply uncertainties
    float GetSF(float pt, float eta, int syst_var = 0) {
        float nominal_sf = 1.0f;

        for (const auto& r : binnedSFRecords) {
          if (eta >= r.etaMin && eta < r.etaMax &&
          pt >= r.ptMin && pt < r.ptMax) {
            nominal_sf = r.sf;
            break;
          }
        }
        
        // 1. Calculate Nominal SF
        for (const auto& r : sfRecords) {
            if (eta >= r.etaMin && eta < r.etaMax) {
                float pt_eval = std::max(r.ptMin, std::min(pt, r.ptMax));
                
                // Numerator: sqrt(p0*|p0|/pt^2 + p1^2/pt + p2^2)
                float num1 = (r.p0 * std::abs(r.p0)) / (pt_eval * pt_eval);
                float num2 = (r.p1 * r.p1) / pt_eval;
                float num3 = (r.p2 * r.p2);
                float num = std::sqrt(std::max(0.0f, num1 + num2 + num3));
                
                // Denominator: sqrt(p3*|p3|/pt^2 + p4^2/pt + p5^2)
                float den1 = (r.p3 * std::abs(r.p3)) / (pt_eval * pt_eval);
                float den2 = (r.p4 * r.p4) / pt_eval;
                float den3 = (r.p5 * r.p5);
                float den = std::sqrt(std::max(0.0f, den1 + den2 + den3));
                
                if (den > 0.0f) nominal_sf = num / den;
                break;
            }
        }

        // 2. Return Nominal if no systematic variation requested
        if (syst_var == 0) return std::max(1.0f, nominal_sf);

        // 3. Find Uncertainty Bin (No Interpolation)
        float unc = 0.0f;
        for (const auto& r : sfUncRecords) {
            if (eta >= r.etaMin && eta < r.etaMax) {
                if (r.pts.empty()) break;
                
                // Iterate through the pT bins to find where the jet falls
                for (size_t i = 0; i < r.pts.size(); ++i) {
                    // If it is the very last bin, or the jet pT is less than the NEXT bin's edge
                    if (i == r.pts.size() - 1 || pt < r.pts[i+1]) {
                        unc = (syst_var == 1) ? r.uncUps[i] : r.uncDowns[i];
                        break;
                    }
                }
                break;
            }
        }

        // Apply relative multiplicative shift (Official CMS Recommendation)
        float final_sf = nominal_sf;
        if (syst_var == 1)  final_sf = nominal_sf * (1.0f + unc);
        if (syst_var == -1) final_sf = nominal_sf * (1.0f - unc);
        
        return final_sf;
    }

    // Main Hybrid Smearing Function (Now completely stateless & reproducible)
    float GetSmearedPt(float pt_reco, float eta_reco, float phi_reco, float jetRho,
                       const std::vector<float>& gen_pts, const std::vector<float>& gen_etas, const std::vector<float>& gen_phis,
                       int syst_var = 0, unsigned long long eventID = 0) { // <--- Added eventID
        
        float resolution = GetResolution(pt_reco, eta_reco);
        float sf = GetSF(pt_reco, eta_reco, syst_var);

        // 1. Find closest Gen Jet
        int best_match_idx = -1;
        float min_dR = 100.0f;
        for (size_t i = 0; i < gen_pts.size(); ++i) {
            float dEta = std::abs(eta_reco - gen_etas[i]);
            float dPhi = std::abs(phi_reco - gen_phis[i]);
            if (dPhi > TMath::Pi()) dPhi = 2 * TMath::Pi() - dPhi;
            float dR = std::sqrt(dEta * dEta + dPhi * dPhi);

            // Matching: dR < 0.1 (AK2 specific) and dPt < 3 * sigma
            if (dR < 0.1f && std::abs(pt_reco - gen_pts[i]) < 3.0f * resolution * pt_reco) {
                if (dR < min_dR) {
                    min_dR = dR;
                    best_match_idx = i;
                }
            }
        }

        float pt_smeared = pt_reco;

        // 2. Hybrid Method Implementation
        if (best_match_idx != -1) {
            // Scaling Method (Gen Match Found)
            float pt_gen = gen_pts[best_match_idx];
            pt_smeared = std::max(0.0f, pt_gen + sf * (pt_reco - pt_gen));
        } else {
            // Stochastic Smearing (No Gen Match)
            if (sf > 1.0f) {
                // --- STATELESS HASH PRNG ---
                // a. Create a unique, deterministic seed using CMS recommended entropy sources
                std::size_t seed = 0;
                auto hash_combine = [&seed](std::size_t hash_val) {
                    seed ^= hash_val + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                };
                
                hash_combine(std::hash<unsigned long long>{}(eventID));
                hash_combine(std::hash<float>{}(pt_reco));
                hash_combine(std::hash<float>{}(eta_reco));
                hash_combine(std::hash<float>{}(jetRho));

                // b. Initialize a lightweight local random engine
                // (minstd_rand is an LCG, effectively zero setup cost for per-jet instantiation)
                std::minstd_rand lcg(static_cast<uint32_t>(seed));
                
                // c. Draw from a Gaussian distribution
                float width = resolution * std::sqrt(std::max(sf * sf - 1.0f, 0.0f));
                std::normal_distribution<float> gauss_dist(0.0f, width);
                
                float smear_factor = gauss_dist(lcg);
                pt_smeared = std::max(0.0f, pt_reco * (1.0f + smear_factor));
            }
        }
        return pt_smeared;
    }
};

#endif
