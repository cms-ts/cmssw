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
// JER PROVIDER CLASS (Updated for Autumn18 Resolution Files)
// ---------------------------------------------------------------------------------------------------------
class JERProvider {
public:
    struct ResRecord {
        float etaMin, etaMax;
        float rhoMin, rhoMax;
        float ptMin, ptMax;
        float p0, p1, p2, p3; // Parameters for the resolution formula
    };

    struct SFRecord {
        float etaMin, etaMax;
        float ptMin, ptMax;
        float sf, sf_down, sf_up;
    };

    std::vector<SFRecord> sfRecords;
    std::vector<ResRecord> resRecords;
    TRandom3 rand;

    JERProvider(int seed = 12345) { rand.SetSeed(seed); }

    // Load Scale Factor text file
    // Load Scale Factor text file
    void LoadSF(std::string filename) {
      std::ifstream file(filename);
      if (!file.is_open()) { std::cerr << "JERProvider Error: Cannot open SF file " << filename << std::endl; return; }
      std::string line;
      while (std::getline(file, line)) {
        // Skip header (starts with {)
        if (line.empty() || line[0] == '{') continue;

        std::stringstream ss(line);
        std::vector<float> tokens;
        float temp;
        while (ss >> temp) tokens.push_back(temp);

        SFRecord r;

        // CASE 1: Standard 8-column format (Eta + Pt bins)
        // Format: EtaMin EtaMax PtMin PtMax nParams SF SFDown SFUp
        if (tokens.size() >= 8) {
          r.etaMin  = tokens[0];
          r.etaMax  = tokens[1];
          r.ptMin   = tokens[2];
          r.ptMax   = tokens[3];
          // tokens[4] is nParams (usually 3), skip
          r.sf      = tokens[5];
          r.sf_down = tokens[6];
          r.sf_up   = tokens[7];
          sfRecords.push_back(r);
        }
        // CASE 2: 6-column format (Eta bins only, Pt independent)
        // Format: EtaMin EtaMax nParams SF SFDown SFUp
        else if (tokens.size() >= 6) {
          r.etaMin  = tokens[0];
          r.etaMax  = tokens[1];
          r.ptMin   = 0.0;    // Default: valid for all Pt
          r.ptMax   = 99999.0;// Default: valid for all Pt
          // tokens[2] is nParams, skip
          r.sf      = tokens[3];
          r.sf_down = tokens[4];
          r.sf_up   = tokens[5];
          sfRecords.push_back(r);
        }
      }
      std::cout << "JERProvider: Loaded " << sfRecords.size() << " SF records." << std::endl;
    }

    // Load Resolution text file
    void LoadResolution(std::string filename) {
      std::ifstream file(filename);
      if (!file.is_open()) { std::cerr << "JERProvider Error: Cannot open Resolution file " << filename << std::endl; return; }
      std::string line;
      while (std::getline(file, line)) {
        if (line.empty() || line[0] == '{') continue; // Skip header
        std::stringstream ss(line);

        // Resolution File Format
        // Format: EtaMin EtaMax RhoMin RhoMax Unused PtMin PtMax p0 p1 p2 p3
        // Example: -4.7 -3.2 0 7.2 6 15 3000 -0.8393 1.056 0.1108 -1.238
        float etaMin, etaMax, rhoMin, rhoMax, ptMin, ptMax;
        int unused; // the '6'
        float p0, p1, p2, p3;

        if (!(ss >> etaMin >> etaMax >> rhoMin >> rhoMax >> unused >> ptMin >> ptMax >> p0 >> p1 >> p2 >> p3)) continue;

        ResRecord r;
        r.etaMin = etaMin; r.etaMax = etaMax;
        r.rhoMin = rhoMin; r.rhoMax = rhoMax;
        r.ptMin = ptMin;   r.ptMax = ptMax;
        r.p0 = p0; r.p1 = p1; r.p2 = p2; r.p3 = p3;
        resRecords.push_back(r);
      }
      std::cout << "JERProvider: Loaded " << resRecords.size() << " Resolution records." << std::endl;
    }

