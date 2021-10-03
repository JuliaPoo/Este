#pragma once

#include "Este\sync.hpp"
#include <unordered_map>

namespace Ctx // Forward declaration
{
	class Image;
	class Bb;
	class BbExecuted;
	class Rtn;
}

namespace Ctx {

	class Proc {

	public:

		Proc();

		~Proc();

		// Serializer
		// std::ostream& operator<<(std::ostream& out) const;

		// Adds image to this->images and serializes it into serial::db (.este.json)
		void addImage(const Ctx::Image& img);

		// Serializes rtn into serial::rtn (.rtn.csv)
		// Does nothing if rtn already serialized
		void addRtn(const Ctx::Rtn& rtn);

		// Adds bb to bbs
		// Does nothing if bb already serialized
		void addBb(const Ctx::Bb& bb);

		// Serializes BbExecuted into serial::trace (.trace.csv)
		void addBbExecuted(const Ctx::BbExecuted& bbe);

		// Get routine that starts at addr. Returns NULL if not found.
		const Rtn* getRtn(ADDRINT addr) const;

		// Get image whereby addr is situated in. Returns NULL if not found.
		const Image* getImage(ADDRINT addr) const;

		// Get image whereby addr is situated in
		// with additional condition of being executable. 
		// Returns NULL if not found.
		const Image* getImageExecutable(ADDRINT addr) const;

		// Get number of images
		const int32_t getNumImages() const;

		// Get number of routines
		const int32_t getNumRtn() const;

		// Get number of bbs
		const int32_t getNumBb() const;

		// Get bb index
		const uint32_t getBbIdx(ADDRINT low_addr) const;

		// Check if bb already executed once.
		bool isBbExecuted(ADDRINT addr_low) const;

		// Checks if routine has already been serialized
		bool isRtnSerialized(ADDRINT addr) const;

	private:

		// Lock that ensures safe access into this->images
		Sync::RW _images_lock;

		// Lock that ensures safe access into serial::db
		Sync::RW _serial_db_lock;

		// Lock that ensures safe access into this->bbs
		Sync::RW _bbs_lock;

		// Lock that ensures safe access into serial::rtn
		Sync::RW _serial_rtn_lock;

		// Lock that ensures safe access into this->rtns
		Sync::RW _rtns_lock;

		// Lock that ensures safe access into serial::bb
		Sync::RW _serial_bb_lock;

		// Lock that ensures safe access into serial::trace
		Sync::RW _serial_trace_lock;

		// All images loaded
		std::vector<Image> images;

		// All routines encountered
		// starting address : rtn object
		std::tr1::unordered_map<ADDRINT, Rtn> rtns;

		// All unique bb encountered
		// lower address : bb index inside .bb.csv file (starting from 0)
		std::tr1::unordered_map<ADDRINT, uint32_t> bbs;
	};
}