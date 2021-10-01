#pragma once

#include <pin.H>

namespace Ctx { // Forward declaration
	class Proc;
}

// TODO: Deal with routine calls
namespace Ctx {

	// Bb object represents a basic block, 
	// contains information relating to said basic block
	// Object is never stored in memory, 
	// but instead is directly serialized into .bb file
	class Bb {

	public:

		// Constructor
		Bb(const ADDRINT low_addr, const uint32_t size, const Proc* proc);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const Bb& bb);

		// Add routine called
		// void addRoutineCalled(const Routine& rtn);

		// Get idx of `this`
		const int32_t getIdx() const;

		// Get address range
		const std::pair<ADDRINT, ADDRINT>& getAddrRange() const;

	private:

		/* To be serialized */

		// Index of bb. -1 if invalid.
		int32_t idx = -1;

		// Address range occupied by basic block
		std::pair<ADDRINT, ADDRINT> addr_range;

		// Bytes of the basic block
		std::vector<uint8_t> bytes;

		// Index of binary Bb is located in. Is -1 if not in binary (e.g. heap or stakc)
		int32_t image_idx = -1;

		// Index of executable section of the binary Bb is located in (-1 if image_idx=-1)
		int32_t section_idx = -1;

		// Indices of routines that's directly called in Bb
		// std::vector<int32_t> routines_called;
	};

	// BbExecuted is a class that represents the execution of a basic block
	// While Bb stores info about the basic block, BbExecuted stores info
	// about how the block was executed.
	class BbExecuted
	{
	public:

		// Constructor
		BbExecuted(uint32_t bb_idx, OS_THREAD_ID os_tid, THREADID pin_tid);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const BbExecuted& bbe);

	private:

		// Index of basic block executed
		uint32_t bb_idx = 0xffffffff;

		// OS's Thread id that executed block
		OS_THREAD_ID os_tid = 0xffffffff;

		// Pin's Thread id that executed block
		THREADID pin_tid = 0xffffffff;
	};

	std::ostream& operator<<(std::ostream& out, const Bb& bb);
	std::ostream& operator<<(std::ostream& out, const BbExecuted& bbe);
}