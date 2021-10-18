from datahandler import EsteDataHandler

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
        
class EsteServerHandler(CGIHTTPRequestHandler):
    
    _data_handler = EsteDataHandler(CUR_DIR)

    def end_headers(self):
        self.send_header('Cache-Control', 'no-cache, no-store, must-revalidate')
        self.send_header('Pragma', 'no-cache')
        self.send_header('Expires', '0')
        super(EsteServerHandler, self).end_headers()

    def _send_error(self, code:int, error:str) -> None:
        self.send_response(code)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(error.encode())

    def _send_json_header(self, cnt:str) -> None:
        self.send_response(200)
        self.send_header('Content-Length', str(len(cnt)))
        self.send_header('Content-type', 'application/json')
        self.end_headers()

    def _handle_GET_node(self, url:str) -> None:

        params = parse_qs(url.query)
        id = int(params['id'][0])
        pid = int(params['pid'][0])
        ptid = int(params['pin_tid'][0])

        node = self._data_handler.get_node_details(id, pid, ptid)
        node = json.dumps(node).encode()

        self._send_json_header(node)
        self.wfile.write(node)

    def _handle_GET_terminate(self, url:str) -> None:

        self._send_error(200, "Terminating signal received...")
        SERVER.server_close()
        
    def do_GET(self) -> None:

        url = urlparse(self.path)

        if url.path == "/callback/node":
            try:
                self._handle_GET_node(url)
            except Exception as e:
                self._send_error(
                    400, f"Error handling GET request to `{self.path}`! {repr(e)}")
            return
        if url.path == "/callback/terminate":
            self._handle_GET_terminate(url)
            return

        # Hand over to CGIHTTPRequestHandler
        super(EsteServerHandler, self).do_GET()

    
def serve(port:int) -> None:

    '''Starts the webserver'''

    global SERVER

    # Create server object listening the port 7777
    SERVER = HTTPServer(
        server_address = ('', port), 
        RequestHandlerClass = EsteServerHandler)

    try:
        logging.info(f'Starting Este server at port {port}...\n')
        SERVER.serve_forever()
    except KeyboardInterrupt:
        pass
    except Exception as e:
        logging.info(f"Exception: {repr(e)}")

    logging.info(f'Stopping Este server at port {port}...\n')
    SERVER.server_close()

def launch(port:int) -> None:

    '''Launches a browser tab'''

    webbrowser.open(f'http://localhost:{port}') 
    return

def main() -> int:

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
    