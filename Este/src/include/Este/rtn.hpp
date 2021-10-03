#pragma once

#include <pin.H>

namespace Ctx { // Forward declaration
	class Proc;
}

namespace Ctx
{
	// Routine object represents a routine
	// loaded in memory.
	class Rtn {

	public:

		// Constructor
		Rtn(const RTN rtn, const Proc* proc);

		// Constructor
		Rtn(const ADDRINT addr, const std::string& rtn_name, const Proc* proc);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const Rtn& rtn);

		// Get index
		const int32_t getIdx() const;

		// Get address
		const ADDRINT getAddr() const;

		// Get name of routine
		const std::string& getName() const;

	private:
		
		// Index of routine. -1 if invalid
		int32_t idx = -1;

		// Address of start of rtn
		ADDRINT addr = 0;

		// Routine name
		std::string rtn_name;

		// Index of image rtn is in. -1 if invalid
		int32_t image_idx = -1;

		// Index of executable section in image rtn is in. -1 if invalid
		int32_t section_idx = -1;

	};

	std::ostream& operator<<(std::ostream& out, const Rtn& rtn);
}