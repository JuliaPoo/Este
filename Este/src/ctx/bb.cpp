#include "Este\bb.hpp"
#include "Este\proc.hpp"
#include "Este\utils.hpp"
#include "Este\image.hpp"

using namespace Ctx;

Bb::Bb(const ADDRINT low_addr, const uint32_t size, const Proc* proc)
{
	this->idx = proc->getNumBb();
	auto baddr = low_addr;
	auto bsz = size;
	this->addr_range = std::make_pair(baddr, baddr + bsz);
	this->bytes.assign(reinterpret_cast<char*>(baddr), reinterpret_cast<char*>(baddr + bsz));

	const auto img = proc->getImageExecutable(baddr);
	if (img) {
		this->image_idx = img->getIdx();
		this->section_idx = img->getExecutableSectionIdx(baddr);
	}
	else {
		const auto img2 = proc->getImage(baddr);
		if (img2) {
			this->image_idx = img2->getIdx();
			this->section_idx = -1;
		}
		else {
			this->image_idx = -1;
			this->section_idx = -1;
		}
	}

	// TODO: Add routines
}

std::ostream& Ctx::operator<<(std::ostream& out, const Bb& bb)
{
	out << bb.idx << "," // idx
		<< bb.addr_range.first << "," // addr_low
		<< bb.addr_range.second << "," // addr_high
		<< EsteUtils::toHex(bb.bytes) << "," // bytes
		<< bb.image_idx << "," // image_idx
		<< bb.section_idx // section_idx
		<< "\n"; // End entry.
	// TODO: Add routines
	return out;
}


const int32_t Bb::getIdx() const
{
	return this->idx;
}

// Get address range
const std::pair<ADDRINT, ADDRINT>& Bb::getAddrRange() const
{
	return this->addr_range;
}

BbExecuted::BbExecuted(uint32_t bb_idx, OS_THREAD_ID os_tid, THREADID pin_tid)
{
	this->bb_idx = bb_idx;
	this->os_tid = os_tid;
	this->pin_tid = pin_tid;
}

std::ostream& Ctx::operator<<(std::ostream& out, const BbExecuted& bbe)
{
	out << bbe.bb_idx << "," // bb idx
		<< bbe.os_tid << "," // tid
		<< bbe.pin_tid // pin tid
		<< "\n"; // End entry

	return out;
}