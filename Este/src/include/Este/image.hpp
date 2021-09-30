#pragma once

#include <string>
#include <vector>

#include <pin.H>

namespace Ctx { // Forward declaration
	class Proc;
}

namespace Ctx
{
	// Image object represents a loaded binary
	// in the target process
	class Image {

	public:

		Image() {};

		// Initialise itself from PIN
		Image(const IMG img, const Proc* proc);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const Image& i);

		// To string method for logging
		std::string toStr();

		const int32_t getIdx() const;

		const std::string& getPath() const;

		const std::pair<ADDRINT, ADDRINT>& getAddrRange() const;

		const std::vector< std::pair<ADDRINT, ADDRINT>>& getAddrRangeExecutable() const;

		bool isMain() const;

		bool isWhitelisted() const;

		bool isWithinBinary(ADDRINT addr) const;

		bool isWithinExecutableRange(ADDRINT addr) const;

		int32_t getExecutableSectionIdx(ADDRINT addr) const;

	private:

		/* To be serialized */

		// Image idx
		int32_t idx = -1;

		// File path of binary
		std::string path;

		// Address range of binary
		std::pair<ADDRINT, ADDRINT> addr_range;

		// Address range of executable sections
		std::vector< std::pair<ADDRINT, ADDRINT>> addr_range_executable;

		// Is main executable
		bool is_main = false;

		// Is whitelisted
		bool is_whitelisted = false;

	};

	// Image serializer declaration
	std::ostream& operator<<(std::ostream& out, const Image& i);
}