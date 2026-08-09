#!/bin/bash
# Wrapper script for Z+Jet Systematics Analysis, runs Nominal Data/MC first, then iterates over
# defined systematic variations (JEC, JER, SF, etc.), adapted based on collision type (PbPb vs ppref).
# NOW PARALLELIZED: Runs Nominal Data/MC and all systematic variations simultaneously.

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

# Create a logs directory to keep your workspace clean
mkdir -p syst_logs

# ==============================================================================
#  STEP 1: NOMINAL RUNS (Baseline)
# ==============================================================================
echo "=== Step 1: Launching Nominal Baselines ==="

echo "-> Submitting Nominal Data..."
nohup root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)" > syst_logs/log_data_nom.txt 2>&1 &

echo "-> Submitting Nominal MC..."
nohup root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)" > syst_logs/log_mc_nom.txt 2>&1 &

# ==============================================================================
#  STEP 2: SYSTEMATIC VARIATIONS
# ==============================================================================
echo "=== Step 2: Launching Systematic Variations ==="

# --- Define Systematics Lists ---
# MC Systematics (Applied to all collision types)
# 1,2,15,16,17,18: Muon SF Up/Down
# 4: Prior Model
# 8: Binning (MC)
# 9,10: JEC Down/Up
# 11,12: JER Down/Up

numbers_MC="1 2 4 8 9 10 11 12 15 16 17 18 19 20"

# Data Systematics (Collision Dependent)
if [[ "$COLLISION" == *"PbPb"* ]]; then
    numbers_data="6 7 8 20"
    # PbPb: Include Centrality variations (6, 7) + Binning (8)
else
    numbers_data="8 20"
    # ppref: Centrality not applicable, only Binning (8)
fi

# --- Run MC Systematics ---
echo "-> Submitting MC Systematics: [ $numbers_MC ]"
for i in $numbers_MC; do
  nohup root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, $i, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)" > syst_logs/log_mc_syst_${i}.txt 2>&1 &
done

# --- Run Data Systematics ---
echo "-> Submitting Data Systematics: [ $numbers_data ]"
for k in $numbers_data; do
  nohup root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, $k, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)" > syst_logs/log_data_syst_${k}.txt 2>&1 &
done

# ==============================================================================
#  WAIT FOR ALL JOBS TO FINISH
# ==============================================================================
echo "------------------------------------------------"
echo "All ROOT jobs submitted to the background!"
echo "Waiting for them to finish... (You can monitor progress in the syst_logs/ directory)"
echo "------------------------------------------------"

# The 'wait' command blocks the script from continuing until all background jobs (&) complete.
wait

echo "=== All ROOT jobs finished successfully! ==="

# ==============================================================================
#  STEP 3: POST-PROCESSING
# ==============================================================================
echo "=== Step 3: Post-Processing ==="

echo "-> Plotting after applying MC re-weighting"
cd weights_MC/final_weight_3/ || exit
root -l -b -q "JEWEL_weight_3.C(\"$COLLISION\", 1, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)" > ../../syst_logs/log_jewel_weight.txt 2>&1
cd - > /dev/null # Return to original dir silently

echo "-> Update Z_Acceptance"
cd plot/ || exit
root -l -b -q extract_acceptance.C > ../syst_logs/log_extract_acc.txt 2>&1
cd - > /dev/null # Return to original dir silently

echo "------------------------------------------------"
echo "All systematics processing and plotting finished."
echo "------------------------------------------------"
