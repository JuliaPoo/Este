#include "Este\rtn.hpp"
#include "Este\image.hpp"
#include "Este\proc.hpp"

using namespace Ctx;

Rtn::Rtn(const RTN rtn, const Proc* proc)
{
	this->idx = proc->getNumRtn();
	this->addr = RTN_Address(rtn);
	this->rtn_name = RTN_Name(rtn);

	const auto img = proc->getImageExecutable(this->addr);
	if (img) {
		this->image_idx = img->getIdx();
		this->section_idx = img->getExecutableSectionIdx(this->addr);
	}
	else {
		const auto img2 = proc->getImage(this->addr);
		if (img2) {
			this->image_idx = img2->getIdx();
			this->section_idx = -1;
		}
		else {
			this->image_idx = -1;
			this->section_idx = -1;
		}
	}
}

Rtn::Rtn(const ADDRINT addr, const std::string& rtn_name, const bool is_dynamic, const Proc* proc)
{
	// TODO
}

std::ostream& Ctx::operator<<(std::ostream& out, const Rtn& rtn)
{
	out << rtn.idx << ','
		<< rtn.addr << ','
		<< rtn.rtn_name << ','
		<< rtn.image_idx << ','
		<< rtn.section_idx << ','
		<< '\n'; // End entry
	return out;
}

const ADDRINT Rtn::getAddr() const
{
	return this->addr;
}

const std::string& Rtn::getName() const
{
	return this->rtn_name;
}