#!/bin/bash

# List of configuration files
configs=(
    "crab_run3_ppref_DATA_ZMM_PPRefSingleMuon0.py"
    "crab_run3_ppref_DATA_ZMM_PPRefSingleMuon1.py"
    "crab_run3_ppref_DATA_ZMM_PPRefSingleMuon2.py"
    "crab_run3_ppref_DATA_ZMM_PPRefSingleMuon3.py"
)

for cfg in "${configs[@]}"; do
    echo "--------------------------------------------------"
    echo "Submitting: $cfg"
    echo "Time: $(date)"
    
    # Run the CRAB submission
    crab submit -c "$cfg"
    
    echo "Submission finished. Waiting 2 hours..."
    # Sleep for 5 hours
    sleep 5h
done

echo "All tasks have been submitted."
