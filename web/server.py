import os
from http.server import HTTPServer, CGIHTTPRequestHandler
import webbrowser

# Make sure the server is created at current directory
CUR_DIR = os.path.dirname(os.path.realpath(__file__))
os.chdir(CUR_DIR)

# Create server object listening the port 7777
server_object = HTTPServer(
    server_address=('', 7777), 
    RequestHandlerClass = CGIHTTPRequestHandler)
    
def serve():
    '''Starts the webserver'''
    server_object.serve_forever()

def launch():
    '''Launches a browser tab'''
    webbrowser.open('http://localhost:7777') 

launch()
serve()
    