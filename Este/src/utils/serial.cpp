#include "Este\serial.hpp"

std::ofstream db;

void Serial::Init_serializers(
	std::string db_filepath
)
{
	new (&db) decltype(db)(
		db_filepath.c_str(),
		std::ios::out | std::ios::binary);
}

std::ofstream& Serial::getDb()
{
	return db;
}