:: run <time to pause the tool> <program> <arguments>
:: E.g. run 0 notepad.exe test.txt
:: E.g. run 10 a-32.exe

:: Create folders
if not exist gen mkdir gen
if not exist web\gen mkdir web\gen

:: Delete previous runs
del gen\pin.* /f /q
del gen\Este-out\* /f /q
del web\gen\* /f /q

:: Copy Este dlls into gen folder
copy Este\build\Release-x64\Este-64.dll gen\Este-64.dll
copy Este\build\Release-Win32\Este-32.dll gen\Este-32.dll

:: Perform analysis on target
for /f "tokens=1,* delims= " %%a in ("%*") do set RUN_COMMAND=%%b
".\Este\extLibs\pin-3.18-98332-gaebd7b1e6-msvc-windows\pin.exe" -logfile "gen\pin.log" -error_file "gen\pin.err" -follow_execv -pause_tool %1 -t64 "gen\Este-64.dll" -t "gen\Este-32.dll" -config-file "este-config.toml" -- %RUN_COMMAND%

:: Perform post analysis
python postprocessing/main.py

:: Launch visualization
python web/server.py