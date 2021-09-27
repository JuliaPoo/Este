#pragma once

#include <string>
#include <vector>

namespace Knobs {

	// Parses the config toml file and initialises logging.
	void Init();

	// Get whitelisted binaries list
	std::vector<std::string> getWhitelistBinaries();

	// returns output directory
	std::string getOutputDir();

	// returns output files path prefix: <output-dir>/pid<pid>
	std::string getOutputPrefix();
}