import json, csv
import pathlib
import capstone as cs
from typing import Tuple, List

class EsteDataHandler:

    _este_arch_to_cs_map = {
        "ARCH_X86": (cs.CS_ARCH_X86, cs.CS_MODE_32),
        "ARCH_X8664": (cs.CS_ARCH_X86, cs.CS_MODE_64)
    }

    def __init__(self, root_dir:pathlib.Path):

        self.root_dir = root_dir
        self._tid_trace_cache = {}
        self._pid_info_cache = {}

        _tmp = json.load(
            open(self.root_dir/"gen/pid_tid_map.json"))
        self.pid_tid_map = dict(
                (int(pid), dict(t.values() for t in _tmp[pid])) 
            for pid in _tmp)

    def _load_node_links(self, pid:int, ptid:int) -> None:

        if (pid, ptid) in self._tid_trace_cache:
            return

        otid = self.pid_tid_map[pid][ptid]
        fn = f"gen/pid{pid}_tid{otid}_ptid{ptid}.json"
        self._tid_trace_cache[(pid, ptid)] = json.load(
            open(self.root_dir/fn))

    def _load_pid_info(self, pid:int) -> None:

        if pid in self._pid_info_cache:
            return

        fn = f"../gen/Este-out/pid{pid}.este.json"
        pid_info = json.load(
            open(self.root_dir/fn))

        fn = f"../gen/Este-out/pid{pid}.rtn.csv"
        rdr = csv.reader(open(fn), delimiter=',', quotechar='|')
        rtn_info = [(h,[]) for h in next(rdr)]
        for r in rdr:
            for (idx, val) in enumerate(r):
                rtn_info[idx][1].append(val)
        pid_info['rtn_info'] = dict(rtn_info)

        self._pid_info_cache[pid] = pid_info

    @staticmethod
    def _get_disassembly(arch:Tuple[int,int], bs:bytes, addr:int) -> List[str]:
        
        md = cs.Cs(*arch)
        return ["0x%x:\t%s\t%s" %(i.address, i.mnemonic, i.op_str) 
            for i in md.disasm(bs, addr)]

    def get_node_details(self, id:int, pid:int, ptid:int) -> dict:

        self._load_node_links(pid, ptid)
        self._load_pid_info(pid)

        pid_info = self._pid_info_cache[pid]
        arch = self._este_arch_to_cs_map[
            pid_info['architecture']]

        tid_trace = self._tid_trace_cache[(pid, ptid)]
        node = tid_trace['nodes'][id]

        # disassembly
        node['disassembly'] = self._get_disassembly(
            arch, bytes.fromhex(node['bytes']), int(node['addr']))

        # image_path
        img_idx = int(node['image_idx'])
        img = pid_info['binaries_loaded'][img_idx]
        node['image_path'] = img['path']

        return node