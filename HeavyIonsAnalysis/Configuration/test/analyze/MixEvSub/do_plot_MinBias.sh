#!/bin/bash

# Get Arguments (Collision Type, CentMin, CentMax, PtZMin, PtZMax)
COLLISION=${1:-PbPb23}
CENT_MIN=${2:-0}
CENT_MAX=${3:-30}
PTZ_MIN=${4:-40.0}
PTZ_MAX=${5:-9999.0}

# Usage:
# ./do_plot_MinBias.sh              (Defaults to PbPb23, 0-30%, pT > 40)
# ./do_plot_MinBias.sh PbPb23 30 50 40 9999

# Description: Runs MinBias plot analysis for Data and MC with dynamic tags.

# Exit immediately if a command exits with a non-zero status
set -e

echo "================================================="
echo "  Plotting MinBias subtraction "
echo "  Collision Type: $COLLISION"
echo "  Kinematics: Cent $CENT_MIN-$CENT_MAX%, ptZ $PTZ_MIN-$PTZ_MAX"
echo "================================================="

histogram_labels=("h_jet_pt_lj" "h_deltaPhi_Zj" "h_xZj")

# --- Plot Data ---
for label in "${histogram_labels[@]}"; do
  echo "Plotting histogram: \"$COLLISION\", $label, true (DATA)"
  root -l -b -q "plot_MinBias.C(\"$COLLISION\", \"$label\", true, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
done

# --- Plot MC ---
for label in "${histogram_labels[@]}"; do
  echo "Plotting histogram: \"$COLLISION\", $label, false (MC)"
  root -l -b -q "plot_MinBias.C(\"$COLLISION\", \"$label\", false, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
done

echo "================================================="
echo "  Plotting complete." 
echo "================================================="
