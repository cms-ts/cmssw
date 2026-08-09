#!/bin/bash
# Wrapper script for Z+Jet analysis workflow for weight computation (Ncoll, Rho, Vz).
# Automatically handles data/MC loops, skip Ncoll/Rho reweighting for ppref collisions.

# --- ARGUMENT SECTION ---
# Usage: ./do_analyze_weights.sh [collision] [cent_min] [cent_max] [ptZ_min] [ptZ_max]
# Default: PbPb23 0 30 40 9999
COLLISION=${1:-"PbPb23"}
CENT_MIN=${2:-0}
CENT_MAX=${3:-30}
PTZ_MIN=${4:-40.0}
PTZ_MAX=${5:-9999.0}

# Starting directory
ORIGINAL_DIR=$(pwd)

echo "------------------------------------------------"
echo "Running analysis for: $COLLISION"
echo "Kinematics: Cent $CENT_MIN-$CENT_MAX%, ptZ $PTZ_MIN-$PTZ_MAX"
echo "------------------------------------------------"

echo "=== Step 3: Final Analysis & JEWEL Weights ==="
echo "-> Running Final MC (Phase 3)..."
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"

echo "-> Plotting Vz (check) and computing JEWEL Weights..."
cd weights_MC/vz_weights_2/
root -l -b -q "vz_weight_2.C(\"$COLLISION\", 1, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
cd "$ORIGINAL_DIR"

# We need to run alternative for MC model syst studies in ppref
if [[ "$COLLISION" == *"ppref24"* ]]; then
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"alternative\", 3, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
fi

cd weights_MC/final_weight_3/
root -l -b -q "JEWEL_weight_3.C(\"$COLLISION\", 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
cd "$ORIGINAL_DIR"

echo "Complete."
