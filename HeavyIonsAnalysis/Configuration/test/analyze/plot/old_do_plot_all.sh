#!/bin/bash

histogram_labels=("h_mumu" "h_Z_pt" "h_njet" "h_cen" "h_mumu_j" "h_Z_pt_j" "h_jet_pt_lj" "h_cen_j" "h_deltaPhi_Zj" "h_xZj" "h_vz" "h_avg_rho")

for label in "${histogram_labels[@]}"; do
  echo "Plotting histogram: $label"  # Optional: Print which histogram is being plotted
  root -l -b -q "old_plot.C(\"$label\")"  # Call ROOT with the label as an argument
done

echo "Plotting complete." # Optional: Confirmation message
