#include "Este\instruments.hpp"
#include "Este\errors.hpp"
#include "Este\proc.hpp"
#include "Este\image.hpp"
#include "Este\bb.hpp"
#include "Este\rtn.hpp"
#include "Este\mt_map.hpp"

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
	Ctx::Image i(img, procCtx);
    procCtx->addImage(i);
	LOGGING("Image loaded! %s", i.toStr().c_str());
}

VOID BblBef(const CONTEXT* pinctx, ADDRINT instptr, THREADID tid, Ctx::Proc* procCtx, uint32_t bbl_size)
{
    // Log unique bb if not been encountered before
    if (!procCtx->isBbExecuted(instptr)) { // Early terminate
        Ctx::Bb bb(pinctx, procCtx, instptr, bbl_size);
        procCtx->addBb(bb);
    }

    // Log execution of bb
    NATIVE_TID os_tid; OS_GetTid(&os_tid);
    Ctx::BbExecuted bbe(procCtx->getBbIdx(instptr), os_tid, tid);
    procCtx->addBbExecuted(bbe);
}

VOID Trace(TRACE trace, Ctx::Proc* procCtx)
{
    auto taddr = TRACE_Address(trace);

    // Check if trace is within whitelisted binaries
    auto img = procCtx->getImageExecutable(taddr);
    if (!(img == NULL || img->isWhitelisted()))
        return;

    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {

        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)BblBef,
            IARG_CONST_CONTEXT,
            IARG_INST_PTR,
            IARG_THREAD_ID,
            IARG_PTR, procCtx,
            IARG_UINT32, BBL_Size(bbl),
            IARG_END);
    }
}

VOID Rtn(RTN rtn, Ctx::Proc* procCtx)
{
    if (procCtx->isBbExecuted(RTN_Address(rtn)))
        return;
    Ctx::Rtn r(rtn, procCtx);
    procCtx->addRtn(r);
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

    // Routines
    RTN_AddInstrumentFunction((RTN_INSTRUMENT_CALLBACK)Rtn, procCtx);
}