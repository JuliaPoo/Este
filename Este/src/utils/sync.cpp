#include "Este\sync.hpp"
#include "Este\errors.hpp"

#include <stdlib.h>
#include <time.h>

using namespace Sync;

TimedMutex::TimedMutex(uint timeout)
{
	this->timeout = timeout;
	PIN_MutexInit(&this->mutex);
}

TimedMutex::~TimedMutex()
{
	PIN_MutexFini(&this->mutex);
}

bool TimedMutex::lock()
{
	if (this->timeout) {
		auto t = time(NULL); // PIN got problem?
		while (!PIN_MutexTryLock(&this->mutex)) {
			if (time(NULL) - t > this->timeout)
				return false;
			PIN_Sleep(100);
		}
	}
	else PIN_MutexLock(&this->mutex);
	return true;
}

void TimedMutex::unlock()
{
	PIN_MutexUnlock(&this->mutex);
}

RW::RW(void)
{
}

RW::~RW(void)
{
}

TOKEN RW::writer_aquire(void)
{
	auto tok = this->_writer_aquire();
	if (!tok) {
		RAISE_EXCEPTION("Failed to aquire writer permissions to `%s`", this->toStr().c_str());
		return 0;
	}
	return tok;
}

TOKEN RW::reader_aquire(void)
{
	auto tok = this->_reader_aquire();
	if (!tok) {
		RAISE_EXCEPTION("Failed to aquire reader permissions to `%s`", this->toStr().c_str());
		return 0;
	}
	return tok;
}

bool RW::writer_release(TOKEN owner)
{
	if (!this->_writer_release(owner)) {
		RAISE_EXCEPTION("Failed to release writer permissions to `%s`", this->toStr().c_str());
		return false;
	}
	return true;
}

bool RW::reader_release(TOKEN owner)
{
	if (!this->_reader_release(owner)) {
		RAISE_EXCEPTION("Failed to release reader permissions to `%s`", this->toStr().c_str());
		return false;
	}
	return true;
}

const std::string RW::toStr(void) const
{
	return "None";
}

TOKEN RW::_writer_aquire(void)
{
	if (!this->mutex_rsrc.lock())
		RAISE_EXCEPTION("Failed to lock mutex_rsrc in `%s:%p`", this->toStr().c_str(), this);

	auto ret = rand() | 1;
	this->wowner = ret;

	return ret;
}

TOKEN RW::_reader_aquire(void)
{
	if (!this->mutex_read.lock())
		RAISE_EXCEPTION("Failed to lock mutex_read in `%s:%p`", this->toStr().c_str(), this);

	this->_read_count++;
	TOKEN tok = rand() | 1;
	this->rowner.insert(tok);
	if (_read_count == 1)
		if (!this->mutex_rsrc.lock())
			RAISE_EXCEPTION("Failed to lock mutex_rsrc in `%s:%p`", this->toStr().c_str(), this);

	this->mutex_read.unlock();
	return tok;
}

bool RW::_writer_release(TOKEN owner)
{

	if (!this->wowner) {
		RAISE_EXCEPTION("No writer owner to `%s` to release!", this->toStr().c_str());
	}
	if (this->wowner != owner) {
		RAISE_EXCEPTION("Attempted to release invalid writer owner to `%s`", this->toStr().c_str());
	}
	this->wowner = 0;

	this->mutex_rsrc.unlock();
	return true;
}

bool RW::_reader_release(TOKEN owner)
{
	if (!this->mutex_read.lock())
		RAISE_EXCEPTION("Failed to lock mutex_read in `%s:%p`", this->toStr().c_str(), this);

	if (!this->rowner.size()) {
		RAISE_EXCEPTION("No reader owner to `%s` to release!", this->toStr().c_str());
	}
	auto it = this->rowner.find(owner);
	if (it == this->rowner.end()) {
		RAISE_EXCEPTION("Attempted to release invalid reader owner to `%s`", this->toStr().c_str());
	}
	this->rowner.erase(it);
	this->_read_count--;
	if (_read_count == 0)
		this->mutex_rsrc.unlock();

	this->mutex_read.unlock();
	return true;
}
