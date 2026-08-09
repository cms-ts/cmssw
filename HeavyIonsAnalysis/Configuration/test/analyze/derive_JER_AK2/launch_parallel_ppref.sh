#!/bin/bash

# Accept radius as first argument, default to 2 if not provided
RADIUS=${1:-2}

# Define the base EOS path dynamically based on the radius
case $RADIUS in
    2)
        BASE="/eos/infnts/cms/store/user/rdelliga/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/CRAB3_Analysis_test25_run3_ppref_MC_QCD/260615_213332"
        ;;
    3)
        BASE="/eos/infnts/cms/store/user/rdelliga/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/CRAB3_Analysis_test25_run3_ppref_MC_QCD_AK3/260713_092300"
        ;;
    4)
        # Future placeholder for AK4
        BASE="/eos/infnts/cms/store/user/rdelliga/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/CRAB3_Analysis_test25_run3_ppref_MC_QCD_AK4/YOUR_AK4_TIMESTAMP"
        ;;
    *)
        echo "Error: Directory for radius AK${RADIUS} is not configured in this script."
        exit 1
        ;;
esac

echo "Launching 9 parallel ROOT jobs for AK${RADIUS} ppref jets..."
echo "Using path: $BASE"

for i in {1..9}; do
    echo "Processing chunk ${i}..." 
    nohup root -l -b -q "derive_JER_ppref.C(\"${BASE}/0000/HiForestMiniAOD_${i}*.root\", \"AK${RADIUS}_forjer_ppref_0000_${i}.root\", ${RADIUS})" > "log_ppref_0000_${i}.txt" 2>&1 &
done

echo "All ppref jobs submitted to the background!"
echo "Check progress with: tail -f log_ppref_0000_1.txt"
