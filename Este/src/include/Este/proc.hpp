#pragma once

#include "Este\sync.hpp"
#include <unordered_map>

namespace Ctx // Forward declaration
{
	class Image;
	class Bb;
}

namespace Ctx {

	class Proc {

	public:

		Proc();

		~Proc();

		// Serializer
		// std::ostream& operator<<(std::ostream& out) const;

		// Adds image to this->images and serializes it into serial::db (.este.json)
		void addImage(Ctx::Image& img);

		// Serializes bb into serial::bb (.bb.csv)
		// Returns if bb already serialized
		void addBb(Ctx::Bb& bb);

		// Get image whereby addr is situated in. Returns NULL if not found.
		const Image* getImage(ADDRINT addr) const;

		// Get image whereby addr is situated in
		// with additional condition of being executable. 
		// Returns NULL if not found.
		const Image* getImageExecutable(ADDRINT addr) const;

		// Get number of images
		const int32_t getNumImages() const;

		// Get number of bbs
		const int32_t getNumBb() const;

		// Check if bb already executed once
		bool isBbExecuted(ADDRINT addr_low) const;

	private:

		// Lock that ensures safe access into this->images
		Sync::RW _images_lock;

		// Lock that ensures safe access into serial::db
		Sync::RW _serial_db_lock;

		// Lock that ensures safe access into this->bbs
		Sync::RW _bbs_lock;

		// Lock that ensures safe access into serial::bb
		Sync::RW _serial_bb_lock;

		// All images loaded
		std::vector<Image> images;

		// All unique bb encountered
		// lower address : bb index inside .bb.csv file (starting from 0)
		std::tr1::unordered_map<ADDRINT, int32_t> bbs;
	};
}