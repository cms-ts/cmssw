import subprocess
import sys

# 1. Point to the CERN redirector
SERVER = "eoscms.cern.ch"
# 2. Point to the ppref data path
BASE_DIR = "/eos/cms/store/group/phys_heavyions/nbarnett/HiForest/MC/2024ppref/QCD_pThat-15to1200_TuneCP5_5p36TeV_pythia8/crab_foresting_2024ppref_MC_v2/"

print(f"Starting safe crawl of {BASE_DIR}...")

with open("samples_root_files_ppref.txt", "w") as f:
    try:
        top_dirs = subprocess.check_output(["xrdfs", SERVER, "ls", BASE_DIR]).decode().splitlines()
    except subprocess.CalledProcessError:
        print("Error reaching server. Did you run voms-proxy-init?")
        sys.exit(1)

    def crawl(path):
        try:
            items = subprocess.check_output(["xrdfs", SERVER, "ls", path]).decode().splitlines()
            for item in items:
                if item.endswith(".root"):
                    # Standardize the CERN XRootD prefix
                    f.write(f"root://{SERVER}/{item}\n")
                elif "." not in item.split("/")[-1]: 
                    crawl(item)
        except subprocess.CalledProcessError:
            pass 

    for d1 in top_dirs:
        print(f"Scanning {d1}...")
        crawl(d1)

print("\nSuccess! All files saved to samples_root_files_ppref_JER.txt")
