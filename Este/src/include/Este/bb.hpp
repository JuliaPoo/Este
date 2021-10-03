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

		// Represents an instruction that called a routine
		typedef struct {
			std::vector<int32_t> rtn_idx;
			ADDRINT inst_addr;
			bool is_dynamic;
		} RtnCall;

		// Default constructor
		Bb();

		// Constructor
		Bb(const CONTEXT* pinctx, const Proc* proc, const ADDRINT low_addr, const uint32_t size);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const Bb& bb);

		// Add routine called
		// void addRoutineCalled(const Routine& rtn);

		// Get idx of `this`
		const int32_t getIdx() const;

		// Get address range
		const ADDRINT getAddr() const;

		// Get size of bb
		const uint32_t getSize() const;

	private:

		static const xed_decoded_inst_t _disassemble(const ADDRINT addr, uint32_t& out_size);

		static const std::string _inst_to_str(const ADDRINT addr, const xed_decoded_inst_t& inst);

		static ADDRINT get_rtn_addr_call_jmp(
			const CONTEXT* pinctx, const xed_decoded_inst_t& inst, 
			const ADDRINT inst_addr, const uint32_t op_idx, const uint32_t memop_idx, 
			bool& addr_is_dynamic);

		// Returns true if jump address is dynamic
		bool process_inst(const CONTEXT* pinctx, const Proc* proc, const ADDRINT inst_addr, const xed_decoded_inst_t& inst);

		void build(const CONTEXT* pinctx, const Proc* proc);

		/* To be serialized */

		// Index of bb. -1 if invalid.
		int32_t idx = -1;

		// lower address occupied by basic block
		ADDRINT addr = 0;

		// size in bytes of bb
		uint32_t size = 0;

		// Bytes of the basic block
		std::vector<uint8_t> bytes;

		// Index of binary Bb is located in. Is -1 if not in binary (e.g. heap or stakc)
		int32_t image_idx = -1;

		// Index of executable section of the binary Bb is located in (-1 if image_idx=-1)
		int32_t section_idx = -1;

		// Indices of routines that's directly called in Bb
		std::vector<int32_t> routines_called;
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