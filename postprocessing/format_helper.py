import json
import csv
from collections import Counter
from pathlib import Path

class DataParser():
    def __init__(self, directory:str, pid:int):

        self.directory:Path = Path(directory)
        self.pid:int = pid
        self.nodes:list = self._loadBB(self.directory / f"pid{self.pid}.bb.csv")
        self.links:int = self._loadTrace(self.directory / f"pid{self.pid}.trace.csv")

    @staticmethod
    def _loadBB(filename:str) -> list:
        nodes = list()
        with open(filename) as f:
            reader = csv.reader(f, delimiter=',', quotechar='|')
            headers = next(reader)
            for row in reader:
                headers[0] = "id" # change from idx to id
                nodes.append(dict(zip(headers, row)))
        return nodes

    @staticmethod
    def _loadTrace(filename):
        links = list()
        with open(filename) as f:
            reader = csv.reader(f, delimiter=',', quotechar='|')
            headers = next(reader)
            previous = 0
            for row in reader:
                if not previous == 0:
                    link = previous
                    link += "," + row[0]
                    links.append(link)
                previous = row[0]
        count = Counter(links)
        result = list()
        for link in count:
            src,dst = link.split(",")
            result.append({"source":src, "target":dst, "count":count[link]})
        return result

    @staticmethod
    def _loadBin(filename):
        with open(filename) as f:
            data = json.load(f)
    
    def nodesToStr(self):
        return json.dumps(self.nodes, indent = 4, sort_keys=True)

    def linksToStr(self):
        return json.dumps(self.links, indent = 4, sort_keys=True)

    def outputToJson(self, filename):
        data = {"nodes":self.nodes, "links":self.links}
        with open(filename, 'w', encoding='utf-8') as f:
            json.dump(data, f)