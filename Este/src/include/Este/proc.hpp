#pragma once

#include "Este\image.hpp"
#include "Este\sync.hpp"

namespace Ctx {

	class Proc {

	public:

		Proc();

		~Proc();

		// Serializer
		// std::ostream& operator<<(std::ostream& out) const;

		// Adds image to this->images and serializes it into serial::db (.este.json)
		void addImage(Ctx::Image& img);

		// Get image whereby addr is situated in. Returns NULL if not found.
		const Image* getImage(ADDRINT addr) const;

		// Get image whereby addr is situated in
		// with additional condition of being executable. 
		// Returns NULL if not found.
		const Image* getImageExecutable(ADDRINT addr) const;

	private:

		// Lock that ensures safe access into this->images
		Sync::RW _images_lock;

		// Lock that ensures safe access into serial::db
		Sync::RW _serial_db_lock;

		std::vector<Image> images;

	};
}