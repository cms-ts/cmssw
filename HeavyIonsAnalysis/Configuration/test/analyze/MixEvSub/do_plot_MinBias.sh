#!/bin/bash

histogram_labels=("h_jet_pt_lj" "h_deltaPhi_Zj" "h_xZj")

for label in "${histogram_labels[@]}"; do
  echo "Plotting histogram: $label, true"  # Optional: Print which histogram is being plotted
  root -l -b -q "plot_MinBias.C(\"$label\", true)"  # Call ROOT with the label as an argument
done

for label in "${histogram_labels[@]}"; do
  echo "Plotting histogram: $label, false"  # Optional: Print which histogram is being plotted
  root -l -b -q "plot_MinBias.C(\"$label\", false)"  # Call ROOT with the label as an argument
done

echo "Plotting complete." # Optional: Confirmation message
