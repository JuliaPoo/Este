#include "pin.H"

#include <iostream>
#include "Este\knobs.hpp"
#include "Este\instruments.hpp"
#include "Este\serial.hpp"

int Usage() {
    std::cout << "Umm what happened?" << std::endl;
    return 1;
}

int main(int argc, char* argv[])
{
    // Initialise knobs
    Knobs::Init();

    // Initialise serial
    Serial::Init_serializers(
        Knobs::getOutputPrefix() + ".este.json", // generic db
        Knobs::getOutputPrefix() + ".bb.csv", // basic blocks
        Knobs::getOutputPrefix() + ".rtn.csv", // routines
        Knobs::getOutputPrefix() + ".trace.csv" // execution trace
    );

    // Initialise PIN
    if (PIN_Init(argc, argv)) return Usage();

    // Registers callbacks
    Instrument::Init_callbacks();

    // Start!
    PIN_InitSymbols();
    PIN_StartProgram();

    return 0;
}