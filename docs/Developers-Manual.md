# Developer's Manual

## Key Components

Este has 3 main components:

1. Este Pintool
    - Analyses the target binary and outputs log files into directory specified by [este-config.toml](../este-config.toml)
    - Source files for this component is located in directory [./Este](../Este).
2. Post Processing
    - Performs processing of log files and outputs into [./web/gen](../web/gen).
    - Source files for this component is located in directory [./postprocessing](../postprocessing)
3. Visualization
    - Starts a python server and opens a browser tab to display the visualization.
    - Source files for this component is located in directory [./web](../web)

## Component 1: Este Pintool

### Commandline Usage

Este Pintool is of the form of two binaries `Este-32.dll` and `Este-64.dll` (32 bit and 64 bit), located in `.\Este\build` folders. Both binaries are required for Este to analyse 32 bit and 64 bit binaries.

Este Pintool requires PINTOOL's launcher binary `pin.exe`. The command to launch Este Pintool takes the following semantics:

```
pin.exe <PIN commandline switches> <este commandline switches> -- <target command>
```

PIN's commandline switches are documented [here](https://software.intel.com/sites/landingpage/pintool/docs/71313/Pin/html/group__KNOBS.html).

Este commandline switches are the following:

```
-config-file <config file name> 
    Config file for Este. Refer to este-config.toml for more details.
```

Example Este Pintool usage:

```
pin.exe -follow_execv -config-file "este-config.toml" -- notepad.exe text.txt
```

### Output Format

Este Pintool outputs into the directory specified within the config file specified by the `-config-file` commandline switch.

Este Pintool outputs 5 files.

1. `pid<pid>.log`
2. `pid<pid>.este.json`
3. `pid<pid>.rtn.csv`
4. `pid<pid>.bb.csv`
5. `pid<pid>.trace.csv`

`pid<pid>.log` contains generic information about Este Pintool's and the target process's runtime, including any Este warnings or errors. In the event of a crash, if the exception was handled by Este Pintool, the error message will be present here.

`pid<pid>.este.json` contains generic information about the target process:
1. PID
2. Architecture
3. Images loaded

`pid<pid>.rtn.csv` contains all routines encountered (including those not executed) that are outside of the whitelisted binaries specified in `-config-file`. The fields are:

| Name | Description |
| ---- | ----------- |
| `idx` | - Index of routine |
| `addr` | - Address that marks the start of the routine |
| `rnt_name` | - Name of routine <br> - Name of routine relies on symbols available. Otherwise it will be named `sub_<addr>`|
| `image_idx` | - Index of image (as specified in `pid<pid>.este.json`) at which the routine belongs in. <br> - `image_idx = -1` if it doesn't belong to an image.|
|`section_idx`| - Index if executable section within image (as specified in `pid<pid>.este.json`) at which the routine belongs in <br> - `section_idx = -1` if it doesn't belong to a section

`pid<pid>.bb.csv` contains all Basic Blocks encountered within the whitelisted binaries specified in `-config-file`. Basic blocks are defined as a number of contiguous instructions that drop through. Jumps, conditional jumps, calls, returns, etc are at the end of every basic block. The fields are:

| Name | Description |
| ---- | ----------- |
| `idx` | - Index of basic block (in order of first execution) |
| `addr` | - Address of start of basic block |
| `size` | - Size in bytes of basic block |
| `bytes` | - Hex encoded bytes of basic block |
| `image_idx` | - Index of image (as specified in `pid<pid>.este.json`) at which the block is in <br> - `image_idx = -1` if it doesn't belong to an image
| `section_idx` | - Index of executable section within image (as specified in `pid<pid>.este.json`) at which the block belongs in <br> - `section_idx = -1` if it doesn't belong to a section |

`pid<pid>.trace.csv` contains all basic blocks executed that are within the whitelisted binaries in `-config-file`. E.g. in a loop, the blocks within the loop are logged multiple times within the trace file. The fields are:

| Name | Description |
| ---- | ----------- |
| `bb_idx` | - Index of basic block executed (as specified in `pid<pid>.bb.csv`) <br> - `bb_idx = -1` if the basic block is outside of whitelisted binaries. This entry indicates a jump outside of whitelisted binaries. |
| `os_tid` | - OS Thread ID that executed said block |
| `pin_tid` | - PIN Thread ID that executed said block (order of creation) |
| `rtn_called_idx` | - Index of routine called (as specified in `pid<pid>.rtn.csv`) <br> - `rtn_called_idx = -1` if no routine was called in basic block. |

### Debugging Este Pintool

Add the PIN commandline switch `-pause_tool <seconds>` to pause the tool upon PIN initialization, and attach a debugger (e.g. WinDbg), and resume execution.
