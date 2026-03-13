#!/bin/bash

# Get Collision Type from argument (default to PbPb23 if empty)
COLLISION=${1:-PbPb23}

# Usage:
# ./do_analyze_Minbias.sh          (Defaults to PbPb23)
# ./do_analyze_Minbias.sh PbPb24

# Description: Runs MinBias analysis for Data and MC across all binning options.
# Now updated to run across distinct centrality intervals (0-30, 30-50, 50-90) for VZ and Combined options.
# Collision Type:
#   Defaults to PbPb23
#   PbPb24
# Options:
#   0: HF Binning (Centrality cuts ignored)
#   1: VZ Binning (Centrality intervals applied)
#   2: Combined VZ + Centrality Binning (Centrality intervals applied)

# Exit immediately if a command exits with a non-zero status
set -e

echo "================================================="
echo "  Starting MinBias Library Production"
echo "  Collision Type: $COLLISION"
echo "================================================="

# Define the centrality intervals we want to run for Options 1 and 2
# Format: "min max"
CENT_INTERVALS=("0 30" "30 90")

# --- Loop through options 0, 1, and 2 ---
for OPT in 0 1 2
do
    echo ""
    echo "-------------------------------------------------"
    case $OPT in
        0) echo "Processing OPTION 0 (HF Binning) - Centrality ignored" ;;
        1) echo "Processing OPTION 1 (VZ Binning) - Iterating Centrality" ;;
        2) echo "Processing OPTION 2 (Combined VZ + Centrality) - Iterating Centrality" ;;
    esac
    echo "-------------------------------------------------"

    if [ "$OPT" -eq 0 ]; then
        # For HF Binning, we only need to run once. We pass 0 100 as dummy centrality bounds,
        # since the C++ macro logic ignores centrality bounds when OPT=0.
        echo "  [Data] Running analyze_MinBias_TTreeReader.C(\"$COLLISION\", true, $OPT, 0, 100)..."
        root -l -b -q "analyze_MinBias_TTreeReader.C(\"$COLLISION\", true, $OPT, 0, 100)"
        
        echo "  [MC]   Running analyze_MinBias_TTreeReader.C(\"$COLLISION\", false, $OPT, 0, 100)..."
        root -l -b -q "analyze_MinBias_TTreeReader.C(\"$COLLISION\", false, $OPT, 0, 100)"
    else
        # For VZ and Combined Binning, loop through the defined centrality intervals
        for INTERVAL in "${CENT_INTERVALS[@]}"; do
            # Read min and max from the interval string
            read -r CENT_MIN CENT_MAX <<< "$INTERVAL"
            
            echo "  >>> Centrality Interval: ${CENT_MIN}-${CENT_MAX}% <<<"
            
            # 1. Run on Data (true)
            echo "  [Data] Running analyze_MinBias_TTreeReader.C(\"$COLLISION\", true, $OPT, $CENT_MIN, $CENT_MAX)..."
            root -l -b -q "analyze_MinBias_TTreeReader.C(\"$COLLISION\", true, $OPT, $CENT_MIN, $CENT_MAX)"
            
            # 2. Run on MC (false)
            echo "  [MC]   Running analyze_MinBias_TTreeReader.C(\"$COLLISION\", false, $OPT, $CENT_MIN, $CENT_MAX)..."
            root -l -b -q "analyze_MinBias_TTreeReader.C(\"$COLLISION\", false, $OPT, $CENT_MIN, $CENT_MAX)"
        done
    fi

    echo "  -> Option $OPT Complete."
done

echo ""
echo "================================================="
echo "  All jobs finished successfully."
echo "================================================="
