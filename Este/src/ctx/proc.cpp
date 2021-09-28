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

void Proc::addImage(Ctx::Image img)
{
	// Add image
	auto w = this->_images_lock.writer_aquire();
	this->images.push_back(img);
	this->_images_lock.writer_release(w);

	w = this->_serial_db_lock.writer_aquire();
	Serial::getDb() << img << ",";
	this->_serial_db_lock.writer_release(w);
}

