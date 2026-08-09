#!/bin/bash
# Wrapper script for Z+Jet analysis workflow for weight computation (Ncoll, Rho, Vz).
# Automatically handles data/MC loops, skip Ncoll/Rho reweighting for ppref collisions.

# --- ARGUMENT SECTION ---
# Usage: ./do_analyze_weights.sh [collision] [cent_min] [cent_max] [ptZ_min] [ptZ_max]
# Default: PbPb23 0 30 40 9999
COLLISION=${1:-"PbPb23"}
CENT_MIN=${2:-0}
CENT_MAX=${3:-30}
PTZ_MIN=${4:-40.0}
PTZ_MAX=${5:-9999.0}

# Starting directory
ORIGINAL_DIR=$(pwd)

echo "------------------------------------------------"
echo "Running analysis for: $COLLISION"
echo "Kinematics: Cent $CENT_MIN-$CENT_MAX%, ptZ $PTZ_MIN-$PTZ_MAX"
echo "------------------------------------------------"

# --- CHECK IF PbPb OR ppref ---
if [[ "$COLLISION" == *"PbPb"* ]]; then
    # ==============================================================================
    #  PbPb WORKFLOW (Full Reweighting: Ncoll -> Rho -> Vz -> Final)
    # ==============================================================================

    echo "=== Step 0: Ncoll Weights (PbPb Only) ==="
    echo "-> Running Data..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 0, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    echo "-> Running MC..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 0, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    echo "-> Plotting Ncoll..."
    cd weights_MC/Ncoll_weights_0/
    root -l -b -q "Ncoll_weight_0.C(\"$COLLISION\", 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    cd "$ORIGINAL_DIR"

    echo "=== Step 1: Rho Weights (PbPb Only) ==="
    echo "-> Running Data..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    echo "-> Running MC..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    echo "-> Plotting Ncoll (check) and Computing Rho Weights..."
    cd weights_MC/Ncoll_weights_0/
    root -l -b -q "Ncoll_weight_0.C(\"$COLLISION\", 1, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    cd ../rho_weights_1/
    root -l -b -q "rho_weight_1.C(\"$COLLISION\", 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    cd "$ORIGINAL_DIR"

else
    # ==============================================================================
    #  ppref WORKFLOW (Pileup -> Vz -> Final)
    # ==============================================================================
    echo "=== Step 1: Pileup Weights (ppref only) ==="
    echo "-> Running Data (Phase 1)..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"data\", 1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    
    echo "-> Running MC (Phase 1)..."
    root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    
    echo "-> Computing PU Weights..."
    cd weights_MC/rho_weights_1/
    root -l -b -q "pu_weight_1.C(\"$COLLISION\", 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
    cd "$ORIGINAL_DIR"
fi

# ==============================================================================
#  COMMON WORKFLOW (Vz Weighting -> Final Analysis)
# ==============================================================================

echo "=== Step 2: Vz Weights ==="
echo "-> Running MC (Phase 2 - Calculate Vz Weights)..."

if [[ "$COLLISION" == *"PbPb"* ]]; then
   echo "-> (Optional) Checking Rho reweighting..."
   root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", -1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
   cd weights_MC/rho_weights_1/
   root -l -b -q "rho_weight_1.C(\"$COLLISION\", 1, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
   cd "$ORIGINAL_DIR"
else
   echo "-> (Optional) Checking PU reweighting..."
   root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", -1, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
   cd weights_MC/rho_weights_1/
   root -l -b -q "pu_weight_1.C(\"$COLLISION\", 1, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
   cd "$ORIGINAL_DIR"
fi

# Run Phase 2 to get Vz distributions for reweighting
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 2, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"

echo "-> Computing Vz Weights..."
cd weights_MC/vz_weights_2/
# Pass the collision name to the macro so it knows to look for ppref or PbPb files
root -l -b -q "vz_weight_2.C(\"$COLLISION\", 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
cd "$ORIGINAL_DIR"

echo "=== Step 3: Final Analysis & JEWEL Weights ==="
echo "-> Running Final MC (Phase 3)..."
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"signal\", 3, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"

echo "-> Plotting Vz (check) and computing JEWEL Weights..."
cd weights_MC/vz_weights_2/
root -l -b -q "vz_weight_2.C(\"$COLLISION\", 1, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
cd "$ORIGINAL_DIR"

# We need to run alternative for MC model syst studies in ppref
if [[ "$COLLISION" == *"ppref24"* ]]; then
root -l -b -q "analyze_HI_TTreeReader_ZMM.C(\"$COLLISION\", \"alternative\", 3, 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
fi

cd weights_MC/final_weight_3/
root -l -b -q "JEWEL_weight_3.C(\"$COLLISION\", 0, $CENT_MIN, $CENT_MAX, $PTZ_MIN, $PTZ_MAX)"
cd "$ORIGINAL_DIR"

# ==============================================================================
#  Step 4: Data-Driven UE Swap Efficiencies
# ==============================================================================
echo "=== Step 4: Data-Driven UE Swap Efficiencies ==="

# 0. Format variables to match C++ "%.0f" (removes decimals, e.g., 40.0 -> 40)
PTZ_MIN_INT=${PTZ_MIN%.*}
PTZ_MAX_INT=${PTZ_MAX%.*}

# 1. Build the dynamic tags for file naming
if [[ "$PTZ_MAX" == "9999"* ]]; then
    PT_TAG="_ptZ${PTZ_MIN_INT}_Inf"
else
    PT_TAG="_ptZ${PTZ_MIN_INT}_${PTZ_MAX_INT}"
fi

# 2. Determine System and Prefix
if [[ "$COLLISION" == *"PbPb"* ]]; then
    RUN_TAG="_Cen${CENT_MIN}_${CENT_MAX}${PT_TAG}"
    PREFIX="HI"
    if [[ "$COLLISION" == *"24"* ]]; then PREFIX="HI24"; fi
    DIR_NAME="${PREFIX}/Muons"
    IS_PBPB=1

    echo "-> Running Data-Driven Fits for PbPb..."
    root -l -b -q "compute_swap_efficiency_fit.C(\"plot/output_${PREFIX}_mu_data${RUN_TAG}.root\", \"swap_efficiency_PbPb.root\", \"${DIR_NAME}\")"
    root -l -b -q "compute_swap_efficiency_fit.C(\"plot/output_${PREFIX}_mu_MC_signal${RUN_TAG}.root\", \"swap_efficiency_PbPb_MC.root\", \"${DIR_NAME}\")"
else
    RUN_TAG="${PT_TAG}"
    PREFIX="ppref"
    DIR_NAME="ppref/Muons"
    IS_PBPB=0

    echo "-> Running Data-Driven Fits for pp..."
    root -l -b -q "compute_swap_efficiency_fit.C(\"plot/output_${PREFIX}_mu_data${RUN_TAG}.root\", \"swap_efficiency_ppref.root\", \"${DIR_NAME}\")"
    root -l -b -q "compute_swap_efficiency_fit.C(\"plot/output_${PREFIX}_mu_MC_signal${RUN_TAG}.root\", \"swap_efficiency_ppref_MC.root\", \"${DIR_NAME}\")"
fi

# 3. Generate summary plots and extract Truth histograms
echo "-> Generating Data-Driven Summary Plots and extracting MC Truth..."
root -l -b -q "plot_swap_efficiency_summary.C(${IS_PBPB}, \"${RUN_TAG}\", \"${PREFIX}\")"

echo "Complete."
