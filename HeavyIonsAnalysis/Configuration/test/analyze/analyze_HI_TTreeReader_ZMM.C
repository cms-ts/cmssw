// C++ includes
#include <iostream>   // Input/output stream. Needed for std::cout.
#include <vector>     // For std::vector
#include <algorithm>
#include "./MixEvSub/binning_config.h"
#include <string>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TLeaf.h"
#include "TChain.h"
#include "TLorentzVector.h"
#include "TCanvas.h"
#include "TLatex.h"
#include "TDirectory.h"

#include <glob.h>
#include "TTreeReader.h"
#include "TTreeReaderValue.h"
#include "TTreeReaderArray.h"

#include "JetCorrector.h" // for JEC
#include "JetUncertainty.h" // for up and down var on JEC
#include "JERProvider.h"     // Include JER Provider
#include "MC_samples.h" // Include the header file for MC samples

#include <fstream>      // For std::ifstream
#include <map>          // For std::map
#include <stdexcept>    // For std::runtime_error
#include <limits>       // For std::numeric_limits
#include <nlohmann/json.hpp> // For JSON parsing
#include "CorrectionSF.h"
#include <cstdlib>

// Use the nlohmann::json namespace
using json = nlohmann::json;

using namespace std;

// ---------------------------------------------------------------------------------------------------------
// Function to find the total recorded lumi from the summary line
// ---------------------------------------------------------------------------------------------------------
double getLumiFromSummary(const std::string& filename) {
  std::ifstream file(filename);
  std::string line;
  double totalRecorded = 0.0;
  std::string searchKey = "#Sum recorded :";

  if (!file.is_open()) {
    std::cerr << "Error: Could not open file " << filename << std::endl;
    return 0.0;
  }

  while (std::getline(file, line)) {
    // Check if the line starts with "#Sum recorded :"
    if (line.find(searchKey) != std::string::npos) {
      // Find the position of the colon to split the key from the value
      size_t colonPos = line.find(':');
      if (colonPos != std::string::npos) {
        // Extract everything after the colon
        std::string numberString = line.substr(colonPos + 1);
        // std::stod automatically handles leading whitespace
        try {
          totalRecorded = std::stod(numberString);
        } catch (const std::exception& e) {
          std::cerr << "Error parsing number: " << numberString << std::endl;
        }
        // Once found, we can stop reading the file
        break;
      }
    }
  }
  file.close();
  // Fallback warning if 0.0 is returned (optional)
  if (totalRecorded == 0.0) {
    std::cout << "Warning: Luminosity not found or zero in " << filename << std::endl;
  }
  return totalRecorded;
}

double RelativePhi(double phi_1,double phi_2) {
  double d_phi =  abs(phi_1 - phi_2);
  if (d_phi > acos(-1)) d_phi = 2*acos(-1) - d_phi;
  return d_phi;
}

/**
 * @brief Mimics the CentralityBinProducer logic to find the hiBin from hiHF.
 * @param hiHF The transverse energy sum in the HF calorimeter.
 * @param table The centrality table (must be sorted in descending HF order).
 * @return int The corresponding centrality bin (0-199).
 */
int getHiBin(float hiHF, const std::vector<double>& table) {
    // The table is sorted from 0% (high HF) to 100% (low HF).
    // It has N+1 entries (e.g., 201) for N bins (e.g., 200).
    int nbins = table.size() - 1;
    int bin = nbins; // Default to the last bin

    for (unsigned int i = 0; i < nbins; ++i) {
        // Find the first bin edge that the hiHF is *larger* than.
        if (hiHF > table[i]) {
            bin = i-1;
            break;
        }
    }
    // Logic from CentralityBinProducer.cc:
    // i=0: hiHF >= table[0] (e.g. 8171)? bin=0. (0-0.5%)
    // i=59: hiHF >= table[59] (e.g. 1865)? bin=59. (29.5-30%)
    // ...
    // i=200: hiHF >= table[200] (e.g. 0)? bin=200. (100%+)
    if (bin == -1) bin = 0;
    return bin;
}

