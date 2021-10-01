#include "Este\image.hpp"
#include "Este\errors.hpp"
#include "Este\serial.hpp"
#include "Este\knobs.hpp"
#include "Este\utils.hpp"
#include "Este\proc.hpp"

#include <sstream>

using namespace Ctx;

#define STREAM_POINTER_FORMAT std::hex << std::setw(2*sizeof(void*)) << std::setfill('0')

Image::Image(const IMG img, const Proc* proc)
{
	this->idx = proc->getNumImages();
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
	this->is_whitelisted = Knobs::isBinaryWhitelisted(this->path);
}

std::string Image::toStr()
{
	std::stringstream ret;
	ret << "[" << this->idx << "] " // idx
		<< __get_filename_from_fullpath(this->path.c_str()) // Filename
		<< (this->is_main ? " [MAIN]" : "") // is_main
		<< (this->is_whitelisted ? " [WHITELISTED]" : "") // is_main
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
		<< "\"idx\":" << i.idx << ","
		<< "\"path\":" << EsteUtils::json_escape(i.path) << ","
		<< "\"addr_range\":[" << i.addr_range.first << "," << i.addr_range.second << "],"
		<< "\"addr_range_executable\":[";

	uint8_t is_first = 0;
	for (auto r : i.addr_range_executable) {
		out << (is_first++==0 ? "[" : ",[");
		out << r.first << "," << r.second << "]";
	}

	out << "],"
		<< "\"is_main\":" << (i.is_main ? "true" : "false") << ","
		<< "\"is_whitelisted\":" << (i.is_whitelisted ? "true" : "false")
		<< "}"; // Closing

	return out;
}

const int32_t Image::getIdx() const
{
	return this->idx;
}

const std::string& Image::getPath() const 
{ 
	return this->path; 
}

const std::pair<ADDRINT, ADDRINT>& Image::getAddrRange() const 
{ 
	return this->addr_range; 
}

const std::vector< std::pair<ADDRINT, ADDRINT>>& Image::getAddrRangeExecutable() const 
{ 
	return this->addr_range_executable; 
}

bool Image::isMain() const 
{ 
	return this->is_main; 
}

bool Image::isWhitelisted() const 
{
	return this->is_whitelisted; 
}

bool Image::isWithinBinary(ADDRINT addr) const
{
	auto& r = this->addr_range;
	return addr < r.second&& addr > r.first;
}

bool Image::isWithinExecutableRange(ADDRINT addr) const 
{
	for (auto& r : this->addr_range_executable)
		if (addr < r.second && addr > r.first)
			return true;
	return false;
}

int32_t Image::getExecutableSectionIdx(ADDRINT addr) const
{
	int32_t idx = 0;
	for (auto& r : this->addr_range_executable) {
		if (addr < r.second && addr > r.first)
			return idx;
		idx++;
	}
	return -1;
}