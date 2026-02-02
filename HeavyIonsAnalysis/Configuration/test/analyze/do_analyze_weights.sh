#!/bin/bash
# Wrapper script for Z+Jet analysis workflow for weight computation (Ncoll, Rho, Vz).
# Automatically handles data/MC loops, skip Ncoll/Rho reweighting for ppref collisions.

# --- COLLISION SECTION ---
# Uncomment ONE of the following lines to select the collision type
#COLLISION="PbPb23"
COLLISION="ppref24"
#COLLISION="PbPb24"

# Starting directory
ORIGINAL_DIR=$(pwd)

echo "------------------------------------------------"
echo "Running analysis for: $COLLISION"
echo "------------------------------------------------"

# --- CHECK IF PbPb OR ppref ---
if [[ "$COLLISION" == *"PbPb"* ]]; then
    # ==============================================================================
    #  PbPb WORKFLOW (Full Reweighting: Ncoll -> Rho -> Vz -> Final)
    # ==============================================================================

echo "=== Step 0: Ncoll Weights (PbPb Only) ==="
    echo "-> Running Data..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 0, 0)"
    echo "-> Running MC..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 0, 0)"
    echo "-> Plotting Ncoll..."
    cd weights_MC/Ncoll_weights_0/
    root -l -b -q "Ncoll_weight_0.C(\"$COLLISION\", 0)"
    cd "$ORIGINAL_DIR"

    echo "=== Step 1: Rho Weights (PbPb Only) ==="
    echo "-> Running Data..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, 0)"
    echo "-> Running MC..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 1, 0)"
    echo "-> Plotting Ncoll (check) and Computing Rho Weights..."
    cd weights_MC/Ncoll_weights_0/
    root -l -b -q "Ncoll_weight_0.C(\"$COLLISION\", 1)"
    cd ../rho_weights_1/
    root -l -b -q "rho_weight_1.C(\"$COLLISION\", 0)"
    cd "$ORIGINAL_DIR"

else
    # ==============================================================================
    #  ppref WORKFLOW (Skip Ncoll/Rho -> Start at Vz)
    # ==============================================================================
    echo "=== Step 0 & 1: Skipped for ppref (No Ncoll/Rho reweighting) ==="
    # We still need to run Data (Phase 1) to produce the output file containing
    # the Vz distribution that Step 2 will compare against.
    echo "-> Running Data (Phase 1) for reference histograms..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, 0)"
fi

# ==============================================================================
#  COMMON WORKFLOW (Vz Weighting -> Final Analysis)
# ==============================================================================

echo "=== Step 2: Vz Weights ==="
echo "-> Running MC (Phase 2 - Calculate Vz Weights)..."

# Note: For PbPb we optionally run Phase -1 (Rho check) here.
# For ppref we skip it because there is no Rho reweighting to check.
if [[ "$COLLISION" == *"PbPb"* ]]; then
   echo "-> (Optional) Checking Rho reweighting..."
   root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", -1, 0)"
   cd weights_MC/rho_weights_1/
   root -l -b -q "rho_weight_1.C(\"$COLLISION\", 1)"
   cd "$ORIGINAL_DIR"
fi

# Run Phase 2 to get Vz distributions for reweighting
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 2, 0)"

echo "-> Computing Vz Weights..."
cd weights_MC/vz_weights_2/
# Pass the collision name to the macro so it knows to look for ppref or PbPb files
root -l -b -q "vz_weight_2.C(\"$COLLISION\", 0)"
cd "$ORIGINAL_DIR"

echo "=== Step 3: Final Analysis & JEWEL Weights ==="
echo "-> Running Final MC (Phase 3)..."
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, 0)"

echo "-> Plotting Vz (check) and computing JEWEL Weights..."
cd weights_MC/vz_weights_2/
root -l -b -q "vz_weight_2.C(\"$COLLISION\", 1)"
cd "$ORIGINAL_DIR"

# We need to run alternative for MC model syst studies in ppref
if [[ "$COLLISION" == *"ppref24"* ]]; then
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"alternative\", 3, 0)"
fi

cd weights_MC/final_weight_3/
root -l -b -q "JEWEL_weight_3.C(\"$COLLISION\", 0)"
cd "$ORIGINAL_DIR"

echo "Complete."
