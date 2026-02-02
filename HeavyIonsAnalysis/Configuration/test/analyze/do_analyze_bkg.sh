#!/bin/bash
# Wrapper script for analyzing Background MC samples using the final analysis step (Phase 3).
# Dynamically reads sample labels from MC_samples.h

# --- CONFIGURATION SECTION ---
# Uncomment ONE of the following lines to select the collision type
#COLLISION="PbPb23"
COLLISION="ppref24"
#COLLISION="PbPb24"


echo "------------------------------------------------"
echo "Analyzing backgrounds for: $COLLISION"
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
# - Splits by quotes (") and takes the 6th field (path="1", out="3", label="5" -> actually index 6 in 1-based split includes quotes)
# - Filters out "signal"
echo "Reading sample list from MC_samples.h (Vector: $CPP_VECTOR)..."

input_name=($(awk -v vname="$CPP_VECTOR" '
    # Find start of the specific vector definition
    $0 ~ "const std::vector<FileInfo> " vname {in_block=1; next} 

    # Stop at the end of the vector
    in_block && /};/ {exit}

    # Process lines inside the block
    in_block {
        # Skip commented lines (start with optional whitespace then //)
        if ($0 ~ /^[ \t]*\/\//) next

        # Check if line contains quotes (valid entry)
        if ($0 ~ /"/) {
            # Split line by quotes.
            # Field 2 = path, Field 4 = output file, Field 6 = Label
            split($0, arr, "\"")
            label = arr[6]

            # Print label if it is valid and not signal
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
  # Syntax: analyze_HI_TTreeReader_ZMM.C(collision, sample_name, weight_phase, systFlag)
  root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"$k\", 3, 0)"
done

echo "------------------------------------------------"
echo "Background analysis complete."
echo "------------------------------------------------"