//To run, root -l analyze_HI_TTreeReader_ZMM.C
//Default isData, for MC root -l 'analyze_HI_TTreeReader_ZMM.C(false, 3)'
void analyze_HI_TTreeReader_ZMM(const char * sample_name = "data", int weight_phase = 1, int systFlag = 0) {

  // --- Centrality Tables from TWiki ---
  // (Note: The last value 8171.19 is the 100% boundary)
  std::vector<double> centrality_table_nominal_asc = {
    0, 10.3417, 11.0768, 11.7884, 12.5014, 13.223, 13.979, 14.7243, 15.4805, 16.2881, 17.0953, 17.9161, 18.7604, 19.6304, 20.551, 21.5208, 22.5138, 23.5369, 24.5918, 25.6714, 26.8101, 28.006, 29.2436, 30.5117, 31.8466, 33.228, 34.6998, 36.2222, 37.8447, 39.5591, 41.3376, 43.1468, 45.0235, 47.0435, 49.0904, 51.2926, 53.6121, 56.0542, 58.5133, 61.1585, 63.8113, 66.639, 69.5748, 72.568, 75.676, 78.9026, 82.3018, 85.7734, 89.444, 93.2175, 97.2133, 101.253, 105.319, 109.546, 113.867, 118.346, 123.019, 127.884, 132.907, 138.133, 143.553, 149.142, 154.94, 160.848, 166.99, 173.359, 179.998, 186.842, 193.842, 201.117, 208.532, 216.211, 224.158, 232.3, 240.736, 249.435, 258.302, 267.463, 277.014, 286.845, 296.83, 307.189, 317.783, 328.756, 339.992, 351.395, 363.16, 375.455, 387.794, 400.501, 413.561, 426.97, 440.651, 454.496, 468.886, 483.371, 498.4, 513.754, 529.457, 545.441, 561.988, 578.779, 595.858, 613.597, 631.446, 649.849, 668.467, 687.351, 706.767, 726.71, 747.018, 767.66, 788.701, 810.28, 832.266, 854.725, 877.463, 900.464, 924.078, 948.465, 973.01, 998.014, 1023.62, 1049.62, 1075.96, 1102.82, 1129.95, 1157.78, 1186.03, 1215.15, 1244.29, 1274.23, 1304.67, 1335.36, 1366.46, 1398.45, 1431.19, 1464.02, 1497.65, 1531.96, 1566.77, 1602.1, 1637.92, 1674.27, 1711.1, 1749.07, 1787.21, 1826.02, 1865.25, 1905.66, 1946.56, 1987.95, 2030.81, 2073.58, 2117.19, 2161.79, 2206.96, 2252.79, 2300.01, 2347.25, 2395.35, 2444.79, 2494.51, 2544.84, 2596.27, 2649.26, 2703.27, 2758.12, 2813.87, 2870.23, 2927.2, 2985.36, 3045.09, 3105.31, 3166.48, 3229.14, 3293.26, 3359.1, 3425.63, 3493.44, 3562.41, 3633.68, 3706.37, 3780.38, 3856.7, 3934.4, 4013.53, 4095.4, 4178.3, 4263.29, 4350.7, 4440.56, 4532.84, 4628.66, 4727.32, 4827.83, 4933.06, 5042.94, 5161.8, 5305.56, 8171.19
  };

  std::vector<double> centrality_table_up_asc = {
    0, 10.3379, 11.0725, 11.7802, 12.4919, 13.21, 13.9601, 14.7025, 15.4608, 16.2614, 17.0642, 17.8839, 18.7243, 19.5915, 20.501, 21.4676, 22.4592, 23.4754, 24.5231, 25.597, 26.7278, 27.914, 29.1421, 30.4015, 31.7342, 33.0996, 34.549, 36.0668, 37.6735, 39.3801, 41.1438, 42.9432, 44.8014, 46.7956, 48.853, 51.0064, 53.3073, 55.7264, 58.1748, 60.78, 63.436, 66.2411, 69.112, 72.1054, 75.1783, 78.3803, 81.7626, 85.1756, 88.7616, 92.5521, 96.492, 100.55, 104.72, 109.084, 113.514, 118.197, 123.071, 128.061, 133.341, 138.724, 144.257, 149.905, 155.906, 162.144, 168.39, 174.967, 181.708, 188.672, 196.01, 203.584, 211.181, 219.091, 227.066, 235.409,244.073, 253.075, 262.124, 271.539, 281.203, 290.991, 301.253, 311.645, 322.338, 333.437, 344.657, 356.169, 368.062, 380.364, 392.808, 405.651, 418.805, 432.246, 445.973, 459.933, 474.24, 488.99, 504.095, 519.521, 535.239, 551.417, 568.002, 584.782, 601.983, 619.827, 637.684, 656.132, 674.811, 693.728, 713.198, 733.254,753.661, 774.269, 795.462, 817.018, 839.074, 861.556, 884.288, 907.465, 931.318, 955.542, 980.139, 1005.18, 1030.77, 1056.79, 1083.18, 1110.08, 1137.3, 1165.1, 1193.47, 1222.35, 1251.81, 1281.66, 1311.96, 1342.79, 1373.97, 1406.01, 1438.58, 1471.65, 1505.24, 1539.46, 1574.28, 1609.71, 1645.39, 1681.8, 1718.61, 1756.42, 1794.69, 1833.55, 1872.86, 1913.08, 1953.8, 1995.29, 2038.11, 2080.97, 2124.46, 2168.93, 2214.04, 2260.09, 2307.06, 2354.41, 2402.35, 2451.68, 2501.29, 2551.71, 2603.17, 2655.91, 2709.96, 2764.55, 2820.31, 2876.54, 2933.35, 2991.78, 3051.22, 3111.18, 3172.27, 3234.87, 3298.85, 3364.68, 3430.89, 3498.44, 3567.44, 3638.38, 3711.01, 3784.97, 3861.04, 3938.56, 4017.48, 4099.05, 4181.81, 4266.65, 4353.98, 4443.42, 4535.56, 4630.97, 4729.41, 4829.54, 4934.49, 5044.11, 5162.64, 5306.21, 8171.19
  };

  std::vector<double> centrality_table_down_asc = {
    0, 10.3463, 11.0861, 11.7998, 12.5172, 13.2458, 14.0069, 14.7568, 15.519, 16.3334, 17.1454, 17.9715, 18.8204, 19.7036, 20.6264, 21.605, 22.6104, 23.6387, 24.7063, 25.796, 26.9534, 28.1605, 29.4073, 30.6918, 32.0377, 33.4416, 34.9364, 36.4788, 38.1179, 39.8435, 41.6583, 43.4757, 45.4068, 47.4261, 49.5297, 51.6006, 53.7062, 55.9019, 58.199, 60.5762, 63.0623, 65.6426, 68.3145, 71.1183, 74.0371, 77.0526, 80.1847, 83.4274, 86.8108, 90.3357, 93.9759, 97.7734, 101.745, 105.854, 110.126, 114.482, 119.024, 123.747, 128.668, 133.745, 139.036, 144.527, 150.19, 156.04, 162.051, 168.268, 174.704, 181.437, 188.373, 195.443, 202.785, 210.302, 218.137, 226.175, 234.398, 242.965, 251.767, 260.763, 270.085, 279.822, 289.708, 299.86, 310.338, 321.122, 332.245, 343.563, 355.174, 367.154, 379.524, 392.109, 405.031, 418.299, 431.845, 445.699, 459.798, 474.234, 489.117, 504.386, 519.952, 535.84, 552.219, 569.001, 585.895, 603.3, 621.38, 639.438, 658.099, 676.89, 696.136, 715.903, 736.115, 756.791, 777.69, 799.191, 821.022, 843.312, 866.052, 889.133, 912.586, 936.843, 961.302, 986.171, 1011.74, 1037.65, 1064.03, 1090.8, 1118.01, 1145.77, 1173.82, 1202.86, 1232.18, 1262.05, 1292.32, 1323.16, 1354.33, 1386.08, 1418.62, 1451.56, 1485.4, 1519.47, 1554.19, 1589.76, 1625.49, 1661.89, 1698.83, 1736.63, 1775, 1813.88, 1853.12, 1893.44, 1934.42, 1975.76, 2018.46, 2061.74, 2105.37, 2149.7, 2194.95, 2240.94, 2288.16, 2335.73, 2383.92, 2433.41, 2483.29, 2533.65, 2585.27, 2638.35, 2692.45, 2747.33, 2803.27, 2859.75, 2916.98, 2975.45, 3035.05, 3095.57, 3156.99, 3219.67, 3284.06, 3349.98, 3417.04, 3485.29, 3554.01, 3625.58, 3698.49, 3772.71, 3849.45, 3927.34, 4006.82, 4089.11, 4172.24, 4257.81, 4345.44, 4435.8, 4528.43, 4624.6, 4723.91, 4824.73, 4930.41, 5040.92, 5160.37, 5304.54, 8171.19
  };
  // Create the final (descending) tables
  std::vector<double> centrality_table_nominal = centrality_table_nominal_asc;
  std::vector<double> centrality_table_up = centrality_table_up_asc;
  std::vector<double> centrality_table_down = centrality_table_down_asc;
  // Reverse the centrality tables
  std::reverse(centrality_table_nominal.begin(), centrality_table_nominal.end());
  std::reverse(centrality_table_up.begin(), centrality_table_up.end());
  std::reverse(centrality_table_down.begin(), centrality_table_down.end());
  // Now table[0] is the 0% boundary (high HF) and table[200] is the 100% boundary (0 HF)

  // --- Load Muon Scale Factors from JSON ---
  std::cout << "Loading Muon Scale Factors from JSON..." << std::endl;
  CorrectionSF tightID_SF;
  CorrectionSF hlt_SF;

  std::string json_filename = "HLT_HIL2SingleMu7_and_TightID_abseta1_pt1_cutAndCount_schemaV2.json";
  std::ifstream json_file_stream(json_filename);
  if (!json_file_stream.is_open()) {
      std::cerr << "Error: Cannot open JSON file: " << json_filename << std::endl;
      return;
  }

  json sf_data;
  try {
      sf_data = json::parse(json_file_stream);
  } catch (json::parse_error& e) {
      std::cerr << "[Error] Failed to parse JSON file: " << json_filename << std::endl;
      std::cerr << e.what() << std::endl;
      return;
  }

  bool tightID_loaded = false;
  bool hlt_loaded = false;

  if (sf_data.contains("corrections") && sf_data["corrections"].is_array()) {
      for (const auto& corr : sf_data["corrections"]) {
          std::string name = corr["name"];
          if (name == "NUM_TightID_DEN_genTracks") {
              tightID_SF.load(corr["data"]);
              tightID_loaded = true;
              std::cout << "  Loaded 'NUM_TightID_DEN_genTracks'" << std::endl;
          } else if (name == "NUM_HLT_HIL2SingleMu7_v_DEN_TightID") {
              hlt_SF.load(corr["data"]);
              hlt_loaded = true;
              std::cout << "  Loaded 'NUM_HLT_HIL2SingleMu7_v_DEN_TightID'" << std::endl;
          }
      }
  }

  if (!tightID_loaded || !hlt_loaded) {
      std::cerr << "Error: Failed to load required corrections from JSON." << std::endl;
      if (!tightID_loaded) std::cerr << "  'NUM_TightID_DEN_genTracks' was not found." << std::endl;
      if (!hlt_loaded) std::cerr << "  'NUM_HLT_HIL2SingleMu7_v_DEN_TightID' was not found." << std::endl;
      return;
  }
  std::cout << "JSON Scale Factors loaded successfully." << std::endl;
  // --- End Load Muon Scale Factors from JSON ---

  // Centrality weights
  const float Ncoll[200] = {1893.13, 1867.0, 1834.16, 1805.64, 1770.84, 1744.49, 1699.76, 1661.52, 1615.89, 1579.59, 1540.62, 1499.14, 1469.01, 1432.18, 1402.8, 1368.39, 1338.12, 1302.26, 1274.91, 1245.56, 1215.28, 1183.76, 1160.61, 1131.12, 1107.67, 1078.54, 1055.72, 1026.72, 1000.57, 980.728, 958.777, 936.515, 911.397, 889.182, 869.677, 853.33, 826.999, 808.145, 792.14, 769.639, 753.513, 732.883, 716.817, 697.168, 679.091, 668.056, 650.114, 631.024, 616.203, 597.835, 583.435, 571.454, 555.478, 543.589, 526.328, 511.657, 497.023, 489.255, 471.52, 461.133, 447.767, 436.993, 426.106, 412.626, 403.224, 389.71, 382.595, 371.48, 358.899, 349.179, 339.387, 330.523, 320.094, 313.254, 302.339, 292.421, 282.594, 274.834, 268.847, 259.463, 252.027, 244.561, 236.738, 229.574, 222.898, 215.138, 207.328, 200.879, 196.592, 190.921, 183.942, 176.685, 170.919, 166.96, 161.057, 154.421, 148.816, 144.84, 139.087, 134.448, 128.72, 124.905, 121.166, 116.648, 112.367, 109.012, 104.33, 100.736, 97.3484, 93.2283, 89.3299, 85.9068, 83.6446, 80.2019, 77.5299, 73.9647, 70.7606, 68.2284, 65.793, 63.4532, 60.4738, 58.2406, 55.063, 53.7287, 51.4638, 49.241, 47.0111, 45.5443, 43.1729, 41.5041, 39.5449, 37.9282, 36.8918, 34.9287, 33.1886, 31.9177, 30.756, 29.0803, 27.6721, 26.42, 25.2678, 24.2585, 23.1429, 22.0138, 21.0169, 19.8203, 19.1043, 18.1478, 17.1715, 16.3605, 15.4763, 14.7973, 14.1594, 13.3927, 12.795, 12.1059, 11.5921, 10.9751, 10.3213, 9.94434, 9.3518, 8.94274, 8.37618, 7.94437, 7.48868, 7.06923, 6.71137, 6.31856, 6.03184, 5.67048, 5.43369, 5.13727, 4.83292, 4.58846, 4.37208, 4.15225, 3.84385, 3.63752, 3.45214, 3.24892, 3.02845, 2.81715, 2.66395, 2.5053, 2.29512, 2.13703, 1.93591, 1.79771, 1.64165, 1.54375, 1.45878, 1.36718, 1.2942, 1.23934, 1.18423, 1.14467, 1.11826, 1.0863, 1.06149, 1.04497 };


  // Open the weight file and retrieve the h_weight_rho histogram
    TFile* rho_weightFile = TFile::Open("weights_MC/rho_weights_1/weight_rho.root", "READ");
    if (!rho_weightFile) {
        std::cerr << "Error: Cannot open weights_rho.root file!" << std::endl;
        return;
    }
    TH1D* h_weight_rho = (TH1D*)rho_weightFile->Get("h_weight_rho");
    if (!h_weight_rho) {
        std::cerr << "Error: Cannot retrieve h_weight_rho histogram!" << std::endl;
        return;
    }
    h_weight_rho->SetDirectory(0); // Detach histogram from file to avoid auto-deletion
    rho_weightFile->Close();

  // Open the weight file and retrieve the h_weight_vz histogram
    TFile* vz_weightFile = TFile::Open("weights_MC/vz_weights_2/weight_vz.root", "READ");
    if (!vz_weightFile) {
        std::cerr << "Error: Cannot open weights_vz.root file!" << std::endl;
        return;
    }
    TH1D* h_weight_vz = (TH1D*)vz_weightFile->Get("h_weight_vz");
    if (!h_weight_vz) {
        std::cerr << "Error: Cannot retrieve h_weight_vz histogram!" << std::endl;
        return;
    }
    h_weight_vz->SetDirectory(0); // Detach histogram from file to avoid auto-deletion
    vz_weightFile->Close();

    // Open the weight file and retrieve the h_weight_JEWEL histogram
    TFile* JEWEL_weightFile = TFile::Open("weights_MC/final_weight_3/weight_JEWEL.root", "READ");
    if (!JEWEL_weightFile) {
        std::cerr << "Error: Cannot open weights_JEWEL.root file!" << std::endl;
        return;
    }
    TH1D* h_weight_JEWEL = (TH1D*)JEWEL_weightFile->Get("h_weight_JEWEL");
    if (!h_weight_JEWEL) {
        std::cerr << "Error: Cannot retrieve h_weight_JEWEL histogram!" << std::endl;
        return;
    }
    h_weight_JEWEL->SetDirectory(0); // Detach histogram from file to avoid auto-deletion
    JEWEL_weightFile->Close();

  //TTrees
  TChain data("data"), EventTree("EventTree"), HiTree("HiTree"),  skimanalysis("skimanalysis"), hltanalysis("hltanalysis"), hiFJRhoAnalyzerFinerBins("hiFJRhoAnalyzerFinerBins");

  glob_t globlist;


  // Binning_option for mixed event background subtraction, Use HF binning as default
  // 0: HF binning only
  // 1: VZ binning only
  // 2: VZ + Centrality binning
  int binning_option = 2;

  // File with MinBias sample
  TFile *inFile_MinBias;

  TString file_name = sample_name;
  bool isData = false;

  double Xsec = 1;
  double Ngen = 1;

  if (file_name.Contains("data")) {
    isData = true;
    glob("/eos/infnts/cms/store/user/kdeleo/HIPhysicsRawPrime*/CRAB3_Analysis_test13_ZMM_Prime*/*/*.root", GLOB_NOSORT, NULL, &globlist);
    if (binning_option == 0) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_HF.root");
    else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_VZ.root");
    else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_data_VZ_Cen_Combined.root");
    else { cerr << "Invalid binning_option for data MinBias file." << endl; return; }
    cout << "This is data" << endl;
  }
  else {
    // Loop over files
    for (const auto& file : files) {
      if (file_name.Contains(file.label)) {
        glob(file.path_miniaod, GLOB_NOSORT, NULL, &globlist);
        if (binning_option == 0) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_HF.root");
        else if (binning_option == 1) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_VZ.root");
        else if (binning_option == 2) inFile_MinBias = TFile::Open("./MixEvSub/MinBias_leading_jets_MC_VZ_Cen_Combined.root");
        else { cerr << "Invalid binning_option for MC MinBias file." << endl; return; }
        Xsec = file.xsec;
        Ngen = file.ngen;
        cout << "This is MC " << file.label << ": ngen = " << Ngen << " xsec = " << Xsec << endl;
      }
    }
  }
  cout << "Found " << globlist.gl_pathc << " files"<< endl;

  // --- Initialize JER Provider ---
  JERProvider jer;
  if (!isData) {
    // Is MC
    cout << "Initializing JER..." << endl;
    // Load both SF and Resolution Files
    jer.LoadSF("Autumn18_RunD_V7b_MC_SF_AK4PF.txt");
    jer.LoadResolution("Autumn18_RunD_V7b_MC_PtResolution_AK4PF.txt");
    // Note: We typically don't apply Phi/Eta smearing for standard analysis
    // unless specifically required, so we only load PtResolution.
  }
  // --------------------------------

  // --- Load Jet Veto Map ---
  TFile* f_veto = TFile::Open("Summer23BPixPrompt23_RunD_v1.root");
  if (!f_veto || f_veto->IsZombie()) {
      std::cerr << "Error: Cannot open Jet Veto file Summer23BPixPrompt23_RunD_v1.root!" << std::endl;
      return;
  }
  TH2D* h_jet_veto_map = (TH2D*)f_veto->Get("jetvetomap_all");
  if (!h_jet_veto_map) {
      std::cerr << "Error: Cannot retrieve jetvetomap_all from file!" << std::endl;
      return;
  }
  h_jet_veto_map->SetDirectory(0); // Detach from file so it stays in memory
  f_veto->Close();
  std::cout << "Loaded Jet Veto Map: jetvetomap_all" << std::endl;

  //MC normalization
  double number_A = 208; // Lead
  // --- Read Lumi Automatically ---
  double Lumi = getLumiFromSummary("brilcalc_Collisions2023HI.csv"); // nb-1
  std::cout << "Parsed Lumi  : " << Lumi << " nb^-1" << std::endl;
  // Get MC all histogram
  TFile* file_MC_all = TFile::Open("./weights_MC/MC_all_weights/output_HI_mu_MC_all.root", "READ");
  TDirectoryFile* dir_Muons_MC_all = (TDirectoryFile*)file_MC_all->Get("HI/Muons");
  TH1D* h_norm = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights");
  TH1D* h_norm_cen = (TH1D*)dir_Muons_MC_all->Get("h_sum_weights_cen");
  TH1D* h_nev = (TH1D*)dir_Muons_MC_all->Get("h_n_events");
  TH1D* h_cen_after = (TH1D*)dir_Muons_MC_all->Get("h_cen_after");
  double n_ev = h_nev->Integral(0, h_nev->GetNbinsX()+1);
  double sum_w = h_norm->Integral(0, h_norm->GetNbinsX()+1);
  double sum_ncoll = h_norm_cen->Integral(0, h_norm_cen->GetNbinsX()+1);
  double sum_w_and_ncoll = h_cen_after->Integral(0, h_cen_after->GetNbinsX()+1);
  std::cout << "n_ev = " << n_ev << " sum_w = " << sum_w << " sum_ncoll = " << sum_ncoll << std::endl;

  double norm_MC_w_ncoll = number_A*number_A*Lumi*Xsec*(n_ev/sum_ncoll)/sum_w;
  double norm_MC_w = number_A*number_A*Lumi*Xsec/sum_w;
  if (!file_name.Contains("signal")) norm_MC_w_ncoll = number_A*number_A*Lumi*Xsec*n_ev/sum_ncoll/Ngen;
  if (!isData) std::cout << " norm_MC_w = " << norm_MC_w << "norm_MC_w_ncoll = " << norm_MC_w_ncoll << std::endl;

  if (!inFile_MinBias || inFile_MinBias->IsZombie()) {
        std::cerr << "Error: Could not open input file! Check path and file existence." << std::endl;
        return;
  }

  for (size_t i = 0; i < globlist.gl_pathc; i++) {
    //data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzerSubstructure/t");
    data.Add(TString(globlist.gl_pathv[i]) + "/akCs2PFJetAnalyzer/t");
    EventTree.Add(TString(globlist.gl_pathv[i]) + "/muonAnalyzer/MuonTree");
    HiTree.Add(TString(globlist.gl_pathv[i]) + "/hiEvtAnalyzer/HiTree");
    skimanalysis.Add(TString(globlist.gl_pathv[i]) + "/skimanalysis/HltTree");
    hiFJRhoAnalyzerFinerBins.Add(TString(globlist.gl_pathv[i]) + "/hiFJRhoAnalyzerFinerBins/t");
    //hltanalysis.Add(TString(globlist.gl_pathv[i]) + "/hltanalysis/HltTree");

  }
  globfree(&globlist);

  //To associate additional TTrees with a primary TTree. This allows you to access information from the friend trees while looping over the primary tree
  data.AddFriend("EventTree");
  data.AddFriend("HiTree");
  data.AddFriend("skimanalysis");
  data.AddFriend("hiFJRhoAnalyzerFinerBins");
  //data.AddFriend("hltanalysis");

  TTreeReader fReader(&data);

  // Declaration of leaf types
  TTreeReaderValue<Int_t> run = {fReader, "run"};    // Run number
  TTreeReaderValue<Int_t> evt = {fReader, "evt"};    // Event number
  TTreeReaderValue<Int_t> lumi = {fReader, "lumi"};  // Luminosity block
  TTreeReaderValue<Int_t> hiBin = {fReader, "hiBin"}; // centralityx2
  TTreeReaderValue<Float_t> weight = {fReader, isData ? "hiHF" : "weight"}; // MC event weight, not used in data
  TTreeReaderValue<Float_t> vz = {fReader, "vz"};
  TTreeReaderArray<double> rho = {fReader, "rho"};
  TTreeReaderValue<Float_t> hiHF = {fReader, "hiHF"};
  //TTreeReaderValue<float> Ncoll = {fReader, "Ncoll"}; // Ncoll

  // Filters
  TTreeReaderValue<int> pprimaryVertexFilter = {fReader, "pprimaryVertexFilter"};
  TTreeReaderValue<int> pclusterCompatibilityFilter = {fReader, "pclusterCompatibilityFilter"};
  TTreeReaderValue<int> pphfCoincFilter2Th4 = {fReader, "pphfCoincFilter2Th4"};

  // Trigger, no needed because already in production
  //TTreeReaderValue<Int_t> HLT_HIL2SingleMu7_v3 = {fReader, "HLT_HIL2SingleMu7_v3"};

  // Muon
  TTreeReaderValue<Int_t> nReco = {fReader, "nReco"};
  TTreeReaderArray<Float_t> recoPt = {fReader, "recoPt"};
  TTreeReaderArray<Float_t> recoEta = {fReader, "recoEta"};
  TTreeReaderArray<Float_t> recoPhi = {fReader, "recoPhi"};
  TTreeReaderArray<Int_t> recoCharge = {fReader, "recoCharge"};
  TTreeReaderArray<bool> recoIDTight = {fReader, "recoIDTight"};
  const double muonMass = 0.1056583755; //From PDG 2024

  // Jet
  TTreeReaderValue<Int_t> nref = {fReader, "nref"};
  //TTreeReaderArray<Float_t> rawpt = {fReader, "jtptUncorrected"};
  TTreeReaderArray<Float_t> jteta = {fReader, "jteta"};
  TTreeReaderArray<Float_t> jtphi = {fReader, "jtphi"};
  //TTreeReaderArray<Float_t> jtgirth = {fReader, "jt_girth"};
  //TTreeReaderArray<Float_t> jtdyndeltaR = {fReader, "jtdyn_deltaR"};
  TTreeReaderArray<Float_t> rawpt = {fReader, "rawpt"};
  //TTreeReaderArray<Float_t> jtm = {fReader, "jtm"};

  // To apply corrections on jets
  vector<string> Files;
  Files.push_back("ParallelMC_L2Relative_AK2PF_PbPb_Reco_v0_2_13_2024.txt");
  JetCorrector JEC(Files);
  JetUncertainty JEU("Autumn18_HI_V8_MC_Uncertainty_AK2PF.txt"); //!!! Old, update

  // Gen jets
  TTreeReaderValue<Int_t> ngen = {fReader, isData ? "nref" : "ngen"};
  TTreeReaderArray<Float_t> genpt = {fReader, isData ? "rawpt" : "genpt"};
  TTreeReaderArray<Float_t> geneta = {fReader, isData ? "jteta" : "geneta"};
  TTreeReaderArray<Float_t> genphi = {fReader, isData ? "jtphi" : "genphi"};

  // Gen Muon
  TTreeReaderValue<Int_t> ngenMu = {fReader, isData ? "nReco" : "nGen"};
  TTreeReaderArray<Float_t> genMuPt = {fReader, isData ? "recoPt" : "genPt"};
  TTreeReaderArray<Float_t> genMuEta = {fReader, isData ? "recoEta" : "genEta"};
  TTreeReaderArray<Float_t> genMuPhi = {fReader, isData ? "recoPhi" : "genPhi"};
  TTreeReaderArray<Int_t> genMuPID = {fReader, isData ? "recoCharge" : "genPID"};

  // Access MinBias sample
  TTree *inputTree = (TTree*)inFile_MinBias->Get("jet_tree");
    if (!inputTree) {
        std::cerr << "Error: Could not find TTree 'jet_tree' in the input file!" << std::endl;
        inFile_MinBias->Close();
        return;
  }

  // Declare variables to hold the branch data
  Float_t HF_MinBias;
  Float_t vz_MinBias;
  Int_t hiBin_MinBias;
  Int_t bin_MinBias; // Global bin number in MinBias tree
  Float_t jet_pt_MinBias;
  Float_t jet_phi_MinBias;
  Float_t jet_eta_MinBias;

  // Set branch addresses to link variables to tree branches
  inputTree->SetBranchAddress("HF_MinBias", &HF_MinBias);
  inputTree->SetBranchAddress("vz_MinBias", &vz_MinBias);
  inputTree->SetBranchAddress("hiBin_MinBias", &hiBin_MinBias); // Link new branch
  inputTree->SetBranchAddress("bin_MinBias", &bin_MinBias);
  inputTree->SetBranchAddress("jet_pt_MinBias", &jet_pt_MinBias);
  inputTree->SetBranchAddress("jet_phi_MinBias", &jet_phi_MinBias);
  inputTree->SetBranchAddress("jet_eta_MinBias", &jet_eta_MinBias);

  // Pre-count the MinBias events Before the event loop, count how many events you actually have for each bin.
  std::map<int, int> mb_counts;
  for(int i=0; i < inputTree->GetEntries(); i++) {
    inputTree->GetEntry(i);
    mb_counts[bin_MinBias]++;
  }

  // --- Bin definition for MinBias matching (based on selected option) ---
  std::vector<std::pair<float, float>> primary_bins; // For HF or VZ only
  std::vector<std::pair<float, float>> vz_bins_combined;     // For combined VZ+Centrality
  std::vector<std::pair<float, float>> centrality_bins_combined; // For combined VZ+Centrality

  int total_mixed_bins_expected = 0; // Total expected bins for the selected option in MinBias
  int events_per_mixed_bin_limit = 0; // The max events collected per bin in MinBias

  if (binning_option == 0) { // HF binning
      total_mixed_bins_expected = BinningConfig::tot_bins;
      events_per_mixed_bin_limit = BinningConfig::ev_per_bin;
      float current_min = BinningConfig::frst_bin_min;
      for (int i = 0; i < total_mixed_bins_expected; ++i) {
          float current_max = (current_min * 1.1);
          primary_bins.push_back({current_min, current_max});
          current_min = current_max;
      }
      std::cout << "Defined HF bins for MinBias matching (" << primary_bins.size() << " total)." << std::endl;
  } else if (binning_option == 1) { // VZ binning only
      total_mixed_bins_expected = BinningConfig_vz::tot_bins;
      events_per_mixed_bin_limit = BinningConfig_vz::ev_per_bin;
      float current_min = BinningConfig_vz::frst_bin_min;
      for (int i = 0; i < total_mixed_bins_expected; ++i) {
          float current_max = (current_min + 10.0f); // 10cm wide vz bins
          primary_bins.push_back({current_min, current_max});
          current_min = current_max;
      }
      std::cout << "Defined VZ bins for MinBias matching (" << primary_bins.size() << " total)." << std::endl;
  } else if (binning_option == 2) { // VZ + Centrality binning
      // Centrality bins based on hiBin
      float cen_bin_width_hiBin = (BinningConfig_Combined_Vz_Centrality::centrality_max_hiBin - BinningConfig_Combined_Vz_Centrality::centrality_min_hiBin) / BinningConfig_Combined_Vz_Centrality::num_centrality_bins;
      for (int i = 0; i < BinningConfig_Combined_Vz_Centrality::num_centrality_bins; ++i) {
          float min_hiBin = BinningConfig_Combined_Vz_Centrality::centrality_min_hiBin + i * cen_bin_width_hiBin;
          float max_hiBin = min_hiBin + cen_bin_width_hiBin;
          centrality_bins_combined.push_back({min_hiBin, max_hiBin});
      }

      // Vz bins based on explicit edges
      for (int i = 0; i < BinningConfig_Combined_Vz_Centrality::num_vz_bins; ++i) {
          vz_bins_combined.push_back({BinningConfig_Combined_Vz_Centrality::vz_bin_edges[i], BinningConfig_Combined_Vz_Centrality::vz_bin_edges[i+1]});
      }

      total_mixed_bins_expected = BinningConfig_Combined_Vz_Centrality::num_centrality_bins * BinningConfig_Combined_Vz_Centrality::num_vz_bins;
      events_per_mixed_bin_limit = BinningConfig_Combined_Vz_Centrality::ev_per_combined_bin;
      std::cout << "Defined combined Centrality-VZ bins for MinBias matching (" << total_mixed_bins_expected << " total)." << std::endl;
  } else {
      std::cerr << "Invalid binning_option: " << binning_option << std::endl;
      return;
  }
  // --- End of bin definition ---

  // TTree entries
  Int_t nEntries_MinBias = inputTree->GetEntries();
  std::cout << "Reading " << nEntries_MinBias << " entries from 'jet_tree'..." << std::endl;
  //Canvas
  gStyle->SetOptStat(0);
  TCanvas* c1 = new TCanvas("c1", "c1", 1200, 800);
  c1->Divide(1,1);
  TCanvas* c2 = new TCanvas("c2", "c2", 1200, 800);
  c2->Divide(1,1);
  TCanvas* c3 = new TCanvas("c3", "c3", 1200, 800);
  c3->Divide(1,1);
  TCanvas* c4 = new TCanvas("c4", "c4", 1200, 800);
  c4->Divide(1,1);
  TCanvas* c5 = new TCanvas("c5", "c5", 1200, 800);
  c5->Divide(1,1);
  TCanvas* c6 = new TCanvas("c6", "c6", 1200, 800);
  c6->Divide(1,1);
  TCanvas* c7 = new TCanvas("c7", "c7", 1200, 800);
  c7->Divide(1,1);

  //Histograms

  // Define binning for xZj unfolding.
  const int nbins_xZj = 5; // Number of bins (number of edges - 1)
  double xZj_bins[nbins_xZj + 1] = {0, 0.6, 0.9, 1.2, 1.5, 2.};
  const int nbins_xZj_meas = (systFlag != 8) ? nbins_xZj : nbins_xZj-1;

  // Define the bins using a vector so we can initialize conditionally
  std::vector<double> xZj_bins_meas_vec;
  if (systFlag != 8) {
    xZj_bins_meas_vec = {0., 0.6, 0.9, 1.2, 1.5, 2.};
  } else {
    xZj_bins_meas_vec = {0., 0.6, 0.9, 1.2, 1.5};
  }

  // Create a pointer to the vector's data (compatible with TH1F constructors)
  double* xZj_bins_meas = xZj_bins_meas_vec.data();
  //  double xZj_max;

  TH1F *h_mumu = new TH1F("h_mumu", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt = new TH1F("h_Z_pt", "Hist;p_{t}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1F *h_njet = new TH1F("h_njet", "Hist;Number of jets; Entries", 10, 0, 10);
  TH1F *h_cen = new TH1F("h_cen", "Hist; centrality bin; Entries", 20, 0, 100);

  TH1F *h_mumu_j = new TH1F("h_mumu_j", "Hist;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F *h_Z_pt_j = new TH1F("h_Z_pt_j", "Hist;p_{t}^{Z} [GeV]; Entries", 30, 0, 300);
  TH1F *h_jet_pt_lj = new TH1F("h_jet_pt_lj", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_cen_j = new TH1F("h_cen_j", "Hist; centrality bin; Entries", 20, 0, 100);
  TH1F *h_HF_j = new TH1F("h_HF_j", "Hist; HF; Entries", 80, 0, 8000);
  TH1F *h_deltaPhi_Zj = new TH1F("h_deltaPhi_Zj", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_xZj = new TH1F("h_xZj", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_fixbinw = new TH1F("h_xZj_fixbinw", "Hist;x_{Zj}; Entries", 30, 0., 3.);
  TH2F *h_jet_etaphi_before = new TH2F("h_jet_etaphi_before", "Jets Before Veto;#eta;#phi", 50, -2.5, 2.5, 60, -3.15, 3.15);
  TH2F *h_jet_etaphi_after  = new TH2F("h_jet_etaphi_after",  "Jets After Veto;#eta;#phi",  50, -2.5, 2.5, 60, -3.15, 3.15);

  TH1F *h_vz = new TH1F("h_vz", "Hist; vz; Entries", 30, -20, 20);
  TH1F *h_avg_rho = new TH1F("h_avg_rho", "Hist; <#rho>; Entries", 50, 0, 400);
  auto *h_avg_rho_vs_cen = new TProfile("h_avg_rho_vs_cen", "Profile of <#rho> vs centrality bin", 200, 0, 200, 0, 400);

  TH1F *h_deltaPhi_Zj_MinBias = new TH1F("h_deltaPhi_Zj_MinBias", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_jet_pt_lj_MinBias = new TH1F("h_jet_pt_lj_MinBias", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_xZj_MinBias = new TH1F("h_xZj_MinBias", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  TH1F *h_deltaPhi_Zj_matched = new TH1F("h_deltaPhi_Zj_matched", "Hist;#Delta#phi_{Zj}; Entries", 20, 0,TMath::Pi());
  TH1F *h_jet_pt_lj_matched = new TH1F("h_jet_pt_lj_matched", "Hist;leading jet p_{T} [GeV]; Entries", 30, 0, 300);
  TH1F *h_xZj_matched = new TH1F("h_xZj_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);

  // --- RooUnfold Histograms ---

  TH1F* h_xZj_true = new TH1F("h_xZj_true", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);     // For true MC
  TH1D *h_xZj_for_JEWEL_w = new TH1D("h_xZj_for_JEWEL_w", "True x_{Zj};Entries", 60, 0.,3.);
  TH1F* h_mumu_true = new TH1F("h_mumu_true", "True m;m_{#mu#mu} [GeV]; Entries", 20, 60, 120);
  TH1F* h_xZj_reco = new TH1F("h_xZj_reco", "Reco x_{Zj};x_{Zj};Entries", nbins_xZj_meas, xZj_bins_meas);
  TH2F* h_response = new TH2F("h_response", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_unmatched = new TH2F("h_response_unmatched", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas,nbins_xZj, xZj_bins);
  TH2F* h_response_MinBias = new TH2F("h_response_MinBias", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  TH1F *h_xZj_train_closure = new TH1F("h_xZj_train_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_train_closure_matched = new TH1F("h_xZj_train_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_test_closure = new TH1F("h_xZj_test_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_test_closure_matched = new TH1F("h_xZj_test_closure_matched", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_MinBias_train_closure = new TH1F("h_xZj_MinBias_train_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F *h_xZj_MinBias_test_closure = new TH1F("h_xZj_MinBias_test_closure", "Hist;x_{Zj}; Entries", nbins_xZj_meas, xZj_bins_meas);
  TH1F* h_xZj_true_train_closure = new TH1F("h_xZj_true_train_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH1F* h_xZj_true_test_closure = new TH1F("h_xZj_true_test_closure", "True x_{Zj};x_{Zj};Entries", nbins_xZj, xZj_bins);
  TH2F* h_response_closure = new TH2F("h_response_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_closure_unmatched = new TH2F("h_response_closure_unmatched", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);
  TH2F* h_response_MinBias_closure = new TH2F("h_response_MinBias_closure", "Response Matrix;Reco x_{Zj};True x_{Zj}", nbins_xZj_meas, xZj_bins_meas, nbins_xZj, xZj_bins);

  // --- End RooUnfold Histograms ---

  //TH1F *h_jetgirth = new TH1F("h_jetgirth", "Hist;girth; Entries", 10, 0, 0.2);
  //TH1F *h_jet_deltaR = new TH1F("h_jet_deltaR", "Hist; R_{g}; Entries", 10, 0, 0.2);

  // Output root file
  TFile *file_output_HI_mu;

  if (isData && weight_phase != 0) {
     if (systFlag == 0)  file_output_HI_mu = new TFile("./plot/output_HI_mu_data.root", "RECREATE");
     else if (systFlag == 6) {
       cout << "Running Systematic (Centrality) down-variation (systFlag = 6)" << endl;
       file_output_HI_mu = new TFile("./syst_cen/output_HI_mu_data_cen_down.root", "RECREATE");
     }
     else if (systFlag == 7) {
       cout << "Running Systematic (Centrality) up-variation (systFlag = 7)" << endl;
       file_output_HI_mu = new TFile("./syst_cen/output_HI_mu_data_cen_up.root", "RECREATE");
     }
     else if (systFlag == 8) {
       cout << "Running binning variation (systFlag = 8)" << endl;
       file_output_HI_mu = new TFile("./syst_binning/output_HI_mu_data_binning.root", "RECREATE");
     }
  }

  if (weight_phase == 0) {
    if (isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_HI_mu_data_Ncoll_weights.root", "RECREATE");
    }
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/Ncoll_weights_0/output_HI_mu_MC_Ncoll_weights.root", "RECREATE");
    }
  }

  if (weight_phase == 1) {
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/rho_weights_1/output_HI_mu_MC_rho_weights.root", "RECREATE");
    }
  }

  // This is just for plotting rho distributions after reweighting
  else if (weight_phase == -1) {
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_HI_mu_MC_rho_weights_after.root", "RECREATE");
    }
  }

  else if (weight_phase == 2) {
    if (!isData) {
      file_output_HI_mu = new TFile("./weights_MC/vz_weights_2/output_HI_mu_MC_vz_weights.root", "RECREATE");
    }
  }

  else if (weight_phase == 3) {
    if (!isData) {
      if (systFlag == 0) file_output_HI_mu = new TFile("./plot/output_HI_mu_MC_"+file_name+".root", "RECREATE");
      else if (systFlag == 1) {
        cout << "Running Systematic (SF muon) - DOWN variation (systFlag = 1)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_HI_mu_MC_SF_down.root", "RECREATE");
      }
      else if (systFlag == 2) {
        cout << "Running Systematic (SF muon) - UP variation (systFlag = 2)" << endl;
        file_output_HI_mu = new TFile("./syst_SF_muon/output_HI_mu_MC_SF_up.root", "RECREATE");
      }
      else if (systFlag == 4) {
        cout << "Running Systematic (Prior model) variation (systFlag = 4)" << endl;
        file_output_HI_mu = new TFile("./syst_prior_model/output_HI_mu_MC_prior_model.root", "RECREATE");
      }
      else if (systFlag == 8) {
        cout << "Running  binning variation (systFlag = 8)" << endl;
        file_output_HI_mu = new TFile("./syst_binning/output_HI_mu_MC_binning.root", "RECREATE");
      }
      else if (systFlag == 9) {
        cout << "Running Systematic JEC - DOWN variation (systFlag = 9)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_HI_mu_MC_JEC_down.root", "RECREATE");
      }
      else if (systFlag == 10) {
        cout << "Running Systematic JEC - UP variation (systFlag = 10)" << endl;
        file_output_HI_mu = new TFile("./syst_JEC/output_HI_mu_MC_JEC_up.root", "RECREATE");
      }
      else if (systFlag == 11) {
        cout << "Running Systematic JER - DOWN variation (systFlag = 11)" << endl;
        file_output_HI_mu = new TFile("./syst_JER/output_HI_mu_MC_JER_down.root", "RECREATE");
      }
      else if (systFlag == 12) {
        cout << "Running Systematic JER - UP variation (systFlag = 12)" << endl;
        file_output_HI_mu = new TFile("./syst_JER/output_HI_mu_MC_JER_up.root", "RECREATE");
      }
      else if (systFlag == 13) {
        cout << "Running Systematic shape - DOWN variation (systFlag = 13)" << endl;
        file_output_HI_mu = new TFile("./syst_shape/output_HI_mu_MC_shape_down.root", "RECREATE");
      }
      else if (systFlag == 14) {
        cout << "Running Systematic shape - UP variation (systFlag = 14)" << endl;
        file_output_HI_mu = new TFile("./syst_shape/output_HI_mu_MC_shape_up.root", "RECREATE");
      }
    }
  }

  // Loop over events to access and analyze the data
  unsigned int iEvent = 0;
  unsigned int itotev = 0;
  while (fReader.Next()) {
    itotev++;
    if(*pprimaryVertexFilter<=0) continue;
    if(*pclusterCompatibilityFilter<=0) continue;
    if(*pphfCoincFilter2Th4<=0) continue;
    // Use hiHF to recalculate hiBin for systematics
    int hiBin_to_use = *hiBin; //getHiBin(*hiHF, centrality_table_nominal); // Start with the nominal hiBin
    if (isData && *hiBin != getHiBin(*hiHF, centrality_table_nominal)) cout << "!!! WARNING: *hiBin = " << *hiBin
     << " hiHF = " << *hiHF << " hiBin nominal = " << getHiBin(*hiHF, centrality_table_nominal) << " hiBin up = "
     << getHiBin(*hiHF, centrality_table_up) << " hiBin down = " << getHiBin(*hiHF, centrality_table_down) << endl;
    if (isData && systFlag == 6) hiBin_to_use = getHiBin(*hiHF, centrality_table_down); // Centrality Down
    if (isData && systFlag == 7) int hiBin_to_use = getHiBin(*hiHF, centrality_table_up); // Centrality Up
    float weight_cent = Ncoll[hiBin_to_use];
    // Scale MC
    float scale = 1;
    if (!isData && weight_phase == 0) {
      scale*=norm_MC_w*(*weight);
    }
    if (!isData && weight_phase != 0) {
      if (file_name.Contains("signal")) scale*=norm_MC_w_ncoll*weight_cent*(*weight);
      else scale*=norm_MC_w_ncoll*weight_cent;
    }
    // Selection on centrality bin
    if (weight_phase > 1 ) {
      if(hiBin_to_use>59) continue;
    }
    if (weight_phase == 1 || weight_phase == - 1) {
      if (isData) {
        if(hiBin_to_use>59) continue;
      }
    }

    // Calculate average rho
    double sum_rho = 0;
    for (unsigned int i = 0; i < rho.GetSize(); i++) {
      sum_rho += rho[i];
    }
    double avg_rho = 0;
    if (rho.GetSize() > 0) {
      avg_rho = sum_rho / rho.GetSize();
    }
    // use binning to get the value of the weight
    int bin_rho = h_weight_rho->FindBin(avg_rho);

    if (!isData) {
      if (weight_phase == 2 || weight_phase == -1) {
        // Apply rho weight
        scale*=h_weight_rho->GetBinContent(bin_rho);
      }
      if (weight_phase == 3) {
       // Apply rho and vz weight
       int bin_vz = h_weight_vz->FindBin(*vz);
       scale*=h_weight_rho->GetBinContent(bin_rho)*h_weight_vz->GetBinContent(bin_vz);
      }
    }

    // --- Fill information for unfolding ---
    double gen_Z_pt = 0;
    double gen_Z_phi = 0;
    double dPhi_Zj_Gen = 0;
    double true_xZj = 0;
    int ijetGenLeading_unfold = -1;
    if (!isData) {
      // TLorentzVectors for the gen muons
      TLorentzVector genmuPlus, genmuMinus;
      // Loop over muons, save indices of most energetic muon and antimuon pairs
      int iHighPtgenMu = -1;
      int iHighPtgenAntiMu = -1;
      for (unsigned int igenMu = 0; igenMu < *ngenMu; ++igenMu) {
        if (iHighPtgenMu == -1 || genMuPt[igenMu] > genMuPt[iHighPtgenMu]) {
          if (genMuPID[igenMu] == 13) iHighPtgenMu = igenMu;
        }
        if (iHighPtgenAntiMu == -1 || genMuPt[igenMu] > genMuPt[iHighPtgenAntiMu]) {
          if (genMuPID[igenMu] == -13) iHighPtgenAntiMu = igenMu;
        }
      }
      // Z from gen muon-antimuon pairs
      if (iHighPtgenMu != -1 && iHighPtgenAntiMu != -1) {
        genmuMinus.SetPtEtaPhiM(genMuPt[iHighPtgenMu], genMuEta[iHighPtgenMu], genMuPhi[iHighPtgenMu], muonMass);
        genmuPlus.SetPtEtaPhiM(genMuPt[iHighPtgenAntiMu], genMuEta[iHighPtgenAntiMu], genMuPhi[iHighPtgenAntiMu], muonMass);
        double gen_Z_mass = (genmuPlus + genmuMinus).M();
        gen_Z_pt = (genmuPlus + genmuMinus).Pt();
        gen_Z_phi = (genmuPlus + genmuMinus).Phi();
        // Apply mass cut
        if (gen_Z_mass >= 60 && gen_Z_mass <= 120) {
          if (genMuPt[iHighPtgenMu] > 20 && abs(genMuEta[iHighPtgenMu]) < 2.4 && genMuPt[iHighPtgenAntiMu] > 20 && abs(genMuEta[iHighPtgenAntiMu]) < 2.4) {
            // Cut on pt(Z)
            if (gen_Z_pt > 40 ) {
              h_mumu_true->Fill(gen_Z_mass, scale);
              // Loop over gen jets
              for (int ijetGen = 0; ijetGen < *ngen; ++ijetGen) {
                // Apply truth-level cuts
                if (genpt[ijetGen] < 30 || abs(geneta[ijetGen]) > 2.5) continue;
                double detaMinusGen = geneta[ijetGen] - genmuMinus.Eta();
                double dphiMinusGen = RelativePhi(genphi[ijetGen], genmuMinus.Phi());
                double dRMinusGen = TMath::Sqrt(detaMinusGen * detaMinusGen + dphiMinusGen * dphiMinusGen);
                double detaPlusGen = geneta[ijetGen] - genmuPlus.Eta();
                double dphiPlusGen = RelativePhi(genphi[ijetGen], genmuPlus.Phi());
                double dRPlusGen = TMath::Sqrt(detaPlusGen * detaPlusGen + dphiPlusGen * dphiPlusGen);
                if(dRMinusGen < 0.2 || dRPlusGen < 0.2 ) continue;
                if (ijetGenLeading_unfold == -1 || genpt[ijetGen] > genpt[ijetGenLeading_unfold]) {
                  ijetGenLeading_unfold = ijetGen;
                }
              }
              if (ijetGenLeading_unfold != -1) {
                dPhi_Zj_Gen = RelativePhi(gen_Z_phi, genphi[ijetGenLeading_unfold]);
                if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                  // Calculate true x_Zj
                  true_xZj = genpt[ijetGenLeading_unfold] / gen_Z_pt;
                  // Scale MC to JEWEL for check unfolding dependance on shape
                  if (systFlag == 4) {
                    int bin_xZj_JEWEL = h_weight_JEWEL->FindBin(true_xZj);
                    double weight_JEWEL = (h_weight_JEWEL->GetBinContent(bin_xZj_JEWEL)>0) ? h_weight_JEWEL->GetBinContent(bin_xZj_JEWEL) : 1;
                    scale*=weight_JEWEL;
                  }
                  if (systFlag == 13) {
                    if (true_xZj>=0.0 && true_xZj<0.6) scale*=2.2;
                    if (true_xZj>=0.6 && true_xZj<0.9) scale*=0.7;
                    if (true_xZj>=0.9 && true_xZj<1.5) scale*=0.3;
                  }
                  if (systFlag == 14) {
                    if (true_xZj>=0.0 && true_xZj<0.6) scale*=0.6;
                    if (true_xZj>=0.6 && true_xZj<0.9) scale*=1.4;
                    if (true_xZj>=0.9 && true_xZj<1.5) scale*=1.3;
                  }
                  h_xZj_for_JEWEL_w->Fill(true_xZj, scale);
                  //Remove overflow and put it in the last bin
                  //if (true_xZj > xZj_max) true_xZj = xZj_max - 0.01;
                  h_xZj_true->Fill(true_xZj, scale);
                  if (itotev < 0.7*Ngen) h_xZj_true_train_closure->Fill(true_xZj, scale);
                  else h_xZj_true_test_closure->Fill(true_xZj, scale);
                }
              }
            }
          }
        }
      }
    }
    // --- End fill information for unfolding ---

    //if(*HLT_HIL2SingleMu7_v3<=0) continue; // no needed because already in production
    bool good_pair = false;
    if (*nReco < 2 ) continue;
    iEvent++;
    //cout << "***iEvent = " << iEvent << "\t run = " << run << "\t lumi = " << lumi << "\t evt = " << event << endl;

    // TLorentzVectors for the muons
    TLorentzVector muPlus, muMinus;

    // Loop over muons, save indices of most energetic muon and antimuon pairs
    int iHighPtMu = -1;
    int iHighPtAntiMu = -1;
    //cout << "-----------------------------" << endl;
    for (unsigned int iMu = 0; iMu < *nReco; ++iMu) {
      if (recoIDTight[iMu]) {
        //cout << "iMu: " << iMu << " pt = " << recoPt[iMu] <<  " Q = " << recoCharge[iMu] << endl;
        if (iHighPtMu == -1 || recoPt[iMu] > recoPt[iHighPtMu]) {
          if (recoCharge[iMu] == -1) iHighPtMu = iMu;
        }
        if (iHighPtAntiMu == -1 || recoPt[iMu] > recoPt[iHighPtAntiMu]) {
          if (recoCharge[iMu] == +1) iHighPtAntiMu = iMu;
        }
      }
    }
    //cout << "iHighPtMu = " << iHighPtMu << " iHighPtAntiMu = " << iHighPtAntiMu << endl;

    // Z from muon-antimuon pairs
    if (iHighPtMu == -1 || iHighPtAntiMu == -1) continue;
    muMinus.SetPtEtaPhiM(recoPt[iHighPtMu], recoEta[iHighPtMu], recoPhi[iHighPtMu], muonMass);
    muPlus.SetPtEtaPhiM(recoPt[iHighPtAntiMu], recoEta[iHighPtAntiMu], recoPhi[iHighPtAntiMu], muonMass);
    double Z_mass = (muPlus + muMinus).M();
    double Z_pt = (muPlus + muMinus).Pt();
    double Z_phi = (muPlus + muMinus).Phi();
    // Apply Muon Scale Factors from TH2F histograms
    if (!isData) {
      std::string sf_flag;
      if (systFlag == 1 ) sf_flag = "systdown";
      else if (systFlag == 2) sf_flag = "systup";
      else sf_flag = "nominal";
      // Apply TightID scale factors for both muons in the Z candidate
      double sf_id_mu_plus = tightID_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], sf_flag);
      double sf_id_mu_minus = tightID_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], sf_flag);
      scale *= sf_id_mu_plus * sf_id_mu_minus;
      // Apply HLT scale factor combining them as SF1 + SF2 - (SF1 * SF2)
      // For trigger we only need one to lepton to have fired, so we use the addition rule of probability
      double sf_hlt_mu_plus = hlt_SF.getValue(abs(recoEta[iHighPtAntiMu]), recoPt[iHighPtAntiMu], sf_flag);
      double sf_hlt_mu_minus = hlt_SF.getValue(abs(recoEta[iHighPtMu]), recoPt[iHighPtMu], sf_flag);
      scale *= (sf_hlt_mu_plus + sf_hlt_mu_minus - (sf_hlt_mu_plus * sf_hlt_mu_minus));
    }

    // Apply mass cut
    if (Z_mass >= 60 && Z_mass <= 120) {
      if (recoPt[iHighPtMu] > 20 && abs(recoEta[iHighPtMu]) < 2.4 && recoPt[iHighPtAntiMu] > 20 && abs(recoEta[iHighPtAntiMu]) < 2.4){
        good_pair = true;
      }
    }
    if (!good_pair) continue;

    // Cut on pt(Z)
    if (Z_pt < 40 ) continue;

    h_vz->Fill(*vz, scale);
    h_avg_rho->Fill(avg_rho, scale);
    h_avg_rho_vs_cen->Fill(hiBin_to_use, avg_rho, scale);
    h_cen->Fill((hiBin_to_use)/2, scale);
    h_mumu->Fill(Z_mass, scale);
    h_Z_pt->Fill(Z_pt, scale);

    // Pre-calculate GenJet Vectors for easier passing to JER function
    std::vector<float> v_gen_pts, v_gen_etas, v_gen_phis;
    if (!isData) {
        for (int i = 0; i < *ngen; i++) {
            v_gen_pts.push_back(genpt[i]);
            v_gen_etas.push_back(geneta[i]);
            v_gen_phis.push_back(genphi[i]);
        }
    }

    // Loop over Jets
    unsigned int njets = 0;
    double detaMinus = 0, dphiMinus = 0, dRMinus = 0;
    double detaPlus = 0, dphiPlus = 0, dRPlus = 0;
    int ijetLeading = -1;
    int iGenjetMatchedtoLeadingReco = -1;
    bool isLeadingJetMatched = false;
    double jtpt_corr[20000];
    for(int ijet=0; ijet<*nref; ijet++){
      // Apply JEC and JEC uncertainty
      //cout << "before JEC: " << rawpt[ijet] << endl;
      JEC.SetJetPT(rawpt[ijet]);
      JEC.SetJetEta(jteta[ijet]);
      JEC.SetJetPhi(jtphi[ijet]);
      double Correction = JEC.GetCorrection();
      double CorrectedPT = JEC.GetCorrectedPT();
      JEU.SetJetPT(CorrectedPT);
      JEU.SetJetEta(jteta[ijet]);
      JEU.SetJetPhi(jtphi[ijet]);
      double pt_jec_applied = CorrectedPT;
      if (!isData && systFlag == 9) pt_jec_applied = CorrectedPT * (1 - JEU.GetUncertainty().first); //down
      if (!isData && systFlag == 10) pt_jec_applied = CorrectedPT * (1 + JEU.GetUncertainty().second); //up
      //jtpt_corr[ijet] = rawpt[ijet];
      //cout << "after JEC: jtpt_corr = " << jtpt_corr[ijet] << " CorrectedPT = " << CorrectedPT << endl;
      // Apply JER (Hybrid Method)
      double pt_final = pt_jec_applied;
      if (!isData) {
        int jer_syst = 0;
        // map systFlag 11/12 to JER Up/Down
        if (systFlag == 11) jer_syst = -1; // Down
        if (systFlag == 12) jer_syst = 1;  // Up

        pt_final = jer.GetSmearedPt(pt_jec_applied, jteta[ijet], jtphi[ijet], avg_rho, v_gen_pts, v_gen_etas, v_gen_phis, jer_syst);
      }
      jtpt_corr[ijet] = pt_final;
      // Selections and Z-Jet dR Cleaning
      if(jtpt_corr[ijet]<30) continue;
      if(abs(jteta[ijet])>2.5) continue;
      h_jet_etaphi_before->Fill(jteta[ijet], jtphi[ijet], scale);
      // Check Jet Veto Map: if bin content > 0, the jet is in a vetoed region
      if (h_jet_veto_map->GetBinContent(h_jet_veto_map->FindBin(jteta[ijet], jtphi[ijet])) > 0) continue;
      h_jet_etaphi_after->Fill(jteta[ijet], jtphi[ijet], scale);
      detaMinus = jteta[ijet] - muMinus.Eta();
      dphiMinus = RelativePhi(jtphi[ijet], muMinus.Phi());
      dRMinus = TMath::Sqrt(detaMinus * detaMinus + dphiMinus * dphiMinus);
      detaPlus = jteta[ijet] - muPlus.Eta();
      dphiPlus = RelativePhi(jtphi[ijet], muPlus.Phi());
      dRPlus = TMath::Sqrt(detaPlus * detaPlus + dphiPlus * dphiPlus);
      if(dRMinus < 0.2 || dRPlus < 0.2 ) continue;
      njets++;
      //cout << "-----------------------------" << endl;
      //cout << "ijet: " << ijet << " pt = " << jtpt_corr[ijet] << " eta = " << jteta[ijet] << " phi = " << jtphi[ijet] << " m = " << jtm[ijet] << endl;
      //cout << "muMinus pt = " << muMinus.Pt() << " eta = " << muMinus.Eta() << " phi = " << muMinus.Phi() << endl;
      //cout << "muPlus pt = " << muPlus.Pt() << " eta = " << muPlus.Eta() << " phi = " << muPlus.Phi() << endl;
      //cout << "dRMinus = " << dRMinus << " dRPlus = " << dRPlus << endl;

      // --- Matching Logic for CURRENT Reco Jet (ijet) ---
      double min_dR = 9999.0;
      int matched_gen_jet_idx = -1;

      // Loop over Gen Jets to find the best match for the current reco jet
      if (!isData) {
        for (int igenjet = 0; igenjet < *ngen; igenjet++) {
          // gen jet cuts looser than reco jet cuts
          if(genpt[igenjet]<20.) continue;
          if(abs(geneta[igenjet])>3.) continue;

          double deta_gen = jteta[ijet] - geneta[igenjet];
          double dphi_gen = RelativePhi(jtphi[ijet], genphi[igenjet]);
          double dR_gen = TMath::Sqrt(deta_gen * deta_gen + dphi_gen * dphi_gen);

          // min_dR tracks the smallest dR found for the current reco jet
          // matched_gen_jet_idx stores the index of the gen jet that yielded the min_dR.
          if (dR_gen < min_dR) {
            min_dR = dR_gen;
            matched_gen_jet_idx = igenjet;
          }
        }
      }

      if (ijetLeading == -1 || jtpt_corr[ijet] > jtpt_corr[ijetLeading]) {
          ijetLeading = ijet;
          // Check if a match was found within a reasonable dR cone
          if (!isData) {
            if (min_dR < 0.1) {
              isLeadingJetMatched = true;
              iGenjetMatchedtoLeadingReco = matched_gen_jet_idx;
            }
            else {
              isLeadingJetMatched = false;
              iGenjetMatchedtoLeadingReco = -1; // Reset if no match
            }
          }
      }
    } //end loop over jets
    //cout << "ijetLeading = " << ijetLeading << endl;

    if (ijetLeading != -1) {
        double dPhi_Zj = RelativePhi(Z_phi, jtphi[ijetLeading]);
        double xZj = jtpt_corr[ijetLeading]/Z_pt;
        //Remove overflow and put it in the last bin
        //if (xZj > xZj_max) xZj = xZj_max - 0.01;
        h_deltaPhi_Zj->Fill(dPhi_Zj, scale);

        // --- Determine the current bin number for MinBias matching ---
        int current_global_bin_n = -1;
        
        if (binning_option == 0) { // HF binning
            float current_val = *hiHF;
            int bin_n = 0;
            for (const auto& bin_range : primary_bins) {
                if (current_val >= bin_range.first && current_val < bin_range.second) {
                    current_global_bin_n = bin_n;
                    break;
                }
                bin_n++;
            }
        } else if (binning_option == 1) { // VZ binning only
            float current_val = *vz;
            int bin_n = 0;
            for (const auto& bin_range : primary_bins) {
                if (current_val >= bin_range.first && current_val < bin_range.second) {
                    current_global_bin_n = bin_n;
                    break;
                }
                bin_n++;
            }
        } else if (binning_option == 2) { // VZ + Centrality binning
            int cen_bin_idx = -1;
            int vz_bin_idx = -1;

            for (size_t c_bin_n = 0; c_bin_n < centrality_bins_combined.size(); ++c_bin_n) {
                const auto& bin_range = centrality_bins_combined[c_bin_n];
                if (hiBin_to_use >= bin_range.first && hiBin_to_use < bin_range.second) {
                    cen_bin_idx = c_bin_n;
                    break;
                }
            }

            for (size_t v_bin_n = 0; v_bin_n < vz_bins_combined.size(); ++v_bin_n) {
                const auto& bin_range = vz_bins_combined[v_bin_n];
                if (*vz >= bin_range.first && *vz < bin_range.second) {
                    vz_bin_idx = v_bin_n;
                    break;
                }
            }
            if (cen_bin_idx != -1 && vz_bin_idx != -1) {
                current_global_bin_n = cen_bin_idx * BinningConfig_Combined_Vz_Centrality::num_vz_bins + vz_bin_idx;
            } else {
                // This event doesn't fall into a defined combined bin, skip background subtraction for it
                current_global_bin_n = -1; 
            }
        }
        // --- End of bin determination ---

        if (current_global_bin_n != -1) { // Only proceed with MinBias matching if a valid bin was found
            // Loop over the TTree entries for mixing events with MinBias
            // This assumes the MinBias tree 'bin_MinBias' corresponds to 'current_global_bin_n'
            // and contains 'events_per_mixed_bin_limit' events for each of these bins.
            double events_filled_for_this_bin_in_MinBias = 0; 
            for(int iEntry=0; iEntry< nEntries_MinBias; iEntry++){
                inputTree->GetEntry(iEntry); // Read all branch values for the current entry
                if (current_global_bin_n == bin_MinBias) { // Match by global bin number
                    // Determine weight for this specific bin
                    // If we have 100 events, weight is 1/100. If we have 37, weight is 1/37.
                    double n_mix = mb_counts[current_global_bin_n];
                    double mixing_weight = (n_mix > 0) ? (1.0 / n_mix) : 0.0;
                    // Apply same jet cuts as for signal jets
                    if (jet_pt_MinBias >= 30 && abs(jet_eta_MinBias) <= 2.5) {
                        // Ensure background is not estimated from the bad detector region
                        if (h_jet_veto_map->GetBinContent(h_jet_veto_map->FindBin(jet_eta_MinBias, jet_phi_MinBias)) == 0) {
                          double detaMinus_MinBias = jet_eta_MinBias - muMinus.Eta();
                          double dphiMinus_MinBias = RelativePhi(jet_phi_MinBias, muMinus.Phi());
                          double dRMinus_MinBias = TMath::Sqrt(detaMinus_MinBias * detaMinus_MinBias + dphiMinus_MinBias * dphiMinus_MinBias);
                          double detaPlus_MinBias = jet_eta_MinBias - muPlus.Eta();
                          double dphiPlus_MinBias = RelativePhi(jet_phi_MinBias,muPlus.Phi());
                          double dRPlus_MinBias = TMath::Sqrt(detaPlus_MinBias * detaPlus_MinBias + dphiPlus_MinBias * dphiPlus_MinBias);
                          if (dRMinus_MinBias >= 0.2 && dRPlus_MinBias >= 0.2 ) {
                              double dPhi_Zj_MinBias = RelativePhi(Z_phi, jet_phi_MinBias);
                              double xZj_MinBias = jet_pt_MinBias/Z_pt;
                              //Remove overflow and put it in the last bin
                              //if (xZj_MinBias > xZj_max) xZj_MinBias = xZj_max - 0.01;
                              h_deltaPhi_Zj_MinBias->Fill(dPhi_Zj_MinBias, scale * mixing_weight);
                              if (dPhi_Zj_MinBias > 7 * TMath::Pi() / 8) {
                                  h_jet_pt_lj_MinBias->Fill(jet_pt_MinBias, scale * mixing_weight);
                                  h_xZj_MinBias->Fill(xZj_MinBias, scale * mixing_weight);
                                  if (itotev < 0.7*Ngen) h_xZj_MinBias_train_closure->Fill(xZj_MinBias, scale * mixing_weight);
                                  else h_xZj_MinBias_test_closure->Fill(xZj_MinBias, scale * mixing_weight);
                                  if (!isData) {
                                    if (ijetGenLeading_unfold != -1) {
                                      if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                                        //if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
                                          h_response_MinBias->Fill(xZj_MinBias, true_xZj, scale * mixing_weight);
                                          if (itotev < 0.7*Ngen) h_response_MinBias_closure->Fill(xZj_MinBias, true_xZj, scale * mixing_weight);
                                        //}
                                      }
                                    }
                                  }
                              }
                          }
                        }
                    }
                    events_filled_for_this_bin_in_MinBias++;
                }
            }
            if (events_filled_for_this_bin_in_MinBias != events_per_mixed_bin_limit) {
                std::cout << "--- Warning! MinBias bin " << current_global_bin_n << " has only " << events_filled_for_this_bin_in_MinBias
                          << " events (expected " << events_per_mixed_bin_limit << "). ---" << std::endl;
            }
        }

        if (!isData) {
            if (isLeadingJetMatched) {
              double dPhi_Zj_matched = RelativePhi(Z_phi, jtphi[ijetLeading]);
              h_deltaPhi_Zj_matched->Fill(dPhi_Zj_matched, scale);
          }
        }

        if (dPhi_Zj > 7 * TMath::Pi() / 8) {
          h_njet->Fill(njets, scale);
          h_mumu_j->Fill(Z_mass, scale);
          h_Z_pt_j->Fill(Z_pt, scale);
          h_jet_pt_lj->Fill(jtpt_corr[ijetLeading], scale);
          h_xZj->Fill(xZj, scale);
          h_xZj_fixbinw->Fill(xZj, scale);
          if (!isData) {
            if (ijetGenLeading_unfold != -1) {
              if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                h_response_unmatched->Fill(xZj, true_xZj, scale);
                if (itotev < 0.7*Ngen) h_response_closure_unmatched->Fill(xZj, true_xZj, scale);
              }
            }
          }
          if (itotev < 0.7*Ngen) h_xZj_train_closure->Fill(xZj, scale);
          else h_xZj_test_closure->Fill(xZj, scale);
          h_cen_j->Fill((hiBin_to_use)/2, scale);
          h_HF_j->Fill(*hiHF, scale);

          if (!isData) {
            if (isLeadingJetMatched) {
              h_jet_pt_lj_matched->Fill(jtpt_corr[ijetLeading], scale);
              h_xZj_matched->Fill(xZj, scale);
            }
          }

          //h_jetgirth->Fill(jtgirth[ijetLeading], scale);
          //h_jet_deltaR->Fill(jtdyndeltaR[ijetLeading], scale);
        }

        // --- Fill information for unfolding ---
        if (RelativePhi(Z_phi, jtphi[ijetLeading]) > 7 * TMath::Pi() / 8) {
          if (!isData) {
            if (ijetGenLeading_unfold != -1) {
              if (dPhi_Zj_Gen > 7 * TMath::Pi() / 8) {
                // Calculate true x_Zj
                // Check if the leading gen jet is matched to leading reconstructed jet
                if (ijetGenLeading_unfold == iGenjetMatchedtoLeadingReco) {
                  h_response->Fill(xZj, true_xZj, scale);
                  h_xZj_reco->Fill(xZj, scale);
                  if (itotev < 0.7*Ngen) {
                    h_response_closure->Fill(xZj, true_xZj, scale);
                    h_xZj_train_closure_matched->Fill(xZj, scale);
                  }
                  else h_xZj_test_closure_matched->Fill(xZj, scale);
                }
              }
            }
          }
        } // --- end filling information for unfolding ---
    }  // end reco leading jet selection
  }  // end loop events

  // Finalize histograms for Mixed event subtraction
  // Scale by the number of events per bin that were collected in the MinBias file
  //int scale_binning = 1;
  //if (binning_option == 0) {scale_binning = BinningConfig::ev_per_bin; cout << "HF matching for mixed event bkg subtraction" << endl;}
  //else if (binning_option == 1) {scale_binning = BinningConfig_vz::ev_per_bin; cout << "vz matching for mixed event bkg subtraction" << endl;}
  //else if (binning_option == 2) {scale_binning = BinningConfig_Combined_Vz_Centrality::ev_per_combined_bin; cout << "Combined vz + Centrality matching for mixed event bkg subtraction" << endl;}

  //h_deltaPhi_Zj_MinBias->Scale(1. / scale_binning);

  TH1F* h_deltaPhi_Zj_subtracted = (TH1F*)h_deltaPhi_Zj->Clone("h_deltaPhi_Zj_subtracted");
  h_deltaPhi_Zj_subtracted->SetDirectory(0);
  h_deltaPhi_Zj_subtracted->SetTitle("h_deltaPhi_Zj - h_deltaPhi_Zj_MinBias (rescaled)");
  h_deltaPhi_Zj_subtracted->Add(h_deltaPhi_Zj_MinBias, -1); // The -1 performs the subtraction

  //h_jet_pt_lj_MinBias->Scale(1. / scale_binning);

  TH1F* h_jet_pt_lj_subtracted = (TH1F*)h_jet_pt_lj->Clone("h_jet_pt_lj_subtracted");
  h_jet_pt_lj_subtracted->SetDirectory(0);
  h_jet_pt_lj_subtracted->SetTitle("h_jet_pt_lj - h_jet_pt_lj_MinBias (rescaled)");
  h_jet_pt_lj_subtracted->Add(h_jet_pt_lj_MinBias, -1); // The -1 performs the subtraction

  cout << "Bkg Integral (dPhi): " << h_deltaPhi_Zj_MinBias->Integral(0, h_deltaPhi_Zj_MinBias->GetNbinsX()+1) << endl;
  cout << "Bkg Integral (pT): " << h_jet_pt_lj_MinBias->Integral(0, h_jet_pt_lj_MinBias->GetNbinsX()+1) << " fraction: "
       << h_jet_pt_lj_MinBias->Integral(0, h_jet_pt_lj_MinBias->GetNbinsX()+1)/h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
       << endl;

  if (!isData) {
    cout << "Raw - True (pT): " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) - h_jet_pt_lj_matched->Integral(0, h_jet_pt_lj_matched->GetNbinsX()+1)
         << " fraction: " << (h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1) - h_jet_pt_lj_matched->Integral(0, h_jet_pt_lj_matched->GetNbinsX()+1))/h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
         << endl;
  }

  //h_xZj_MinBias->Scale(1. / scale_binning);

  TH1F* h_xZj_subtracted = (TH1F*)h_xZj->Clone("h_xZj_subtracted");
  h_xZj_subtracted->SetDirectory(0);
  h_xZj_subtracted->SetTitle("h_xZj - h_xZj_MinBias (rescaled)");
  h_xZj_subtracted->Add(h_xZj_MinBias, -1); // The -1 performs the subtraction

  //h_response_MinBias->Scale(1. / scale_binning);

  TH2F* h_response_subtracted = (TH2F*)h_response_unmatched->Clone("h_response_subtracted");
  h_response_subtracted->SetDirectory(0);
  h_response_subtracted->SetTitle("h_response_unmatched - h_response_MinBias (rescaled)");
  h_response_subtracted->Add(h_response_MinBias, -1); // The -1 performs the subtraction

  //h_xZj_MinBias_train_closure->Scale(1. / scale_binning);
  //h_xZj_MinBias_test_closure->Scale(1. / scale_binning);

  TH1F* h_xZj_train_closure_subtracted = (TH1F*)h_xZj_train_closure->Clone("h_xZj_train_closure_subtracted");
  h_xZj_train_closure_subtracted->SetDirectory(0);
  h_xZj_train_closure_subtracted->SetTitle("h_xZj_train_closure - h_xZj_MinBias_train_closure (rescaled)");
  h_xZj_train_closure_subtracted->Add(h_xZj_MinBias_train_closure, -1); // The -1 performs the subtraction

  TH1F* h_xZj_test_closure_subtracted = (TH1F*)h_xZj_test_closure->Clone("h_xZj_test_closure_subtracted");
  h_xZj_test_closure_subtracted->SetDirectory(0);
  h_xZj_test_closure_subtracted->SetTitle("h_xZj_test_closure - h_xZj_MinBias_test_closure (rescaled)");
  h_xZj_test_closure_subtracted->Add(h_xZj_MinBias_test_closure, -1); // The -1 performs the subtraction

  //h_response_MinBias_closure->Scale(1. / scale_binning);

  TH2F* h_response_closure_subtracted = (TH2F*)h_response_closure_unmatched->Clone("h_response_closure_subtracted");
  h_response_closure_subtracted->SetDirectory(0);
  h_response_closure_subtracted->SetTitle("h_response_closure_unmatched - h_response_MinBias_closure (rescaled)");
  h_response_closure_subtracted->Add(h_response_MinBias_closure, -1); // The -1 performs the subtraction

  cout << "Number of events = " << h_jet_pt_lj->Integral(0, h_jet_pt_lj->GetNbinsX()+1)
       << ", if Z_pt>80: " << h_Z_pt_j->Integral(h_Z_pt->FindBin(80), h_Z_pt->GetNbinsX()+1) << endl;
  cout << "tot ev = " << itotev << endl;

  c1->cd(1);
  h_mumu->Draw();
  c2->cd(1);
  h_Z_pt->Draw();
  c3->cd(1);
  //h_njet->Draw();
  //h_avg_rho_vs_cen->Draw();
  h_HF_j->Draw();
  c4->cd(1);
  h_jet_pt_lj->Draw();
  c5->cd(1);
//  h_cen->Draw();
  h_xZj->Draw();
  if (!isData) h_xZj_true->SetLineColor(3); h_xZj_true->Draw("same");
  c6->cd(1);
  //h_vz->Draw();
  if (!isData) h_response->Draw("COLZTEXT");
  c7->cd(1);
  h_avg_rho->Draw();
  //c1->SaveAs("h_mumu_j.pdf");

  // Create the main directory "MC" or "DATA"
  TDirectory *Dir = file_output_HI_mu->mkdir("HI");
  // Navigate to the directory
  Dir->cd();
  // Create a new directory named "Muons"
  TDirectory *muonsDir = Dir->mkdir("Muons");
  // Navigate to the "MUONS" directory
  muonsDir->cd();
  h_mumu->Write();
  h_Z_pt->Write();
  h_njet->Write();
  h_cen->Write();
  h_mumu_j->Write();
  h_Z_pt_j->Write();
  h_jet_pt_lj->Write();
  h_jet_pt_lj_MinBias->Write();
  h_jet_pt_lj_subtracted->Write();
  h_jet_pt_lj_matched->Write();
  h_cen_j->Write();
  h_deltaPhi_Zj->Write();
  h_deltaPhi_Zj_MinBias->Write();
  h_deltaPhi_Zj_subtracted->Write();
  h_deltaPhi_Zj_matched->Write();
  h_xZj->Write();
  h_xZj_fixbinw->Write();
  h_jet_etaphi_before->Write();
  h_jet_etaphi_after->Write();
  h_xZj_MinBias->Write();
  h_xZj_subtracted->Write();
  h_xZj_matched->Write();
  h_vz->Write();
  h_avg_rho->Write();
  //h_jetgirth->Write();
  //h_jet_deltaR->Write();

  // Write unfolding specific histograms - NEW
  if (!isData) {
    h_xZj_true->Write();
    h_xZj_for_JEWEL_w->Write();
    h_mumu_true->Write();
    h_xZj_reco->Write();
    h_response->Write();
    h_response_unmatched->Write();
    h_response_MinBias->Write();
    h_response_subtracted->Write();
    h_xZj_train_closure->Write();
    h_xZj_train_closure_matched->Write();
    h_xZj_test_closure->Write();
    h_xZj_test_closure_matched->Write();
    h_xZj_true_train_closure->Write();
    h_xZj_true_test_closure->Write();
    h_response_closure->Write();
    h_response_closure_unmatched->Write();
    h_response_MinBias_closure->Write();
    h_response_closure_subtracted->Write();
    h_xZj_MinBias_train_closure->Write();
    h_xZj_MinBias_test_closure->Write();
    h_xZj_train_closure_subtracted->Write();
    h_xZj_test_closure_subtracted->Write();
  }
  file_output_HI_mu->Close();
}

