#!/bin/bash

# Get Collision Type from argument (default to PbPb23 if empty)
COLLISION=${1:-PbPb23}

# Usage:
# ./do_plot_MinBias.sh         (Defaults to PbPb23)
# ./do_plot_MinBias.sh PbPb24

# Description: Runs MinBias analysis for Data and MC across all binning options.
# Collision Type:
#   Defaults to PbPb23
#   PbPb24

# Exit immediately if a command exits with a non-zero status
set -e

echo "================================================="
echo "  Plotting MinBias subtraction "
echo "  Collision Type: $COLLISION"
echo "================================================="


histogram_labels=("h_jet_pt_lj" "h_deltaPhi_Zj" "h_xZj")

for label in "${histogram_labels[@]}"; do
  echo "Plotting histogram: \"$COLLISION\", $label, true"  # Optional: Print which histogram is being plotted
  root -l -b -q "plot_MinBias.C(\"$COLLISION\", \"$label\", true)"  # Call ROOT with the label as an argument
done

for label in "${histogram_labels[@]}"; do
  echo "Plotting histogram: \"$COLLISION\",  $label, false"  # Optional: Print which histogram is being plotted
  root -l -b -q "plot_MinBias.C(\"$COLLISION\", \"$label\", false)"  # Call ROOT with the label as an argument
done

echo "================================================="
echo "  Plotting complete." # Optional: Confirmation message
echo "================================================="
