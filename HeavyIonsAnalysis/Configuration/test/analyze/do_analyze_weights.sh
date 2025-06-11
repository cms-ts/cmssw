#!/bin/bash

echo "Step 0"

echo "data"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C(true, 0)'
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C(false, 0)'
echo "plot"
cd weights_MC/Ncoll_weights_0/
root -l -b -q Ncoll_weight_0.C
cd -

echo "Step 1"
echo "data"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C(true, 1)'
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C(false, 1)'
echo "plot and compute rho weights"
cd weights_MC/rho_weights_1/
root -l -b -q Ncoll_weight_1.C
root -l -b -q rho_weight_1.C
cd -

echo "Step 2"
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C(false, 2)'
echo "plot and compute vz weights"
cd weights_MC/vz_weights_2/
root -l -b -q rho_weight_cut_2.C
root -l -b -q vz_weight_2.C 
cd -

echo "Step 3"
echo "MC"
root -l -b -q 'analyze_HI_TTreeReader_ZMM.C(false, 3)'
echo "plot"
cd weights_MC/final_weights_3/
root -l -b -q vz_weight_3.C
cd -

echo "Complete." # Optional: Confirmation message
