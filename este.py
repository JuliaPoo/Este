import os
import shutil
import glob
import argparse
import subprocess

parser = argparse.ArgumentParser(description='Visualizes the dynamic runtime of a binary')
parser.add_argument("-t", "--pause", type=str, default='0',
                    help="time to pause tool")
parser.add_argument("-p", "--port", type=str, default=7777,
                    help="webserver port")
parser.add_argument('binary', metavar='b', type=str,
                    help='binary to be analyzed')
args = parser.parse_args()

# Create folders
try:
    os.makedirs('gen')
    os.makedirs('web/gen')
except OSError as e:
    pass

# Delete previous runs
toDelete = ['gen\\pin.*', 'gen\\Este-out\\*', 'web\\gen\\*']
for path in toDelete:
	files = glob.glob(path, recursive=True)
	for file in files:
		try:
			os.remove(file)
		except OSError as e:
			print("Error: %s : %s" % (file, e.strerror))

try:
	shutil.copyfile("Este\\build\\Release-x64\\Este-64.dll", 'gen\\Este-64.dll')
	shutil.copyfile('Este\\build\\Release-Win32\\Este-32.dll', 'gen\\Este-32.dll')
except OSError as e:
	print("Este dlls failed to copy")
	print("Error: %s" % (e.strerror))

# Perform analysis on target
try:
	command = [
	".\\Este\\extLibs\\pin-3.18-98332-gaebd7b1e6-msvc-windows\\pin.exe", 
	"-logfile", "gen\\pin.log", 
	"-error_file", "gen\\pin.err", 
	"-follow_execv", 
	"-pause_tool", args.pause, 
	"-t64", "gen\\Este-64.dll", 
	"-t", "gen\\Este-32.dll", 
	"-config-file", "este-config.toml", 
	"--", args.binary
	]
	subprocess.run(command)
except OSError as e:
	print("Este pin failed to run")
	print("Error: %s" % (e.strerror))

# Perform post data processing
from postprocessing.main import postProcessing # import statement changes working directory for some reason
postProcessing()

# Launch visualization
from web.server import server # server.py requires that the pid_tid_map be created before importing
server(args.port)