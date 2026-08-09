#ifndef JERPROVIDER_H
#define JERPROVIDER_H

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>
#include "TRandom3.h"
#include "TMath.h"

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

    struct SFUncRecord {
        float etaMin, etaMax;
        std::vector<float> pts;
        std::vector<float> uncUps;
        std::vector<float> uncDowns;
    };

    std::vector<SFRecord> sfRecords;
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
        if (!file.is_open()) { std::cerr << "JERProvider Error: Cannot open SF file " << filename << std::endl; return; }
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '{' || line[0] == '[') continue;
            std::stringstream ss(line);
            
            // Format: EtaMin EtaMax nParams PtMin PtMax p0 p1 p2 p3 p4 p5
            float etaMin, etaMax, ptMin, ptMax, p0, p1, p2, p3, p4, p5;
            int nParams;
            
            if (ss >> etaMin >> etaMax >> nParams >> ptMin >> ptMax >> p0 >> p1 >> p2 >> p3 >> p4 >> p5) {
                SFRecord r = {etaMin, etaMax, ptMin, ptMax, p0, p1, p2, p3, p4, p5};
                sfRecords.push_back(r);
            }
        }
        std::cout << "JERProvider: Loaded " << sfRecords.size() << " SF nominal records." << std::endl;
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

        // Apply absolute shift
        float final_sf = (syst_var == 1) ? (nominal_sf + unc) : (nominal_sf - unc);
        
        return final_sf;
    }

    // Main Hybrid Smearing Function
    // (jetRho is kept in the signature to prevent breaking existing main macros, but it is ignored)
    float GetSmearedPt(float pt_reco, float eta_reco, float phi_reco, float jetRho,
                       const std::vector<float>& gen_pts, const std::vector<float>& gen_etas, const std::vector<float>& gen_phis,
                       int syst_var = 0) {
        
        (void)jetRho; // Suppress unused variable warning

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

            // Matching: dR < 0.1 (adjust if using AK4 instead of AK2) and dPt < 3 * sigma
            if (dR < 0.1 && std::abs(pt_reco - gen_pts[i]) < 3.0f * resolution * pt_reco) {
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
                rand.SetSeed(1 + std::abs((int)(phi_reco * 100000)));
                float width = resolution * std::sqrt(std::max(sf * sf - 1.0f, 0.0f));
                float smear_factor = rand.Gaus(0, width);
                pt_smeared = std::max(0.0f, pt_reco * (1.0f + smear_factor));
            }
        }
        return pt_smeared;
    }
};

#endif
