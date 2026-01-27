#!/bin/bash

echo "Analyzing data"
root -l -b -q 'analyze_ppref_TTreeReader_ZMM_stack.C("data")'

input_name=("signal" "TT" "WminusToMuminusNu" "DYto2Tau" "singleT" "Tbar" "WplusToMuplusNu")

for k in "${input_name[@]}"; do
  echo "Analyzing MC: $k"  # Optional: Print which histogram is being plotted
  root -l -b -q "analyze_ppref_TTreeReader_ZMM_stack.C(\"$k\")"  # Call ROOT with the label as an argument
done

echo "Plotting complete." # Optional: Confirmation message

