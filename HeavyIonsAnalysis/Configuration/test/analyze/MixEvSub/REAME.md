# MinBias Library Producer for Mixed Event Subtraction

This directory contains the macros and scripts required to produce ntuples from Minimum Bias (MinBias) events.
These ntuples create a library used to perform **Mixed Event Background Subtraction** in the main Z+Jet analysis.


## 1. Producing MinBias Ntuples

The main analysis macro is `analyze_MinBias_TTreeReader.C`.

### Arguments
The macro accepts two arguments:
`void analyze_MinBias_TTreeReader(bool isData, int use_binning_option)`

| Parameter | Type | Description |
| :--- | :--- | :--- |
| **`isData`** | `bool` | `true` for Data, `false` for MC. (Default: `true`) |
| **`binning_option`** | `int` | **`0`**: HF Binning (Default)<br>**`1`**: VZ Binning<br>**`2`**: Combined VZ + Centrality Binning |

---

### Usage Examples

**Option 0: HF Binning (Default)**
Produces files with suffix `_HF.root`.

```bash
# Run on Data (Default arguments)
root -l -b -q analyze_MinBias_TTreeReader.C
# Output: MinBias_leading_jets_data_HF.root

# Run on MC
root -l -b -q 'analyze_MinBias_TTreeReader.C(false)'
# Output: MinBias_leading_jets_MC_HF.root
```

**Option 1: VZ Binning**
Produces files with suffix `_VZ.root`.

```bash
# Run on Data
root -l -b -q 'analyze_MinBias_TTreeReader.C(true, 1)'
# Output: MinBias_leading_jets_data_VZ.root

# Run on MC
root -l -b -q 'analyze_MinBias_TTreeReader.C(false, 1)'
# Output: MinBias_leading_jets_MC_VZ.root
```

**Option 2: Combined VZ + Centrality Binning (Recommended)**
Produces files with suffix `_VZ_Cen_Combined.root`.

```bash
# Run on Data
root -l -b -q 'analyze_MinBias_TTreeReader.C(true, 2)'
# Output: MinBias_leading_jets_data_VZ_Cen_Combined.root

# Run on MC
root -l -b -q 'analyze_MinBias_TTreeReader.C(false, 2)'
# Output: MinBias_leading_jets_MC_VZ_Cen_Combined.root
```


## 2. Plotting Results

Use the following macros to validate the Background Subtraction steps.

### Single Histogram Plotting

Use `plot_MinBias.C` to plot specific variables (e.g., `h_xZj`).

```bash
# Plot 'h_xZj' for Data
root -l -b -q 'plot_MinBias.C("h_xZj", true)'

# Plot 'h_xZj' for MC
root -l -b -q 'plot_MinBias.C("h_xZj", false)'
```

### Batch Plotting

To generate all validation histograms at once, use the provided shell script.

```bash
# Ensure the script is executable
chmod +x do_plot_MinBias.sh

# Run the script
./do_plot_MinBias.sh
```

