#pragma once

#include <string>
#include <vector>

namespace EsteUtils {

	// Checks if string contains suffix
	bool hasSuffix(std::string const& str, std::string const& suffix);

	// Converts string to lowercase
	std::string& toLower(std::string& str);

	// Escape string for json. Assumes `s` contains ASCII only.
	std::string json_escape(std::string const s);

	// Escape string for csv. Assumes `s` contains ASCII only.
	std::string csv_escape(std::string const s);

	// Hex encode string
	std::string toHex(const std::vector<uint8_t> const s);

}