#!/bin/bash

echo "Analyzing bkg"

input_name=("TT" "WZto2L2Q" "ZZto2L2Q" "ZZto2L2Nu" "WZto3LNu" "WminusToMuminusNu" "WWto2L2Nu" "DYto2Tau" "ZZto4L" "singleT" "Tbar" "WplusToMuplusNu")

for k in "${input_name[@]}"; do
  echo "Analyzing MC: $k"  # Optional: Print which histogram is being plotted
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$k\", 3, 0)"
done

echo "Plotting complete." # Optional: Confirmation message
