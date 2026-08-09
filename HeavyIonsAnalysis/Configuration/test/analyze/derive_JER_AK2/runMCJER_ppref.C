#include "plots.h"
#include "MCJER.C"
#include "MCJPR.C"
#include "doTxtMCJER.C"
#include "TSystem.h" // Needed to create directories

void runMCJER_ppref(int radius = 2) { 
  bool useTrimmedRMS = true; 

  // Make names dynamic based on radius
  TString inFile = Form("AK%d_ZJet_forjer_ppref_FINAL.root", radius);
  TString outFile = Form("MCJER-AK%d-ppref.root", radius);
  TString txtFile = Form("My_ppref24_MC_PtResolution_AK%dPF.txt", radius);
  
  // Create a dynamic directory name (e.g., MCJER_ppref_AK2 or MCJER_ppref_AK3)
  TString outDir = Form("MCJER_ppref_AK%d", radius);
  gSystem->MakeDirectory(outDir.Data()); // Creates the folder automatically!

  // 1. Point to our MERGED ppref file and pass the dynamic directory
  MCJER(inFile, 15, outDir.Data(), outFile, useTrimmedRMS);

  // 2. Generate the final text file
  doTxtMCJER(outFile.Data(), txtFile.Data(), false);
}
