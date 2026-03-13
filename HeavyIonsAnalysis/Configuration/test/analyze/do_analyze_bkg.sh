#!/bin/bash
# Wrapper script for analyzing Background MC samples using the final analysis step (Phase 3).
# Dynamically reads sample labels from MC_samples.h

# --- ARGUMENT SECTION ---
# Usage: ./do_analyze_bkg.sh [collision] [cent_min] [cent_max] [ptZ_min] [ptZ_max]
# Default: PbPb23 0 30 40 9999
COLLISION=${1:-"PbPb23"}
CENT_MIN=${2:-0}
CENT_MAX=${3:-30}
PTZ_MIN=${4:-40.0}
PTZ_MAX=${5:-9999.0}

echo "------------------------------------------------"
echo "Analyzing backgrounds for: $COLLISION"
echo "Kinematics: Cent $CENT_MIN-$CENT_MAX%, ptZ $PTZ_MIN-$PTZ_MAX"
echo "------------------------------------------------"

# 1. Determine which C++ vector to read from the header file
if [ "$COLLISION" == "PbPb23" ]; then
    CPP_VECTOR="files"
elif [ "$COLLISION" == "PbPb24" ]; then
    CPP_VECTOR="files_PbPb24"
elif [ "$COLLISION" == "ppref24" ]; then
    CPP_VECTOR="files_ppref"
else
    echo "Error: Collision type '$COLLISION' not recognized."
    exit 1
fi

# 2. Extract labels dynamically using awk
# - Finds the line starting with the vector definition
# - Reads until the closing brace '};'
# - Skips lines starting with '//'
# - Splits by quotes (") and takes the 6th field
# - Filters out "signal" and "alternative"
echo "Reading sample list from MC_samples.h (Vector: $CPP_VECTOR)..."

input_name=($(awk -v vname="$CPP_VECTOR" '
    # Find start of the specific vector definition
    $0 ~ "const std::vector<FileInfo> " vname {in_block=1; next} 

    # Stop at the end of the vector
    in_block && /};/ {exit}

    # Process lines inside the block
    in_block {
        # Skip commented lines
        if ($0 ~ /^[ \t]*\/\//) next

        # Check if line contains quotes (valid entry)
        if ($0 ~ /"/) {
            # Split line by quotes.
            split($0, arr, "\"")
            label = arr[6]

            # Print label if it is valid and not signal/alternative
            if (label != "" && label != "signal" && label != "alternative") {
                print label
            }
        }
    }
' MC_samples.h))

# 3. Safety Check
if [ ${#input_name[@]} -eq 0 ]; then
    echo "Error: No background samples found for $COLLISION in MC_samples.h!"
    exit 1
fi

echo "Found ${#input_name[@]} background samples: ${input_name[*]}"
echo "------------------------------------------------"

# Loop over each background sample
for k in "${input_name[@]}"; do
  echo "-> Analyzing MC Sample: $k"
  # Run the analyzer in Phase 3 (Final Analysis with weights applied)
  # Updated to pass: (collision, sample, weight_phase, systFlag, cent_min, cent_max, ptZ_min, ptZ_max)
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"$k\", 3, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
done

echo "------------------------------------------------"
echo "Background analysis complete."
echo "------------------------------------------------"
