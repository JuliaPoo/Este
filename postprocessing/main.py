from format_helper import DataParser
import os
import json
import pathlib

# Make sure the post processing is ran in current directory
CUR_DIR = os.path.dirname(os.path.realpath(__file__))
os.chdir(CUR_DIR)

IN_DIR = pathlib.Path(r"..\gen\Este-out")
OUT_DIR = pathlib.Path(r"..\web\gen")

def getPids() -> list:
    files = os.listdir(IN_DIR)
    all_prefix = set(f.split('.')[0] for f in files)
    all_pids = [int(f[3:]) for f in all_prefix]
    return all_pids

def main():

    all_pids = getPids()
    with open(OUT_DIR / "pids.json", "w") as f:
        json.dump(all_pids, f)

    for pid in all_pids:
        db = DataParser(IN_DIR, pid)
        db.outputToJson(OUT_DIR / f"pid_{pid}_este.json")

if __name__ == "__main__":
    main()