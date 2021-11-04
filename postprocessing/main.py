from format_helper import ParsedProcess
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
    
    pid_tid_map = {}
    for pid in all_pids:

        proc = ParsedProcess(IN_DIR, pid)
        all_tids = []
        for thread in proc.threads:

            ptid = thread.pin_tid
            otid = thread.os_tid
            all_tids.append({
                "pin_tid": ptid,
                "os_tid": otid
            })

            thread.outputToJson(
                OUT_DIR / f"pid{pid}_tid{otid}_ptid{ptid}.json"
            )

        pid_tid_map[pid] = all_tids

    with open(OUT_DIR / "pid_tid_map.json", 'w') as f:
        json.dump(pid_tid_map, f)
    

if __name__ == "__main__":
    main()