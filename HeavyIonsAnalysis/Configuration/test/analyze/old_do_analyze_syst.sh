#!/bin/bash
# Wrapper script for Z+Jet Systematics Analysis, runs Nominal Data/MC first, then iterates over
# defined systematic variations (JEC, JER, SF, etc.), adapted based on collision type (PbPb vs ppref).

# --- ARGUMENT SECTION ---
# Usage: ./do_analyze_syst.sh [collision] [cent_min] [cent_max] [ptZ_min] [ptZ_max]
# Default: PbPb23 0 30 40 9999
COLLISION=${1:-"PbPb23"}
CENT_MIN=${2:-0}
CENT_MAX=${3:-30}
PTZ_MIN=${4:-40.0}
PTZ_MAX=${5:-9999.0}

echo "------------------------------------------------"
echo "Analyzing systematics variation for: $COLLISION"
echo "Kinematics: Cent $CENT_MIN-$CENT_MAX%, ptZ $PTZ_MIN-$PTZ_MAX"
echo "------------------------------------------------"

# ==============================================================================
#  STEP 1: NOMINAL RUNS (Baseline)
# ==============================================================================
echo "=== Step 0: Running Nominal Baselines ==="

echo "-> Running Nominal Data (Phase 1)..."
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"

echo "-> Running Nominal MC (Phase 3)..."
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"

# ==============================================================================
#  STEP 2: SYSTEMATIC VARIATIONS
# ==============================================================================
echo "=== Step 2: Running Systematic Loops ==="

# --- Define Systematics Lists ---
# MC Systematics (Applied to all collision types)
# 1,2,15,16,17,18: Muon SF Up/Down
# 4: Prior Model
# 8: Binning (MC)
# 9,10: JEC Down/Up
# 11,12: JER Down/Up
numbers_MC="1 2 4 8 9 10 11 12 15 16 17 18"

# Data Systematics (Collision Dependent)
if [[ "$COLLISION" == *"PbPb"* ]]; then
    # PbPb: Include Centrality variations (6, 7) + Binning (8)
    numbers_data="6 7 8"
else
    # ppref: Centrality not applicable, only Binning (8)
    numbers_data="8"
fi

# --- Run MC Systematics ---
echo "-> Loop over MC Systematics: [ $numbers_MC ]"
for i in $numbers_MC; do
  echo "   ... Processing MC Syst Flag: $i"
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, $i, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
done

# --- Run Data Systematics ---
echo "-> Loop over Data Systematics: [ $numbers_data ]"
for k in $numbers_data; do
  echo "   ... Processing Data Syst Flag: $k"
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, $k, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
done

echo "-> Plotting after applying MC re-weighting"
cd weights_MC/final_weight_3/
# Updated to pass: (collision, after_flag, cent_min, cent_max, ptZ_min, ptZ_max)
root -l -b -q "JEWEL_weight_3.C(\"$COLLISION\", 1, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
cd - > /dev/null  # Return to original dir silently

echo "-> Update Z_Acceptance"
cd plot/
root -l -b -q extract_acceptance.C
cd - > /dev/null  # Return to original dir silently

echo "------------------------------------------------"
echo "All systematics processing finished."
echo "------------------------------------------------"
