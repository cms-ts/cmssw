#ifndef BINNIN_CONFIG_H
#define BINNIN_CONFIG_H

// Configuration for HF (Hadronic Forward) energy binning
struct BinningConfig {
    // Total number of HF bins you want to define.
    static const int tot_bins = 25; // Or 38, depending on your final decision
    // The maximum number of leading jet events to collect per HF bin.
    static const int ev_per_bin = 100; // Or 200
    // The starting HF value for your very first bin.
    static constexpr float frst_bin_min = 700.0f; // Or 200.0f
};

// Configuration for Vz (primary vertex z-coordinate) only binning
struct BinningConfig_vz {
    // Total number of vz bins you want to define.
    // This assumes VZ is the *only* binning variable.
    static const int tot_bins = 3;
    // The maximum number of leading jet events to collect per vz bin.
    static const int ev_per_bin = 1000;
    // The starting vz value for your very first bin.
    static constexpr float frst_bin_min = -15.0f;
    // You might also want to define a fixed bin width or max value if not using current_min * 1.1 or +10
    // static constexpr float vz_bin_width = 10.0f; // Example: if all bins are 10cm wide
};

// Configuration for combined Vz and Centrality binning
struct BinningConfig_Combined_Vz_Centrality {
    // Total number of centrality bins (e.g., 30 bins from 0-30% centrality)
    //static const int num_centrality_bins = 30; // 0-30% -> hiBin 0-60 (60/30 = 2 hiBin per bin)
    //static constexpr float centrality_min_hiBin = 0.0f;
    //static constexpr float centrality_max_hiBin = 60.0f; // Corresponds to 30% centrality

    // We will define the min/max and number of bins dynamically in the macro now.
    // Only keep the Vz specific configs and event limits here.

    // Total number of vz bins (e.g., 3 bins)
    static const int num_vz_bins = 3;
    // Explicit Vz bin edges
    // Using an array to define the edges directly, useful for non-uniform bins
    static constexpr float vz_bin_edges[] = {-15.0f, -5.0f, 5.0f, 15.0f};
    // The size of the array should be num_vz_bins + 1

    // The maximum number of leading jet events to collect per *combined* bin.
    // Total bins will be num_centrality_bins * num_vz_bins (e.g., 30 * 3 = 90)
    static const int ev_per_combined_bin = 100; // Adjust as needed
};

#endif // BINNIN_CONFIG_H
