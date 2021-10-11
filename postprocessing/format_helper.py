import json
import csv
from collections import Counter
from pathlib import Path

from typing import List

class ParsedThread():

    def __init__(self, os_tid:int, pin_tid:int, trace:List[dict], nodes:List[dict]):

        self.nodes:List[dict] = nodes
        self.pin_tid:int = pin_tid
        self.os_tid:int = os_tid
        self._full_trace:List[dict] = trace

        # Split trace up into contiguous sections of
        # whitelisted regions execution
        self._split_trace:List[List[dict]] = self._splitTrace(
            self._full_trace)

        print(self._split_trace)

        # Convert split trace into links
        # for use in plotting
        self.links: List[dict] = self._splitTraceToLinks(
            self._split_trace)

    @staticmethod
    def _splitTrace(trace:List[dict]) -> List[List[dict]]:

        """
        Splits trace into sections of execution
        within whitelisted portions
        """

        split_trace = [[]]
        for bb in trace:

            # Jumped out of whitelisted!
            if int(bb["bb_idx"]) == -1:
                split_trace.append([])
                continue
            split_trace[-1].append(bb)

        return split_trace

    @staticmethod
    def _splitTraceToLinks(split_trace:List[List[dict]]) -> List[dict]:

        """Converts split trace into links"""

        links = list()
        end_trace = len(split_trace)-1
        for trace_id,trace in enumerate(split_trace):

            end_idx = len(trace)-1
            for idx,bb in enumerate(trace):

                print(idx, bb)

                if idx == end_idx:
                    continue

                edge = {}
                edge['target'] = trace[idx+1]['bb_idx']
                edge['source'] = bb['bb_idx']
                
                links.append(tuple(edge.items()))

            # Adds the link between split traces
            if trace_id != end_trace:
                edge = {}
                edge['target'] = split_trace[trace_id+1][0]['bb_idx']
                edge['source'] = trace[-1]['bb_idx']
                edge['non_white'] = 1
                
                links.append(tuple(edge.items()))

        # Remove duplicates
        counts = Counter(links)
        unique_links = list()
        for edge, count in counts.items():
            edge = dict(edge)
            edge['count'] = count
            unique_links.append(edge)

        return unique_links

    def outputToJson(self, filename:str):
        data = {"nodes": self.nodes, "links": self.links}
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(data, f)

class ParsedProcess():

    def __init__(self, directory:str, pid:int):

        self.directory:Path = Path(directory)
        self.pid:int = pid

        # Load all nodes
        self.nodes:List[dict] = self._loadBB(
            self.directory / f"pid{self.pid}.bb.csv")

        # Load trace
        self._process_trace:List[dict] = self._loadTrace(
            self.directory / f"pid{self.pid}.trace.csv")

        # Split trace into different threads
        self.threads:List[ParsedThread] = self._splitTraceToThreads(
            self._process_trace, self.nodes)

    @staticmethod
    def _loadBB(bb_filename:str) -> List[dict]:

        """Loads basic blocks into the graph's nodes"""

        nodes = list()
        with open(bb_filename) as f:
            reader = csv.reader(f, delimiter=',', quotechar='|')
            headers = next(reader)
            for row in reader:
                headers[0] = "id" # change from idx to id
                nodes.append(dict(zip(headers, row)))
        return nodes

    @staticmethod
    def _loadTrace(trace_filename:str) -> List[dict]:

        """Loads trace into a dictionary"""

        trace = list()
        with open(trace_filename) as f:
            reader = csv.reader(f, delimiter=',', quotechar='|')
            headers = next(reader)
            for row in reader:
                trace.append(dict(zip(headers, row)))
        return trace

    @staticmethod
    def _splitTraceToThreads(trace:List[dict], nodes:List[dict]) -> List[ParsedThread]:

        """Splits trace into threads"""

        # Get a list of all tids (os_tid, pin_tid), sorted by pin_tid
        all_tid = [(int(bb["os_tid"]), int(bb["pin_tid"])) for bb in trace]
        all_tid = sorted(list(set(all_tid)), key=lambda bb: bb[1])
        
        # Create a ParsedThread object for each tid
        threads = list()
        for os_tid, pin_tid in all_tid:
            # Get trace for pid
            thread_trace = [bb for bb in trace if int(bb["pin_tid"]) == pin_tid]
            thread_nodes = [nodes[idx] for idx in set(int(bb['bb_idx']) for bb in thread_trace)]
            threads.append(
                ParsedThread(os_tid, pin_tid, thread_trace, thread_nodes))

        return threads
    
    def nodesToStr(self) -> str:
        return json.dumps(self.nodes, indent = 4, sort_keys=True)