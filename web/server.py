from http.server import HTTPServer, CGIHTTPRequestHandler
from urllib.parse import urlparse, parse_qs
import webbrowser

import json
import logging
import pathlib
import os

# Make sure the server is created at current directory
CUR_DIR = pathlib.Path(os.path.dirname(os.path.realpath(__file__)))
os.chdir(CUR_DIR)

USAGE = "Usage: python %s [int:port number]"

class EsteDataHandler:

    def __init__(self):
        pass

    def get_node_details(self, id, pid, ptid):
        return "[1,2,3]"
        
class EsteServerHandler(CGIHTTPRequestHandler):
    
    _data_handler = EsteDataHandler()

    def _send_error(self, code, error):
        self.send_response(code)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(error.encode())

    def _send_json_header(self, cnt):
        self.send_response(200)
        self.send_header('Content-Length', str(len(cnt)))
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def _handle_GET_node(self, url):

        logging.info(f"GET: {self.path}")
        
        params = parse_qs(url.query)
        id = int(params['id'][0])
        pid = int(params['pid'][0])
        ptid = int(params['pin_tid'][0])

        dets = self._data_handler.get_node_details(id, pid, ptid)

        self._send_json_header(dets)
        self.wfile.write(dets.encode())

    def do_GET(self):

        url = urlparse(self.path)

        print(url.path)
        if url.path == "/callback/node":
            try:
                self._handle_GET_node(url)
            except Exception as e:
                self._send_error(
                    400, f"Error handling GET request to `{self.path}`! {repr(e)}")
            return

        super(EsteServerHandler, self).do_GET()

    
def serve(port):

    '''Starts the webserver'''

    # Create server object listening the port 7777
    srv = HTTPServer(
        server_address = ('', port), 
        RequestHandlerClass = EsteServerHandler)

    try:
        logging.info(f'Starting Este server at port {port}...\n')
        srv.serve_forever()
    except KeyboardInterrupt:
        pass

    logging.info(f'Stopping Este server at port {port}...\n')
    srv.server_close()

def launch(port):

    '''Launches a browser tab'''

    webbrowser.open(f'http://localhost:{port}') 

def main():

    from sys import argv
    logging.basicConfig(level=logging.INFO)

    if len(argv) != 2 or not argv[1].isdigit():
        print(USAGE%argv[0])
        return 1

    port = int(argv[1])
    launch(port)
    serve(port)
    
    return 0

if __name__ == "__main__":
    main()
    