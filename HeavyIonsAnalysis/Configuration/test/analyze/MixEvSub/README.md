# MinBias Library Producer for Mixed Event Subtraction

This directory contains the macros and scripts required to produce ntuples from Minimum Bias (MinBias) events.
These ntuples create a library used to perform **Mixed Event Background Subtraction** in the main Z+Jet analysis.


## 0. Shortcuts

```bash
# Run the script (Default PbPb23)
./do_analyze_Minbias.sh
# For PbPb24
./do_analyze_Minbias.sh PbPb24

# Plotting results (Default PbPb23)
./do_plot_MinBias.sh
# For PbPb24
./do_plot_MinBias.sh PbPb24
```


## 1. Producing MinBias Ntuples

The main analysis macro is `analyze_MinBias_TTreeReader.C`.

### Arguments
The macro accepts five arguments:
`void analyze_MinBias_TTreeReader(const char* year_str, bool isData, int use_binning_option, int cent_min, int cent_max)`

| Parameter | Type | Description |
| :--- | :--- | :--- |
| **`year_str`** |`const char*` | `PbPb23` for PbPb23, `PbPb24` for PbPb24. (Default: `PbPb23`)|
| **`isData`** | `bool` | `true` for Data, `false` for MC. (Default: `true`) |
| **`binning_option`** | `int` | **`0`**: HF Binning<br>**`1`**: VZ Binning<br>**`2`**: Combined VZ + Centrality Binning (Default) |
| `cent_min` | `int` | Minimum centrality percentage. (Default: `0`) | | `cent_max` | `int` | Maximum centrality percentage. (Default: `30`) |
---

### Usage Examples

To produce all files at once, across all centrality intervals (0-30, 30-50, 50-90), use the provided shell script.

```bash
# Ensure the script is executable
chmod +x do_analyze_Minbias.sh

# Run the script (Default PbPb23)
./do_analyze_Minbias.sh
# For PbPb24
./do_analyze_Minbias.sh PbPb24
```

**Collision Type PbPb23: (Default)**
Produces files with prefix `MinBias`.

```bash
# Run on Data (Default arguments: PbPb23, Data, Opt 2, 0-30%)
root -l -b -q analyze_MinBias_TTreeReader.C
# Output: MinBias_leading_jets_data_VZ_Cen_Combined_Cen0_30.root

# Run on MC (Default Opt 2, 0-30%)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false)'
# Output: MinBias_leading_jets_MC_VZ_Cen_Combined_Cen0_30.root
```

**Collision Type PbPb24**
Produces files with prefix `MinBias_HI24`.
```bash
# Run on Data (Default arguments)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb24", true)'
# Output: MinBias_HI24_leading_jets_data_VZ_Cen_Combined_Cen0_30.root

# Run on MC
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb24", false)'
# Output: MinBias_HI24_leading_jets_MC_VZ_Cen_Combined_Cen0_30.root
```

**Option 0: HF Binning**
Produces files with suffix `_HF.root`. Centrality arguments are ignored by the macro logic for HF.

```bash
# Run on Data 2023 (Passing 0, 100 as dummy centrality bounds)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", true, 0, 0, 100)'
# Output: MinBias_leading_jets_data_HF.root

# Run on MC 2023
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false, 0, 0, 100)'
# Output: MinBias_leading_jets_MC_HF.root
```

**Option 1: VZ Binning**
Produces files with `suffix _VZ_Cen[min]_[max].root`.

```bash
# Run on Data 2023 (0-30% centrality)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", true, 1, 0, 30)'
# Output: MinBias_leading_jets_data_VZ_Cen0_30.root

# Run on MC 2023 (30-50% centrality)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false, 1, 30, 50)'
# Output: MinBias_leading_jets_MC_VZ_Cen30_50.root
```

**Option 2: Combined VZ + Centrality Binning (Default)**
Produces files with suffix `_VZ_Cen_Combined_Cen[min]_[max].root`. 

```bash
# Run on Data 2023 (50-90% centrality)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", true, 2, 50, 90)'
# Output: MinBias_leading_jets_data_VZ_Cen_Combined_Cen50_90.root

# Run on MC 2023 (30-50% centrality)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false, 2, 30, 50)'
# Output: MinBias_leading_jets_MC_VZ_Cen_Combined_Cen30_50.root
```


## 2. Plotting Results

Use the following macros to validate the Background Subtraction steps.

### Single Histogram Plotting

Use `plot_MinBias.C` to plot specific variables (e.g., `h_xZj`).
*Note: The macro arguments are `(collision_type, histogram_name, isData, collision_type, cent_min, cent_max, ptZ_min, ptZ_max)`.*

```bash
# Plot 'h_xZj' for Data (PbPb23, 0-30%, pT^Z > 40)
root -l -b -q 'plot_MinBias.C("PbPb23", "h_xZj", true, 0, 30, 40.0, 9999.0)'

# Plot 'h_xZj' for MC (PbPb24, 0-30%, pT^Z > 40)
root -l -b -q 'plot_MinBias.C("PbPb23", "h_xZj", false, 0, 30, 40.0, 9999.0)'

# Plot 'h_xZj' for a different bin 
root -l -b -q 'plot_MinBias.C("PbPb23", "h_xZj", true, 30, 50, 40.0, 9999.0)'
```

### Batch Plotting

To generate all validation histograms at once, use the provided shell script.

```bash
# Ensure the script is executable
chmod +x do_plot_MinBias.sh

# Run for PbPb23, 0-30%, pT^Z > 40 (Default)
./do_plot_MinBias.sh

# Specific Centrality 30-50%
./do_plot_MinBias.sh PbPb23 30 50 40 9999

# Specific pT^Z range 60 80
./do_plot_MinBias.sh PbPb23 0 30 60 80
```

