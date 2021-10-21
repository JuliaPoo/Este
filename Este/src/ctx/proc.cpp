#include "Este\proc.hpp"
#include "Este\serial.hpp"
#include "Este\image.hpp"
#include "Este\rtn.hpp"
#include "Este\bb.hpp"
#include "Este\errors.hpp"
#include "Este\knobs.hpp"
#include "Este\utils.hpp"

using namespace Ctx;

Proc::Proc()
{
	NATIVE_PID pid; OS_GetPid(&pid);

	// Write headers for Db
	Serial::getDbSerial()
		<< "{" // Open
		<< "\"pid\":" << pid << ","
		<< "\"architecture\":"
#if (defined TARGET_IA32E)
		<< "\"ARCH_X8664\""
#elif (defined TARGET_IA32)
		<< "\"ARCH_X86\""
#else
#error "Architecture not defined!"
#endif
		<< ","
		<< "\"binaries_loaded\":" << "["; // Open list for binaries

	// Write headers for bb
	auto& bb_serial = Serial::getBbSerial();
	bb_serial << "idx,addr,size,bytes,image_idx,section_idx\n";

	// Write headers for trace
	auto& trace_serial = Serial::getTraceSerial();
	trace_serial << "bb_idx,os_tid,pin_tid,rtn_called_idx\n";

	// Write headers for rtn
	auto& rtn_serial = Serial::getRtnSerial();
	rtn_serial << "idx,addr,rtn_name,image_idx,section_idx\n";
}

Proc::~Proc()
{
	// Make sure no one is holding any locks before destruction
	this->_images_lock.writer_aquire();
	this->_serial_db_lock.writer_aquire();
	this->_bbs_lock.writer_aquire();
	this->_serial_bb_lock.writer_aquire();
	this->_rtns_lock.writer_aquire();
	this->_serial_rtn_lock.writer_aquire();
	this->_serial_trace_lock.writer_aquire();

	Serial::getDbSerial()
		<< "]"  // Close list for binaries
		<< "}"; // Close
	Serial::getDbSerial().flush();
	Serial::getBbSerial().flush();
	Serial::getRtnSerial().flush();
	Serial::getTraceSerial().flush();
}

void* Proc::operator new(size_t i)
{
	return EsteUtils::aligned_malloc(i, 64);
}

void Proc::operator delete(void* p)
{
	EsteUtils::aligned_free(p);
}

void Proc::addImage(const Ctx::Image& img)
{
	// Add image
	auto w = this->_images_lock.writer_aquire();
	this->images.push_back(img);
	this->_images_lock.writer_release(w);

	// Serialize img
	w = this->_serial_db_lock.writer_aquire();
	Serial::getDbSerial() << (this->images.size() == 1 ? "" : ",") << img;
	this->_serial_db_lock.writer_release(w);
}

void Proc::addRtn(const Ctx::Rtn& rtn)
{
	auto w = this->_rtns_lock.writer_aquire();
	// Check if rtn exists already
	if (this->rtns.find(rtn.getAddr()) != this->rtns.end()) {
		this->_rtns_lock.writer_release(w);
		return;
	}
	// Add routine
	this->rtns.insert(std::make_pair(rtn.getAddr(), rtn));
	this->_rtns_lock.writer_release(w);

	// Serialize routine
	w = this->_serial_rtn_lock.writer_aquire();
	Serial::getRtnSerial() << rtn;
	this->_serial_rtn_lock.writer_release(w);
}

void Proc::addBb(const Ctx::Bb& bb)
{
	auto w = this->_bbs_lock.writer_aquire();
	// Check if bb exists already
	if (this->bbs.find(bb.getAddr()) != this->bbs.end()) {
		this->_bbs_lock.writer_release(w);
		return;
	}
	// Add bb
	this->bbs.insert(std::make_pair(bb.getAddr(), bb.getIdx()));
	this->_bbs_lock.writer_release(w);

	// Serialize bb
	w = this->_serial_bb_lock.writer_aquire();
	Serial::getBbSerial() << bb;
	this->_serial_bb_lock.writer_release(w);
}

void Proc::addBbExecuted(const Ctx::BbExecuted& bbe)
{
	// Serialize bbe
	auto w = this->_serial_trace_lock.writer_aquire();
	Serial::getTraceSerial() << bbe;
	this->_serial_trace_lock.writer_release(w);
}

void Proc::addBbOutOfWhitelisted(const OS_THREAD_ID os_tid, const THREADID pin_tid)
{
	// Serialize the event 
	// "bb_idx,os_tid,pin_tid,rtn_called_idx";
	std::stringstream entry;
	entry << "-1," << std::dec << os_tid << "," << std::dec << pin_tid << ",-1\n";
	auto w = this->_serial_trace_lock.writer_aquire();
	Serial::getTraceSerial() << entry.str();
	this->_serial_trace_lock.writer_release(w);
}

const Rtn* Proc::getRtn(ADDRINT addr) const
{
	auto r = const_cast<Proc*>(this)->_rtns_lock.reader_aquire();
	auto it = this->rtns.find(addr);
	const Rtn* ret = it == this->rtns.end() ? NULL : &it->second;
	const_cast<Proc*>(this)->_rtns_lock.reader_release(r);
	return ret;
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
	int32_t ret = static_cast<decltype(ret)>(this->images.size());
	const_cast<Proc*>(this)->_images_lock.reader_release(r);
	return ret;
}

const int32_t Proc::getNumRtn() const
{
	auto r = const_cast<Proc*>(this)->_rtns_lock.reader_aquire();
	int32_t ret = static_cast<decltype(ret)>(this->rtns.size());
	const_cast<Proc*>(this)->_rtns_lock.reader_release(r);
	return ret;
}

const int32_t Proc::getNumBb() const
{
	auto r = const_cast<Proc*>(this)->_bbs_lock.reader_aquire();
	int32_t ret = static_cast<decltype(ret)>(this->bbs.size());
	const_cast<Proc*>(this)->_bbs_lock.reader_release(r);
	return ret;
}

const uint32_t Proc::getBbIdx(ADDRINT low_addr) const
{
	auto r = const_cast<Proc*>(this)->_bbs_lock.reader_aquire();
	auto it = this->bbs.find(low_addr);
	if (it == this->bbs.end())
		RAISE_EXCEPTION("`getBb` when bb doesnt exist! %p", (void*)low_addr);
	uint32_t ret = it->second;
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

bool Proc::isRtnSerialized(ADDRINT addr) const
{
	auto r = const_cast<Proc*>(this)->_rtns_lock.reader_aquire();
	bool ret = this->rtns.find(addr) != this->rtns.end();
	const_cast<Proc*>(this)->_rtns_lock.reader_release(r);
	return ret;
}

bool Proc::isToBeLogged(ADDRINT addr) const
{
	auto img = this->getImageExecutable(addr);
	if (img == NULL)
		return Knobs::isLogDynamicallyGenerated();
	return img->isWhitelisted();
}