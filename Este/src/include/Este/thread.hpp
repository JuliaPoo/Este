#pragma once

#include <pin.H>

namespace Ctx
{
	// Object that stores thread specific information
	// of the target process
	struct Thread {

		OS_THREAD_ID os_tid = 0xffffffff;
		THREADID pin_tid = 0xffffffff;
		bool was_in_whitelisted_code = false;

	};
}