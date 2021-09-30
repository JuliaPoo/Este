#include "Este\proc.hpp"
#include "Este\serial.hpp"
#include "Este\image.hpp"
#include "Este\bb.hpp"
#include "Este\errors.hpp"

using namespace Ctx;

Proc::Proc()
{
	NATIVE_PID pid; OS_GetPid(&pid);

	// Write headers for Db
	Serial::getDb()
		<< "{" // Open
		<< "\"pid\":" << pid << ","
		<< "\"binaries_loaded\":" << "["; // Open list for binaries

	// Write headers for Bb
	auto& bb_serial = Serial::getBb();
	bb_serial << "idx,addr_low,addr_high,bytes,image_idx,section_idx\n";
}

Proc::~Proc()
{
	// Make sure no one is holding any locks before destruction
	this->_images_lock.writer_aquire();
	this->_serial_db_lock.writer_aquire();
	this->_bbs_lock.writer_aquire();
	this->_serial_bb_lock.writer_aquire();

	Serial::getDb()
		<< "]"  // Close list for binaries
		<< "}"; // Close
	Serial::getDb().flush();
	Serial::getBb().flush();
}

void Proc::addImage(Ctx::Image& img)
{
	// Add image
	auto w = this->_images_lock.writer_aquire();
	this->images.push_back(img);
	this->_images_lock.writer_release(w);

	// Serialize img
	w = this->_serial_db_lock.writer_aquire();
	Serial::getDb() << (this->images.size() == 1 ? "" : ",") << img;
	this->_serial_db_lock.writer_release(w);
}

void Proc::addBb(Ctx::Bb& bb)
{
	auto w = this->_bbs_lock.writer_aquire();
	// Check if bb exists already
	if (this->bbs.find(bb.getAddrRange().first) != this->bbs.end()) {
		this->_bbs_lock.writer_release(w);
		return;
	}
	// Add bb
	this->bbs.insert(std::make_pair(bb.getAddrRange().first, bb.getIdx()));
	this->_bbs_lock.writer_release(w);

	// Serialize bb
	w = this->_serial_bb_lock.writer_aquire();
	Serial::getBb() << bb;
	this->_serial_bb_lock.writer_release(w);
}

const Image* Proc::getImage(ADDRINT addr) const
{
	const Image* ret = NULL;
	auto r = const_cast<Proc*>(this)->_images_lock.reader_aquire();
	for (auto& i : this->images)
		if (i.isWithinBinary(addr)) {
			ret = &i;
			break;
		}
	const_cast<Proc*>(this)->_images_lock.reader_release(r);
	return ret;
}

const Image* Proc::getImageExecutable(ADDRINT addr) const
{
	const Image* ret = NULL;
	auto r = const_cast<Proc*>(this)->_images_lock.reader_aquire();
	for (auto& i : this->images)
		if (i.isWithinExecutableRange(addr)) {
			ret = &i;
			break;
		}
	const_cast<Proc*>(this)->_images_lock.reader_release(r);
	return ret;
}

const int32_t Proc::getNumImages() const
{
	auto r = const_cast<Proc*>(this)->_images_lock.reader_aquire();
	int32_t ret = this->images.size();
	const_cast<Proc*>(this)->_images_lock.reader_release(r);
	return ret;
}

const int32_t Proc::getNumBb() const
{
	auto r = const_cast<Proc*>(this)->_bbs_lock.reader_aquire();
	int32_t ret = this->bbs.size();
	const_cast<Proc*>(this)->_bbs_lock.reader_release(r);
	return ret;
}

bool Proc::isBbExecuted(ADDRINT addr_low) const
{
	auto r = const_cast<Proc*>(this)->_bbs_lock.reader_aquire();
	bool ret = this->bbs.find(addr_low) != this->bbs.end();
	const_cast<Proc*>(this)->_bbs_lock.reader_release(r);
	return ret;
}