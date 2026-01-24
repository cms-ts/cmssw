#!/bin/bash

# Description: Runs MinBias analysis for Data and MC across all binning options.
# Options:
#   0: HF Binning
#   1: VZ Binning
#   2: Combined VZ + Centrality Binning

# Exit immediately if a command exits with a non-zero status
set -e

echo "================================================="
echo "  Starting MinBias Library Production"
echo "================================================="

# --- Loop through options 0, 1, and 2 ---
for OPT in 0 1 2
do
    echo ""
    echo "-------------------------------------------------"
    case $OPT in
        0) echo "Processing OPTION 0 (HF Binning)" ;;
        1) echo "Processing OPTION 1 (VZ Binning)" ;;
        2) echo "Processing OPTION 2 (Combined VZ + Centrality)" ;;
    esac
    echo "-------------------------------------------------"

    # 1. Run on Data (true)
    echo "  [Data] Running analyze_MinBias_TTreeReader.C(true, $OPT)..."
    root -l -b -q "analyze_MinBias_TTreeReader.C(true, $OPT)"
    
    # 2. Run on MC (false)
    echo "  [MC]   Running analyze_MinBias_TTreeReader.C(false, $OPT)..."
    root -l -b -q "analyze_MinBias_TTreeReader.C(false, $OPT)"

    echo "  -> Option $OPT Complete."
done

echo ""
echo "================================================="
echo "  All jobs finished successfully."
echo "================================================="
