#pragma once

#include <string>
#include <fstream>

namespace Serial {

	// Initializes serializers
	void Init_serializers(
		std::string db_filepath,
		std::string bb_filepath
	);

	// Returns serializer to .este.json file
	// Contains generic information about process
	std::ofstream& getDb();

	// Returns serializer to .bb.csv file
	// Contains information of all basic blocks executed (within whitelisted binaries)
	std::ofstream& getBb();
}