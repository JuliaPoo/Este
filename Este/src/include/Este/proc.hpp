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
		void addImage(Ctx::Image img);

	private:

		// Lock that ensures safe access into this->images
		Sync::RW _images_lock;

		// Lock that ensures safe access into serial::db
		Sync::RW _serial_db_lock;

		std::vector<Image> images;

	};
}