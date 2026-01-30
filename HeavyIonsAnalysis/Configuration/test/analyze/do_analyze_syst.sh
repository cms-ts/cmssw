#!/bin/bash
# Wrapper script for Z+Jet Systematics Analysis, runs Nominal Data/MC first, then iterates over
# defined systematic variations (JEC, JER, SF, etc.), adapted based on collision type (PbPb vs ppref).

# --- CONFIGURATION SECTION ---
# Uncomment ONE of the following lines to select the collision type
#COLLISION="PbPb23"
#COLLISION="ppref24"
COLLISION="PbPb24"

echo "------------------------------------------------"
echo "Analyzing systematics variation for: $COLLISION"
echo "------------------------------------------------"

# ==============================================================================
#  STEP 0: NOMINAL RUNS (Baseline)
# ==============================================================================
echo "=== Step 0: Running Nominal Baselines ==="

echo "-> Running Nominal Data (Phase 1)..."
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, 0)"

echo "-> Running Nominal MC (Phase 3)..."
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, 0)"

# ==============================================================================
#  STEP 1: PREPARE WEIGHTS (MC Modelling)
# ==============================================================================
echo "=== Step 1: Preparing JEWEL Weights for MC Modelling ==="
cd weights_MC/final_weight_3/
root -l -b -q "JEWEL_weight_3.C(\"$COLLISION\", 0)"
cd - > /dev/null  # Return to original dir silently

# ==============================================================================
#  STEP 2: SYSTEMATIC VARIATIONS
# ==============================================================================
echo "=== Step 2: Running Systematic Loops ==="

# --- Define Systematics Lists ---
# MC Systematics (Applied to all collision types)
# 1,2: Muon SF Up/Down
# 4: Prior Model
# 8: Binning (MC)
# 9,10: JEC Down/Up
# 11,12: JER Down/Up
numbers_MC="1 2 4 8 9 10 11 12"

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
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, $i)"
done

# --- Run Data Systematics ---
echo "-> Loop over Data Systematics: [ $numbers_data ]"
for k in $numbers_data; do
  echo "   ... Processing Data Syst Flag: $k"
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, $k)"
done

echo "-> Plotting after applying JEWEL re-weighting"
cd weights_MC/final_weight_3/
root -l -b -q "JEWEL_weight_3.C(\"$COLLISION\", 1)"
cd - > /dev/null  # Return to original dir silently

echo "------------------------------------------------"
echo "All systematics processing finished."
echo "------------------------------------------------"
