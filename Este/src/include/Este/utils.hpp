#pragma once

#include <string>
#include <vector>

namespace EsteUtils {

	// Allocate memory of size `required_bytes` with alignment `alignment`
	void* aligned_malloc(size_t required_bytes, size_t alignment);

	// Free memory allocated via aligned_malloc
	void aligned_free(void* p);

	// Checks if string contains suffix
	bool hasSuffix(const std::string& str, const std::string& suffix);

	// Converts string to lowercase
	std::string& toLower(std::string& str);

	// Escape string for json. Assumes `s` contains ASCII only.
	std::string json_escape(const std::string s);

	// Escape string for csv. Assumes `s` contains ASCII only.
	std::string csv_escape(const std::string s);

	// Hex encode string
	std::string toHex(const std::vector<uint8_t> s);

}