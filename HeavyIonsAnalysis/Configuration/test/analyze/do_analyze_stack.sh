#!/bin/bash

echo "Analyzing data"
root -l -b -q 'analyze_HI_TTreeReader_ZMM_stack.C("data")'

input_name=("signal" "TT" "WZto2L2Q" "ZZto2L2Q" "WZto3LNu" "WminusToMuminusNu" "WWto2L2Nu" "DYto2Tau" "ZZto4L" "singleT" "singleTbar" "WplusToMuplusNu")

for k in "${input_name[@]}"; do
  echo "Analyzing MC: $k"  # Optional: Print which histogram is being plotted
  root -l -b -q "analyze_HI_TTreeReader_ZMM_stack.C(\"$k\")"  # Call ROOT with the label as an argument
done

echo "Plotting complete." # Optional: Confirmation message
