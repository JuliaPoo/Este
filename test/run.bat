:: run <time to pause the tool> <program> <arguments>
:: E.g. run 0 notepad.exe test.txt
:: E.g. run 10 a-32.exe

rm pin.log
rm pintool.log
del Este-out\* /f /q

cp ..\Este\build\Release-x64\Este.dll Este-64.dll
cp ..\Este\build\Release-Win32\Este.dll Este-32.dll

for /f "tokens=1,* delims= " %%a in ("%*") do set RUN_COMMAND=%%b

..\Este\extLibs\pin-3.18-98332-gaebd7b1e6-msvc-windows\pin.exe -follow_execv -pause_tool %1 -t64 Este-64.dll -t Este-32.dll -config-file yoki-config.toml -- %RUN_COMMAND%

type pintool.log
type pin.log