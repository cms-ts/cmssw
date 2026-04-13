#!/bin/bash

sleep 4h

# List of configuration files
configs=(
    "crab_run3_ppref_MC_ZMM_ZZTo4L.py"
    "crab_run3_ppref_MC_ZMM_ZZTo2L2Nu.py"
    "crab_run3_ppref_MC_ZMM_WWTo2L2Nu.py"
    "crab_run3_ppref_MC_ZMM_T.py"
    "crab_run3_ppref_MC_ZMM_TT.py"
    "crab_run3_ppref_MC_ZMM_Tbar.py"
    "crab_run3_ppref_MC_ZMM_DYto2Tau.py"
    "crab_run3_ppref_MC_ZMM_WminusToMuminusNu.py"
    "crab_run3_ppref_MC_ZMM_WplusToMuplusNu.py"
)

for cfg in "${configs[@]}"; do
    echo "--------------------------------------------------"
    echo "Submitting: $cfg"
    echo "Time: $(date)"
    
    # Run the CRAB submission
    crab submit -c "$cfg"
    
    echo "Submission finished. Waiting 2 hours..."
    # Sleep for 2 hours (7200 seconds)
    sleep 2h
done

echo "All tasks have been submitted."
