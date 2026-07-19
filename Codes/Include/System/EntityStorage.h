#pragma once

#include "System/Entity.h"

#include <string>
#include <memory>
#include <unordered_map>

namespace Atrum {

	class EntityStorage final {

	private:

		EntityStorage() = default;
		~EntityStorage() = default;

		std::vector<std::unique_ptr<Entity>> entitys_{};

		std::unordered_map<uint64_t, size_t> entityMap_{};

	public:

		void StoreEntity(const std::string& name, std::unique_ptr<Entity>& pEntity);

		void EraseEntity(const std::string& name);

		Entity* Find(const std::string& name);

	};

}