    // Calculate Resolution (sigma_pt / pt)
    // Uses formula: sqrt(p0*|p0|/pt^2 + p1^2 * pt^p3 + p2^2)
    float GetResolution(float pt, float eta, float rho) {
      for (const auto& r : resRecords) {
        // Check eta
        if (eta >= r.etaMin && eta < r.etaMax) {
          // Check Rho:
          // CRITICAL:  If rho > rhoMax of the bin, we usually want the LAST bin (highest rho),
          // not to fail. The file goes up to rho=90. If your HI event has rho=200, use the rho=90 bin.
          // We implement this by checking if rho matches, OR if this is the last rho bin and rho is larger.
          bool rhoMatch = (rho >= r.rhoMin && rho < r.rhoMax);
          // Simple clamp check: if rho is huge, accept the record if it covers the highest defined rho (usually ~90)
          if (!rhoMatch && rho >= r.rhoMax && r.rhoMax >= 90.0) {
            rhoMatch = true;
          }
          if (rhoMatch) {
            // Constrain Pt to valid range for stability
            float pt_eval = std::max(r.ptMin, std::min(pt, r.ptMax));

            float term1 = (r.p0 * std::abs(r.p0)) / (pt_eval * pt_eval);
            float term2 = (r.p1 * r.p1) * std::pow(pt_eval, r.p3);
            float term3 = (r.p2 * r.p2);

            float res_sq = term1 + term2 + term3;
            float final_res = (res_sq > 0) ? std::sqrt(res_sq) : 0.0;
            // --- ADD THIS SECTION FOR DEBUGGING ---
/*            std::cout << " [JER DEBUG] Input(Pt=" << pt << ", Eta=" << eta << ", Rho=" << rho << ") "
                      << "MATCHED Bin (Rho=[" << r.rhoMin << "," << r.rhoMax << "]"
                      << " Pt=[" << r.ptMin << "," << r.ptMax << "]"
                      << " Eta=[" << r.etaMin << "," << r.etaMax << "]) "
                      << "--> Res=" << final_res << std::endl;
            // --- DEBUGGING END ---
*/
            return final_res;
          }
        }
      }
      return 0.1; // Fallback if bin not found (shouldn't happen if file covers all eta/rho)
    }

    // Get Scale Factor (SF)
    float GetSF(float pt, float eta, int syst_var = 0) {
      for (const auto& r : sfRecords) {
        if (eta >= r.etaMin && eta < r.etaMax && pt >= r.ptMin && pt < r.ptMax) {
          if (syst_var == 0) return r.sf; // Nominal
          if (syst_var == -1) return r.sf_down; // Down
          if (syst_var == 1) return r.sf_up; // Up
        }
      }
      // If pt > max defined (usually 7000), try to find the last bin for this eta
      if (pt > 7000) {
        for (const auto& r : sfRecords) {
          if (eta >= r.etaMin && eta < r.etaMax && r.ptMax >= 7000) {
            if (syst_var == 0) return r.sf;
          }
        }
      }
      return 1.0;
    }

    // Main Hybrid Smearing Function
    float GetSmearedPt(float pt_reco, float eta_reco, float phi_reco, float rho,
                       const std::vector<float>& gen_pts, const std::vector<float>& gen_etas, const std::vector<float>& gen_phis,
                       int syst_var = 0) {

      float resolution = GetResolution(pt_reco, eta_reco, rho);
      float sf = GetSF(pt_reco, eta_reco, syst_var);

      // 1. Find closest Gen Jet
      int best_match_idx = -1;
      float min_dR = 100.0;
      for (size_t i = 0; i < gen_pts.size(); ++i) {
        float dEta = fabs(eta_reco - gen_etas[i]);
        float dPhi = fabs(phi_reco - gen_phis[i]);
        if (dPhi > TMath::Pi()) dPhi = 2*TMath::Pi() - dPhi;
        float dR = sqrt(dEta*dEta + dPhi*dPhi);

        // --- ADD THIS BLOCK for debugging ---
/*        if (dR < 0.1) {
          float pt_diff = fabs(pt_reco - gen_pts[i]);
          float threshold = 3 * resolution * pt_reco;
          std::cout << " [JER CHECK] RecoPt: " << pt_reco
                    << " | GenPt: " << gen_pts[i]
                    << " | dR: " << dR
                    << " | Res: " << resolution
                    << " | Diff: " << pt_diff
                    << " | MaxDiff: " << threshold
                    << " | PASS: " << (pt_diff < threshold ? "YES" : "NO") << std::endl;
        }
*/        // ---------------------

        // Standard Matching requirements
        // dR < R_cone/2 (0.1 for AK2) AND |pt_reco - pt_gen| < 3 * sigma * pt_reco
        if (dR < 0.1 && fabs(pt_reco - gen_pts[i]) < 3 * resolution * pt_reco) {
          if (dR < min_dR) {
            min_dR = dR;
            best_match_idx = i;
          }
        }
      }

      float pt_smeared = pt_reco;

      // 2. Hybrid Method Implementation
      if (best_match_idx != -1) {
        // --- SCALING METHOD (Gen Match Found) ---
        // Formula: pT_new = pT_gen + SF * (pT_reco - pT_gen)
        float pt_gen = gen_pts[best_match_idx];
        pt_smeared = std::max(0.f, pt_gen + sf * (pt_reco - pt_gen));
      } else {
        // --- STOCHASTIC SMEARING (No Gen Match) ---
        //
        // Only smear if SF > 1 (degrade resolution)
        if (sf > 1.0) {
          // Seed based on phi for reproducibility
          // Take absolute value and add 1 to guarantee seed >= 1
          rand.SetSeed( 1 + std::abs((int)(phi_reco * 100000)) );

          // Width = sigma_JER * sqrt(SF^2 - 1)
          float width = resolution * sqrt(std::max(sf*sf - 1.0f, 0.f));
          //  Smear: pT_new = pT_reco * (1 + Gaus(0, width))
          float smear_factor = rand.Gaus(0, width);
          pt_smeared = pt_reco * (1.0f + smear_factor);
          pt_smeared = std::max(0.f, pt_smeared);
        }
      }
      return pt_smeared;
    }
};

#endif
