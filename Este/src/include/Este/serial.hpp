#pragma once

#include <string>
#include <fstream>

namespace Serial {

	// Initializes serializers
	void Init_serializers(
		std::string db_filepath
	);


	// Returns serializer to .este.json file
	// Contains generic information about process
	std::ofstream& getDb();

}