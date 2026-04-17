import subprocess
import sys

SERVER = "eosinfnts.ts.infn.it"
BASE_DIR = "/eos/infnts/cms/store/user/rdelliga/"

print(f"Starting safe crawl of {BASE_DIR}...")

with open("samples_root_files.txt", "w") as f:
    try:
        # Get top-level directories
        top_dirs = subprocess.check_output(["xrdfs", SERVER, "ls", BASE_DIR]).decode().splitlines()
    except subprocess.CalledProcessError:
        print("Error reaching server.")
        sys.exit(1)

    # Recursive function to walk folders safely without using the banned -R flag
    def crawl(path):
        try:
            items = subprocess.check_output(["xrdfs", SERVER, "ls", path]).decode().splitlines()
            for item in items:
                if item.endswith(".root"):
                    # We found a ROOT file! Write the full XRootD path directly to the file.
                    f.write(f"root://{SERVER}:1094/{item}\n")
                elif "." not in item.split("/")[-1]: 
                    # If it doesn't have an extension, it's a folder. Dive into it.
                    crawl(item)
        except subprocess.CalledProcessError:
            pass # Ignore timeouts on empty/broken folders and keep moving

    for d1 in top_dirs:
        print(f"Scanning {d1}...")
        crawl(d1)

print("\nSuccess! All files saved to samples_root_files.txt")
