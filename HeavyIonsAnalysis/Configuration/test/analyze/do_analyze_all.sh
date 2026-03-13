#!/bin/bash

# --- ppref24 Defaults ---
./do_analyze_weights.sh ppref24 0 30 40 9999
./do_analyze_bkg.sh ppref24 0 30 40 9999
./do_analyze_syst.sh ppref24 0 30 40 9999
# --- ppref24 pT Intervals ---
./do_analyze_weights.sh ppref24 0 30 40 50
./do_analyze_bkg.sh ppref24 0 30 40 50
./do_analyze_syst.sh ppref24 0 30 40 50

./do_analyze_weights.sh ppref24 0 30 50 60
./do_analyze_bkg.sh ppref24 0 30 50 60
./do_analyze_syst.sh ppref24 0 30 50 60

./do_analyze_weights.sh ppref24 0 30 60 80
./do_analyze_bkg.sh ppref24 0 30 60 80
./do_analyze_syst.sh ppref24 0 30 60 80

./do_analyze_weights.sh ppref24 0 30 80 120
./do_analyze_bkg.sh ppref24 0 30 80 120
./do_analyze_syst.sh ppref24 0 30 80 120

# --- PbPb23 Defaults ---
./do_analyze_weights.sh PbPb23 0 30 40 9999
./do_analyze_bkg.sh PbPb23 0 30 40 9999
./do_analyze_syst.sh PbPb23 0 30 40 9999
# --- PbPb23 pT Intervals (Centrality 0-30%) ---
./do_analyze_weights.sh PbPb23 0 30 40 50
./do_analyze_bkg.sh PbPb23 0 30 40 50
./do_analyze_syst.sh PbPb23 0 30 40 50

./do_analyze_weights.sh PbPb23 0 30 50 60
./do_analyze_bkg.sh PbPb23 0 30 50 60
./do_analyze_syst.sh PbPb23 0 30 50 60

./do_analyze_weights.sh PbPb23 0 30 60 80
./do_analyze_bkg.sh PbPb23 0 30 60 80
./do_analyze_syst.sh PbPb23 0 30 60 80

./do_analyze_weights.sh PbPb23 0 30 80 120
./do_analyze_bkg.sh PbPb23 0 30 80 120
./do_analyze_syst.sh PbPb23 0 30 80 120
# --- PbPb23 Centrality Intervals (pT > 40) ---
./do_analyze_weights.sh PbPb23 30 90 40 9999
./do_analyze_bkg.sh PbPb23 30 90 40 9999
./do_analyze_syst.sh PbPb23 30 90 40 9999

echo "All analysis variations completed!"
