#pragma once

#include <pin.H>
#include <unordered_map>

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

		// Default constructor
		Bb();

		// Constructor
		Bb(const CONTEXT* pinctx, const Proc* proc, const ADDRINT low_addr, const uint32_t size);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const Bb& bb);

		// Get idx of `this`
		const int32_t getIdx() const;

		// Get address range
		const ADDRINT getAddr() const;

		// Get size of bb
		const uint32_t getSize() const;

		// Returns xed decoded structure given address.
		// Fills out `out_size` with the size of instruction in bytes.
		static const xed_decoded_inst_t disassemble(const ADDRINT addr, uint32_t& out_size);

		// Disassembles instruction as string
		static const std::string inst_to_str(const ADDRINT addr, const xed_decoded_inst_t& inst);

		// Returns address of target from a call or jmp instruction.
		// Returns NULL if inst isn't a call or jmp.
		static ADDRINT get_target_addr_from_call_jmp(
			const CONTEXT * pinctx, const Proc * proc,
			const ADDRINT inst_addr, const xed_decoded_inst_t & inst);

	private:

		// Intepretes target address from operand of call/jmp
		static ADDRINT get_rtn_addr_call_jmp_from_operand(
			const CONTEXT* pinctx, const xed_decoded_inst_t& inst, 
			const ADDRINT inst_addr, const uint32_t op_idx, const uint32_t memop_idx);

		// Properly sets this->size and this->bytes
		int32_t build();

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

		// If basic block has a routine call out of whitelisted
		bool has_rtn_call_out_of_whitelisted = false;
	};

	// BbExecuted is a class that represents the execution of a basic block
	// While Bb stores info about the basic block, BbExecuted stores info
	// about how the block was executed.
	class BbExecuted
	{
	public:

		// Constructor
		BbExecuted(uint32_t bb_idx, OS_THREAD_ID os_tid, THREADID pin_tid, int32_t rtn_called_idx);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const BbExecuted& bbe);

	private:

		// Index of basic block executed
		uint32_t bb_idx = 0xffffffff;

		// OS's Thread id that executed block
		OS_THREAD_ID os_tid = 0xffffffff;

		// Pin's Thread id that executed block
		THREADID pin_tid = 0xffffffff;

		// Index of routine called.
		int32_t rtn_called_idx = -1;
	};

	std::ostream& operator<<(std::ostream& out, const Bb& bb);
	std::ostream& operator<<(std::ostream& out, const BbExecuted& bbe);
}