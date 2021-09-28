#pragma once

#include <string>
#include <vector>

#include <pin.H>

namespace Ctx
{
	// Image object represents a loaded binary
	// in the target process
	class Image {

	public:

		Image() {};

		// Initialise itself from PIN
		Image(IMG img);

		// Serializer friend declaration
		friend std::ostream& operator<<(std::ostream& out, const Image& i);

		// To string method for logging
		std::string toStr();

		const auto& getPath() const { return this->path; }

		const auto& getAddrRange() const { return this->addr_range; }

		const auto& getAddrRangeExecutable() const { return this->addr_range_executable; }

	private:

		// File path of binary
		std::string path;

		// Address range of binary
		std::pair<ADDRINT, ADDRINT> addr_range;

		// Address range of executable sections
		std::vector< std::pair<ADDRINT, ADDRINT>> addr_range_executable;

		// Is main executable
		bool is_main = false;

	};

	// Image serializer declaration
	std::ostream& operator<<(std::ostream& out, const Image& i);
}