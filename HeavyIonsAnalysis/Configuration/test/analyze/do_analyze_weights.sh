#!/bin/bash

# Starting directory
ORIGINAL_DIR=$(pwd)

echo "Step 0"
echo "data"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("data", 0, 0)'
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("signal", 0, 0)'
echo "plot"
cd weights_MC/Ncoll_weights_0/
root -l -b -q Ncoll_weight_0.C
cd "$ORIGINAL_DIR"

echo "Step 1"
echo "data"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("data", 1, 0)'
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("signal", 1, 0)'
echo "plot and compute rho weights"
cd weights_MC/Ncoll_weights_0/
root -l -b -q 'Ncoll_weight_0.C(1)'
cd ../rho_weights_1/
root -l -b -q rho_weight_1.C
cd "$ORIGINAL_DIR"

echo "Step 2"
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("signal", -1, 0)'
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("signal", 2, 0)'
echo "plot and compute vz weights"
cd weights_MC/rho_weights_1/
root -l -b -q 'rho_weight_1.C(1)'
cd ../vz_weights_2/
root -l -b -q vz_weight_2.C
cd "$ORIGINAL_DIR"

echo "Step 3"
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("signal", 3, 0)'
echo "plot"
cd weights_MC/vz_weights_2//
root -l -b -q 'vz_weight_2.C(1)'
cd ../final_weight_3/
root -l -b -q JEWEL_weight_3.C
root -l -b -q 'JEWEL_weight_3.C(1)'
cd "$ORIGINAL_DIR"
echo "Complete." # Optional: Confirmation message
