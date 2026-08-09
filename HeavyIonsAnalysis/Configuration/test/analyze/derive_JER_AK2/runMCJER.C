#include "plots.h"
#include "MCJER.C"
#include "MCJPR.C"
#include "doTxtMCJER.C"
#include "TSystem.h" // Needed to create directories

void runMCJER(int radius = 2) { 
  bool useTrimmedRMS = true; 

  // Make names dynamic based on radius
  TString inFile = Form("AK%d_ZJet_forjer_FINAL.root", radius);
  TString outFile = Form("MCJER-AK%d.root", radius);
  TString txtFile = Form("My_PbPb23_MC_PtResolution_AK%dPF.txt", radius);
  
  // Create a dynamic directory name (e.g., MCJER_AK2 or MCJER_AK3)
  TString outDir = Form("MCJER_AK%d", radius);
  gSystem->MakeDirectory(outDir.Data()); // Creates the folder automatically!

  // 1. Point to our MERGED file and pass the dynamic directory
  MCJER(inFile, 15, outDir.Data(), outFile, useTrimmedRMS);

  // 2. Generate the final text file
  doTxtMCJER(outFile.Data(), txtFile.Data(), false);
}
