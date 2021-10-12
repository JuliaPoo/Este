#include "pin.H"

#include <iostream>
#include "Este\knobs.hpp"
#include "Este\instruments.hpp"
#include "Este\serial.hpp"
#include "Este\errors.hpp"

#define ESTE_BANNER \
"\n" \
"     ,ggggggg,       ,gg,   ,ggggggggggggggg   ,ggggggg,   " "\n" \
"   ,dP\"\"\"\"\"\"Y8b     i8\"\"8i dP\"\"\"\"\"\"88\"\"\"\"\"\"\" ,dP\"\"\"\"\"\"Y8b  " "\n" \
"   d8'    a  Y8     `8,,8' Yb,_    88        d8'    a  Y8  " "\n" \
"   88     \"Y8P'      `88'   `\"\"    88        88     \"Y8P'  " "\n" \
"   `8baaaa           dP\"8,         88        `8baaaa       " "\n" \
"  ,d8P\"\"\"\"          dP' `8a        88       ,d8P\"\"\"\"       " "\n" \
"  d8\"              dP'   `Yb       88       d8\"            " "\n" \
"  Y8,          _ ,dP'     I8 gg,   88       Y8,            " "\n" \
"  `Yba,,_____, \"888,,____,dP  \"Yb,,8P       `Yba,,_____,   " "\n" \
"    `\"Y8888888 a8P\"Y88888P\"     \"Y8P'         `\"Y8888888   " "\n" \
"\n"

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

    LOGGING(ESTE_BANNER);

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