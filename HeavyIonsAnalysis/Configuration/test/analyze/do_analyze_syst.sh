#!/bin/bash

echo "Nominal MC and data"

root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("data", 1, 0)'

root -l -b -q 'analyze_HI_TTreeReader_ZMM.C("signal", 3, 0)'

echo "Systematics"

echo "Weight for MC modelling"
cd weights_MC/final_weight_3/
root -l -b -q JEWEL_weight_3.C
cd -

# Define the specific list of numbers
numbers_MC="1 2 4 8 9 10 11 12"
numbers_data="6 7 8"

echo "MC"
# Loop through each number in the list
for i in $numbers_MC
do
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"signal\", 3, $i)"
done

echo "data"
for k in $numbers_data
do
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"data\", 1, $k)"
done

echo "--- All processing finished ---"
