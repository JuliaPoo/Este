# Este

Este is an experimental tool in very early development that visualizes the dynamic runtime of a binary via instrumentation. Currently supports X86, X86_64 on Windows.

## Build Instructions

```
git clone --recursive https://github.com/JuliaPoo/Este
cd Este\Este\extLibs
7z x pin-3.18-98332-gaebd7b1e6-msvc-windows.zip
```

Open `Este\Este.sln` in VS 2019, select build configuration to `Release` and build (in x64 or Win32).

## Usage

[TODO]
For now just go into the `.\test` folder and run `run.bat 0 a-32.exe` or smth. The output is in `Este-out`. Read `run.bat` and `este-config.toml` for more information.