#ifndef CORRECTIONSF_H
#define CORRECTIONSF_H

// Includes required by the CorrectionSF class
#include <vector>
#include <map>
#include <string>
#include <stdexcept>    // For std::runtime_error
#include <iostream>     // For std::cerr
#include <algorithm>    // For std::upper_bound
#include <iterator>     // For std::distance

// You must include your JSON library header here.
// This example assumes you are using nlohmann/json.
// Adjust the path as needed.
#include "nlohmann/json.hpp" 
using json = nlohmann::json; // Convenience alias

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Helper class to load and look up corrections from the JSON file
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CorrectionSF {
public:
    CorrectionSF() {}

    // Load correction data from the "data" node of a JSON correction object
    void load(const json& dataNode) {
        try {
            // Assumes structure: binning (eta) -> binning (pt) -> category (sf_type)
            std::string eta_input = dataNode["input"]; // e.g., "abseta1"
            auto eta_edges_json = dataNode["edges"];
            auto eta_content_json = dataNode["content"];

            // Copy eta edges
            m_eta_bins.clear();
            for (const auto& edge : eta_edges_json) {
                m_eta_bins.push_back(edge.get<double>());
            }

            m_pt_bins.resize(m_eta_bins.size() - 1);
            m_values.resize(m_eta_bins.size() - 1);

            // Loop over eta bins
            for (size_t i = 0; i < eta_content_json.size(); ++i) {
                const auto& pt_bin_node = eta_content_json[i];
                std::string pt_input = pt_bin_node["input"]; // e.g., "pt1" or "pt2"
                auto pt_edges_json = pt_bin_node["edges"];
                auto pt_content_json = pt_bin_node["content"];

                // Copy pt edges for this eta bin
                m_pt_bins[i].clear();
                for (const auto& edge : pt_edges_json) {
                    m_pt_bins[i].push_back(edge.get<double>());
                }

                m_values[i].resize(m_pt_bins[i].size() - 1);

                // Loop over pt bins
                for (size_t j = 0; j < pt_content_json.size(); ++j) {
                    const auto& category_node = pt_content_json[j];
                    std::string sf_input = category_node["input"]; // "scale_factors"
                    auto sf_content_json = category_node["content"];

                    // Loop over scale factor types (nominal, stat, syst, ...)
                    for (const auto& sf_entry : sf_content_json) {
                        std::string key = sf_entry["key"];
                        double value = sf_entry["value"];
                        m_values[i][j][key] = value;
                    }
                }
            }
        } catch (std::exception& e) {
            std::cerr << "Error parsing JSON correction data: " << e.what() << std::endl;
            throw std::runtime_error("Failed to load correction from JSON.");
        }
    }

    // Get a scale factor value
    double getValue(double eta, double pt, const std::string& key = "nominal") const {
        if (m_eta_bins.empty()) {
            std::cerr << "Error: CorrectionSF is not initialized." << std::endl;
            return 1.0;
        }

        // Find eta bin (handles underflow/overflow by clamping to first/last bin)
        auto it_eta = std::upper_bound(m_eta_bins.begin(), m_eta_bins.end(), eta);
        int eta_idx = -1;

        if (it_eta == m_eta_bins.begin()) { // Underflow
            eta_idx = 0;
        } else if (it_eta == m_eta_bins.end()) { // Overflow
            eta_idx = m_eta_bins.size() - 2; // Last valid bin index
        } else {
            eta_idx = std::distance(m_eta_bins.begin(), it_eta) - 1;
        }

        if (eta_idx < 0 || eta_idx >= m_values.size()) {
             std::cerr << "Error: Invalid eta index " << eta_idx << " for eta " << eta << std::endl;
             return 1.0;
        }

        // Find pt bin for the corresponding eta bin (handles underflow/overflow)
        const auto& pt_bins_for_eta = m_pt_bins[eta_idx];
        auto it_pt = std::upper_bound(pt_bins_for_eta.begin(), pt_bins_for_eta.end(), pt);
        int pt_idx = -1;

        if (it_pt == pt_bins_for_eta.begin()) { // Underflow
            pt_idx = 0;
        } else if (it_pt == pt_bins_for_eta.end()) { // Overflow
            pt_idx = pt_bins_for_eta.size() - 2; // Last valid bin index
        } else {
            pt_idx = std::distance(pt_bins_for_eta.begin(), it_pt) - 1;
        }

        if (pt_idx < 0 || pt_idx >= m_values[eta_idx].size()) {
            std::cerr << "Error: Invalid pt index " << pt_idx << " for pt " << pt << " in eta bin " << eta_idx << std::endl;
            return 1.0;
        }

        // Find the scale factor by key
        const auto& sf_map = m_values[eta_idx][pt_idx];
        auto it_sf = sf_map.find(key);
        if (it_sf == sf_map.end()) {
            std::cerr << "Error: Could not find scale factor key '" << key << "'" << std::endl;
            // Fallback to nominal if the requested key isn't found (e.g., asking for 'syst' when only 'nominal' exists)
            auto it_nominal = sf_map.find("nominal");
            if (it_nominal != sf_map.end()) return it_nominal->second;
            return 1.0;
        }

        return it_sf->second;
    }

private:
    std::vector<double> m_eta_bins;
    // Each eta bin can have its own set of pt bins
    std::vector<std::vector<double>> m_pt_bins; 
    // [eta_idx][pt_idx] -> map<string, double>
    std::vector<std::vector<std::map<std::string, double>>> m_values; 
};
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#endif // CORRECTIONSF_H
