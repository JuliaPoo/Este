#include "Este\proc.hpp"
#include "Este\serial.hpp"

using namespace Ctx;

Proc::Proc()
{
	NATIVE_PID pid; OS_GetPid(&pid);
	Serial::getDb()
		<< "{" // Open
		<< "\"pid\":" << pid << ","
		<< "\"binaries_loaded\":" << "["; // Open list for binaries
}

Proc::~Proc()
{
	Serial::getDb()
		<< "]"  // Close list for binaries
		<< "}"; // Close
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
