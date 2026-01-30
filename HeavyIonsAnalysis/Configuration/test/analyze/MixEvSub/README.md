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
The macro accepts three arguments:
`void analyze_MinBias_TTreeReader(const char* year_str, bool isData, int use_binning_option)`

| Parameter | Type | Description |
| :--- | :--- | :--- |
| **`year_str`** |`const char*` | `PbPb23` for PbPb23, `PbPb24` for PbPb24. (Default: `PbPb23`)|
| **`isData`** | `bool` | `true` for Data, `false` for MC. (Default: `true`) |
| **`binning_option`** | `int` | **`0`**: HF Binning<br>**`1`**: VZ Binning<br>**`2`**: Combined VZ + Centrality Binning (Default) |
---

### Usage Examples

To produce all files at once, use the provided shell script.

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
# Run on Data (Default arguments)
root -l -b -q analyze_MinBias_TTreeReader.C
# Output: MinBias_leading_jets_data_HF.root

# Run on MC
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false)'
# Output: MinBias_leading_jets_MC_HF.root
```

**Collision Type PbPb24**
Produces files with prefix `MinBias_HI24`.
```bash
# Run on Data (Default arguments)
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb24", true)'
# Output: MinBias_HI24_leading_jets_data_HF.root

# Run on MC
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb24", false)'
# Output: MinBias_HI24_leading_jets_MC_HF.root
```

**Option 0: HF Binning**
Produces files with suffix `_HF.root`.

```bash
# Run on Data 2023
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", true, 0)'
# Output: MinBias_leading_jets_data_HF.root

# Run on MC 2023
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false, 0)'
# Output: MinBias_leading_jets_MC_HF.root
```

**Option 1: VZ Binning**
Produces files with suffix `_VZ.root`.

```bash
# Run on Data 2023
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", true, 1)'
# Output: MinBias_leading_jets_data_VZ.root

# Run on MC 2023
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false, 1)'
# Output: MinBias_leading_jets_MC_VZ.root
```

**Option 2: Combined VZ + Centrality Binning (Default)**
Produces files with suffix `_VZ_Cen_Combined.root`.

```bash
# Run on Data 2023 (Default arguments)
root -l -b -q analyze_MinBias_TTreeReader.C
# Output: MinBias_leading_jets_data_VZ_Cen_Combined.root

# Run on MC 2023
root -l -b -q 'analyze_MinBias_TTreeReader.C("PbPb23", false, 2)'
# Output: MinBias_leading_jets_MC_VZ_Cen_Combined.root
```


## 2. Plotting Results

Use the following macros to validate the Background Subtraction steps.

### Single Histogram Plotting

Use `plot_MinBias.C` to plot specific variables (e.g., `h_xZj`).
*Note: The macro arguments are `(histogram_name, isData, collision_type)`.*

```bash
# Plot 'h_xZj' for Data (PbPb23)
root -l -b -q 'plot_MinBias.C("PbPb23", "h_xZj", true)'

# Plot 'h_xZj' for MC (PbPb24)
root -l -b -q 'plot_MinBias.C("PbPb23", "h_xZj", false)'
```

### Batch Plotting

To generate all validation histograms at once, use the provided shell script.

```bash
# Ensure the script is executable
chmod +x do_plot_MinBias.sh

# Run for PbPb23 (Default)
./do_plot_MinBias.sh

# Run for PbPb24
./do_plot_MinBias.sh PbPb24
```

