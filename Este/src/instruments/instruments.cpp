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
    procCtx->addImage(img);
	LOGGING("Image loaded! %s", i.toStr().c_str());
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
}