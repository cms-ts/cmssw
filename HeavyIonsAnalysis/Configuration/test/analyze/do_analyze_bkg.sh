#!/bin/bash
# Wrapper script for analyzing Background MC samples (TT, WZ, ZZ, etc.).
# Iterates through a defined list of background processes and runs the final analysis step (Phase 3).

# --- CONFIGURATION SECTION ---
# Uncomment ONE of the following lines to select the collision type
#COLLISION="PbPb23"
COLLISION="ppref24"
#COLLISION="PbPb24"

echo "------------------------------------------------"
echo "Analyzing backgrounds for: $COLLISION"
echo "------------------------------------------------"

# List of Background MC samples to process
# Ensure these labels match the ones defined in your MC_samples.h
input_name=(
    "TT"
    "WZto2L2Q"
    "ZZto2L2Q"
    "ZZto2L2Nu"
    "WZto3LNu"
    "WminusToMuminusNu"
    "WWto2L2Nu"
    "DYto2Tau"
    "ZZto4L"
    "singleT"
    "Tbar"
    "WplusToMuplusNu"
)

# Loop over each background sample
for k in "${input_name[@]}"; do
  echo "-> Analyzing MC Sample: $k"
  # Run the analyzer in Phase 3 (Final Analysis with weights applied)
  # Syntax: analyze_HI_TTreeReader_ZMM.C(collision, sample_name, weight_phase, systFlag)
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"$k\", 3, 0)"
done

echo "------------------------------------------------"
echo "Background analysis complete."
echo "------------------------------------------------"
