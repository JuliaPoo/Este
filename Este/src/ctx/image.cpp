#include "Este\image.hpp"
#include "Este\errors.hpp"

#include <sstream>

using namespace Ctx;

#define STREAM_POINTER_FORMAT std::hex << std::setw(2*sizeof(void*)) << std::setfill('0')

Image::Image(IMG img)
{
	this->path = IMG_Name(img);
	this->addr_range = std::make_pair(IMG_LowAddress(img), IMG_HighAddress(img));

	for (SEC sec = IMG_SecHead(img); SEC_Valid(sec); sec = SEC_Next(sec))
	{
		if (!SEC_IsExecutable(sec))
			continue;

		auto sz = SEC_Size(sec);
		auto addr = SEC_Address(sec);
		this->addr_range_executable.push_back(std::make_pair(addr, addr + sz));
	}

	this->is_main = IMG_IsMainExecutable(img);
}

std::string Image::toStr()
{
	std::stringstream ret;
	ret << __get_filename_from_fullpath(this->path.c_str()) // Filename
		<< (this->is_main ? " [MAIN]" : "") // is_main
		<< " [" << STREAM_POINTER_FORMAT << this->addr_range.first << "-" 
		<< STREAM_POINTER_FORMAT << this->addr_range.second << "]" // addr range
		<< " [ Executable: ";
	for (auto r : this->addr_range_executable) // executable addr ranges
		ret << STREAM_POINTER_FORMAT << r.first << "-" 
			<< STREAM_POINTER_FORMAT << r.second << ",";
	ret << "]";
	return ret.str();
}

std::ostream& Ctx::operator<<(std::ostream& out, const Image& i)
{
	out << "{" // Opening
		<< "\"path\":" << "\"" << i.path << "\","
		<< "\"addr_range\":[" << i.addr_range.first << "," << i.addr_range.second << "],"
		<< "\"addr_range_executable\":[";
	for (auto r : i.addr_range_executable)
		out << "[" << r.first << "," << r.second << "],";
	out << "],"
		<< "\"is_main\":" << (i.is_main ? "true" : "false") << ","
		<< "}"; // Closing

	return out;
}