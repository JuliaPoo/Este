#include "pin.H"

#include <iostream>
#include "Este\knobs.hpp"
#include "Este\instruments.hpp"
#include "Este\serial.hpp"

int Usage(char* argv[]) {
    std::cout << 
        "\nUsage:\n" << 
        "    pin.exe <pin knobs> -config-file <config filepath> -- <target program and arguments>\n"
        "        @param config-file: Este config file. See `este-config.toml` for more details\n"
        << std::endl;
    return 1;
}

int main(int argc, char* argv[])
{
    // Initialise PIN
    if (PIN_Init(argc, argv)) return Usage(argv);

    // Initialise knobs
    Knobs::Init();

    // Initialise serial
    Serial::Init_serializers(
        Knobs::getOutputPrefix() + ".este.json", // generic db
        Knobs::getOutputPrefix() + ".bb.csv", // basic blocks
        Knobs::getOutputPrefix() + ".rtn.csv", // routines
        Knobs::getOutputPrefix() + ".trace.csv" // execution trace
    );

    // Registers callbacks
    Instrument::Init_callbacks();

    // Start!
    PIN_InitSymbols();
    PIN_StartProgram();

    return 0;
}