#!/bin/bash

# 1. Define and create the log directory
LOG_DIR="GenLevel_log"
mkdir -p "$LOG_DIR"

# 2. Parse command-line arguments with sensible defaults
SYSTEM=${1:-"ppref24"}
CENT_MIN=${2:-0}
CENT_MAX=${3:-100}
PTZ_MIN=${4:-40.0}
PTZ_MAX=${5:-9999.0}

# 3. Array of jet radii to process
RADII=(0.2 0.3 0.4)

echo "========================================================"
echo " Starting GenLevel Z+Jets Wrapper"
echo " System:      $SYSTEM"
echo " Centrality:  $CENT_MIN to $CENT_MAX%"
echo " pT^Z Cuts:   $PTZ_MIN to $PTZ_MAX GeV"
echo " Radii:       ${RADII[*]}"
echo " Logs folder: ./$LOG_DIR"
echo "========================================================"

# 4. Loop through each radius and submit via nohup
for R in "${RADII[@]}"; do
    # Format radius string for cleaner filenames (e.g., 0.2 -> 02)
    R_STR=${R//./}

    # Define a descriptive log file name based on your parameters
    LOG_FILE="${LOG_DIR}/log_${SYSTEM}_R${R_STR}_cent${CENT_MIN}_${CENT_MAX}_ptZ${PTZ_MIN}_${PTZ_MAX}.log"

    echo "--> Launching job for R = $R... [Log: $LOG_FILE]"

    # Execute ROOT in batch mode via nohup in the background
    nohup root -l -b -q "analyze_GenLevel_Zjets.C(\"${SYSTEM}\", ${R}, ${CENT_MIN}, ${CENT_MAX}, ${PTZ_MIN}, ${PTZ_MAX})" > "$LOG_FILE" 2>&1 &
    
    # Small pause to avoid potential I/O collisions when initializing trees
    sleep 0.5
done

echo "--------------------------------------------------------"
echo "All 3 jobs have been successfully pushed to the background!"
echo "========================================================"
