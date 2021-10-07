#include "Este\instruments.hpp"
#include "Este\errors.hpp"
#include "Este\proc.hpp"
#include "Este\thread.hpp"
#include "Este\image.hpp"
#include "Este\bb.hpp"
#include "Este\rtn.hpp"

#include <pin.H>

using namespace Instrument;

static TLS_KEY tls_key = 0xcccccccc;

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

VOID ThreadStart(THREADID pin_tid, CONTEXT* ctxt, INT32 flags, Ctx::Proc* procCtx)
{
    OS_THREAD_ID os_tid; OS_GetTid(&os_tid);
    LOGGING("Thread started! %d:%d", pin_tid, os_tid);

    auto tctx = new Ctx::Thread();
    tctx->os_tid = os_tid;
    tctx->pin_tid = pin_tid;
    PIN_SetThreadData(tls_key, tctx, pin_tid);
}

VOID ThreadFini(THREADID pin_tid, const CONTEXT* ctxt, INT32 code, Ctx::Proc* procCtx)
{
    OS_THREAD_ID os_tid; OS_GetTid(&os_tid);
    LOGGING("Thread finished! %d:%d", pin_tid, os_tid);
}

VOID ImageLoad(IMG img, Ctx::Proc* procCtx)
{
	Ctx::Image i(img, procCtx);
    procCtx->addImage(i);
	LOGGING("Image loaded! %s", i.toStr().c_str());
}

VOID BblBef(const CONTEXT* pinctx, ADDRINT instptr, THREADID tid, Ctx::Proc* procCtx, uint32_t bbl_size)
{
    int32_t rtn_idx = -1;
    // Log unique bb if not been encountered before
    if (!procCtx->isBbExecuted(instptr)) { // Early terminate
        Ctx::Bb bb(pinctx, procCtx, instptr, bbl_size);
        procCtx->addBb(bb);
    }
}

VOID BblAft(const CONTEXT* pinctx, ADDRINT instptr, ADDRINT bbl_addr, THREADID tid, Ctx::Proc* procCtx)
{
    uint32_t sz;
    auto xeddec = Ctx::Bb::disassemble(instptr, sz);
    ADDRINT target_addr = Ctx::Bb::get_target_addr_from_call_jmp(pinctx, procCtx, instptr, xeddec);

    int32_t rtn_idx = -1;
    if (target_addr) {

        auto img = procCtx->getImageExecutable(target_addr);

        // Log only routine calls outside of whitelist
        if (img != NULL && !img->isWhitelisted()) {

            std::stringstream rtn_name;
            auto rtn = procCtx->getRtn(target_addr);
            if (rtn == NULL) {
                rtn_name << "sub_" << std::hex << target_addr;
                Ctx::Rtn r(target_addr, rtn_name.str(), procCtx);
                procCtx->addRtn(r);
            }

            rtn_idx = procCtx->getRtn(target_addr)->getIdx();
        }
    }

    // Log execution of bb
    NATIVE_TID os_tid; OS_GetTid(&os_tid);
    Ctx::BbExecuted bbe(procCtx->getBbIdx(bbl_addr), os_tid, tid, rtn_idx);
    procCtx->addBbExecuted(bbe);
}

VOID Trace(TRACE trace, Ctx::Proc* procCtx)
{
    auto taddr = TRACE_Address(trace);
    Ctx::Thread* tctx = reinterpret_cast<decltype(tctx)>(
        PIN_GetThreadData(tls_key, PIN_ThreadId()));

    // Check if trace is within whitelisted binaries
    auto img = procCtx->getImageExecutable(taddr);
    if (!(img == NULL || img->isWhitelisted())) {
        if (tctx->was_in_whitelisted_code) {
            tctx->was_in_whitelisted_code = false;
            procCtx->addBbOutOfWhitelisted(tctx->os_tid, tctx->pin_tid);
        }
        return;
    }
    tctx->was_in_whitelisted_code = true;

    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl)) {

        BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)BblBef,
            IARG_CONST_CONTEXT,
            IARG_INST_PTR,
            IARG_THREAD_ID,
            IARG_PTR, procCtx,
            IARG_UINT32, BBL_Size(bbl),
            IARG_END);

        INS_InsertCall(BBL_InsTail(bbl), IPOINT_BEFORE, (AFUNPTR)BblAft,
            IARG_CONST_CONTEXT,
            IARG_INST_PTR,
            IARG_ADDRINT, BBL_Address(bbl),
            IARG_THREAD_ID,
            IARG_PTR, procCtx,
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
    // Initialize TLS key
    tls_key = PIN_CreateThreadDataKey(NULL);

    auto procCtx = new Ctx::Proc();
    LOGGING("Proc created!");

	// Image loading instrumentation
	IMG_AddInstrumentFunction((IMAGECALLBACK)ImageLoad, procCtx);

    // Process finish instrumentation
    PIN_AddPrepareForFiniFunction((PREPARE_FOR_FINI_CALLBACK)PrepareForFiniFunctionFini, procCtx);
    PIN_AddFiniFunction((FINI_CALLBACK)Fini, procCtx);

    // Thread Start and Finish instrumentation
    PIN_AddThreadStartFunction((THREAD_START_CALLBACK)ThreadStart, procCtx);
    PIN_AddThreadFiniFunction((THREAD_FINI_CALLBACK)ThreadFini, procCtx);

    // Basic Blocks
    TRACE_AddInstrumentFunction((TRACE_INSTRUMENT_CALLBACK)Trace, procCtx);

    // Routines
    RTN_AddInstrumentFunction((RTN_INSTRUMENT_CALLBACK)Rtn, procCtx);
}