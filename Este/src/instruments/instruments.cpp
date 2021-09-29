#include "Este\instruments.hpp"
#include "Este\errors.hpp"

#include "Este\proc.hpp"

#include <pin.H>

using namespace Instrument;

VOID PrepareForFiniFunctionFini(Ctx::Proc* procCtx)
{
    // Destroy proc object
    LOGGING("Proc deleted!");
    delete procCtx;
}

VOID Fini(INT32 code, Ctx::Proc* procCtx)
{
    LOGGING("Process terminated with code %d", code);
}


VOID ImageLoad(IMG img, Ctx::Proc* procCtx)
{
	Ctx::Image i(img);
    procCtx->addImage(i);
	LOGGING("Image loaded! %s", i.toStr().c_str());
}

VOID Trace(TRACE trace, Ctx::Proc* procCtx)
{
    auto taddr = TRACE_Address(trace);

    // Check if trace is within whitelisted binaries
    auto img = procCtx->getImageExecutable(taddr);
    if (img == NULL || img->isWhitelisted())
        return;

    // TODO: Log the bb
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {
        // BBL_InsertCall();
    }
}

void Instrument::Init_callbacks()
{
    auto procCtx = new Ctx::Proc();
    LOGGING("Proc created!");

	// Image loading instrumentation
	IMG_AddInstrumentFunction((IMAGECALLBACK)ImageLoad, procCtx);

    // Process finish instrumentation
    PIN_AddPrepareForFiniFunction((PREPARE_FOR_FINI_CALLBACK)PrepareForFiniFunctionFini, procCtx);
    PIN_AddFiniFunction((FINI_CALLBACK)Fini, procCtx);

    // Basic Blocks
    TRACE_AddInstrumentFunction((TRACE_INSTRUMENT_CALLBACK)Trace, procCtx);
}