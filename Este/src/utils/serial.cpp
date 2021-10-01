#include "Este\serial.hpp"

std::ofstream db;
std::ofstream bb;
std::ofstream trace;

void Serial::Init_serializers(
	std::string db_filepath,
    std::string bb_filepath,
	std::string trace_filepath
)
{
	new (&db) decltype(db)(
		db_filepath.c_str(),
		std::ios::out | std::ios::binary);

    new (&bb) decltype(bb)(
        bb_filepath.c_str(),
        std::ios::out | std::ios::binary);

	new (&trace) decltype(trace)(
		trace_filepath.c_str(),
		std::ios::out | std::ios::binary);
}

std::ofstream& Serial::getDbSerial()
{
	return db;
}

std::ofstream& Serial::getBbSerial()
{
    return bb;
}

std::ofstream& Serial::getTraceSerial()
{
	return trace;
}