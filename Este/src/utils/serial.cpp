#include "Este\serial.hpp"

std::ofstream db;
std::ofstream bb;

void Serial::Init_serializers(
	std::string db_filepath,
    std::string bb_filepath
)
{
	new (&db) decltype(db)(
		db_filepath.c_str(),
		std::ios::out | std::ios::binary);

    new (&bb) decltype(bb)(
        bb_filepath.c_str(),
        std::ios::out | std::ios::binary);
}

std::ofstream& Serial::getDb()
{
	return db;
}

std::ofstream& Serial::getBb()
{
    return bb;
}