#!/bin/bash

# Accept radius as first argument, default to 2 if not provided
RADIUS=${1:-2}

# Define the base EOS path and the max subdirectory number based on the radius
case $RADIUS in
    2)
        BASE="/eos/infnts/cms/store/user/rdelliga/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/CRAB3_Analysis_test24_mc_QCD/260606_085138"
        MAX_DIR=3 # Folders 0000 to 0003
        ;;
    3)
        BASE="/eos/infnts/cms/store/user/rdelliga/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/CRAB3_Analysis_test24_mc_QCD_AK3/260713_092621"
        MAX_DIR=4 # Folders 0000 to 0004
        ;;
    4)
        # Future placeholder for AK4
        BASE="/eos/infnts/cms/store/user/rdelliga/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/CRAB3_Analysis_test24_mc_QCD_AK4/YOUR_AK4_TIMESTAMP"
        MAX_DIR=0
        ;;
    *)
        echo "Error: Directory for radius AK${RADIUS} is not configured in this script."
        exit 1
        ;;
esac

echo "Launching $((MAX_DIR + 1)) parallel ROOT jobs for AK${RADIUS} jets..."
echo "Using path: $BASE"

# Loop dynamically through the available subdirectories (0000 up to MAX_DIR)
for i in $(seq 0 $MAX_DIR); do
    # Format the number to always have 4 digits (e.g., 0 -> 0000, 1 -> 0001)
    DIR_STR=$(printf "%04d" $i)
    
    echo "Submitting job for directory ${DIR_STR}..."
    nohup root -l -b -q "derive_JER.C(\"${BASE}/${DIR_STR}/*.root\", \"AK${RADIUS}_forjer_${DIR_STR}.root\", ${RADIUS})" > "log_${DIR_STR}.txt" 2>&1 &
done

echo "All jobs submitted to the background!"
echo "Use 'top' to see the running 'root.exe' processes."
echo "Check progress with: tail -f log_0000.txt"
