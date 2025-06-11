#ifndef BINNIN_CONFIG_H
#define BINNIN_CONFIG_H

struct BinningConfig {
    // Total number of HF bins you want to define.
    static const int tot_bins = 25;//38;

    // The maximum number of leading jet events to collect per HF bin.
    static const int ev_per_bin = 100;//200;

    // The starting HF value for your very first bin.
    static constexpr float frst_bin_min = 700.0f;//200.0f;
};

#endif // BINNIN_CONFIG_H
