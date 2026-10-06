#pragma once

#include "System/Entity.h"

#include <string>
#include <memory>
#include <unordered_map>
#include <cassert>

namespace Atrum {

	class EntityHandle;

	class EntityStorage final {

	private:

		static EntityStorage* instance;

		EntityStorage() { entitys_.reserve(65536); };
		~EntityStorage() = default;

		std::vector<std::unique_ptr<Entity>> entitys_{};

		std::unordered_map<uint64_t, uint64_t> hashToIdMap_{};
		std::unordered_map<uint64_t, size_t> idToIndexMap_{};

		std::vector<EntityHandle> eraseRequestedHandles_{};

		uint64_t nextId_ = 1;

		EntityHandle Find(const uint64_t id);

		void EraseRequested();

		void Erase(EntityHandle& handle);

	public:

		void Create(const std::string& name, std::unique_ptr<Entity>& pEntity);

		EntityHandle Find(const std::string& name);

		bool IsValid(const EntityHandle& handle);

		void RequestErase(EntityHandle& handle);

		void Update();

		static EntityStorage* GetInstance() {

			if (!instance) {

				instance = new EntityStorage();

			}

			return instance;

		}

		static void Destroy() {

			if (instance) {

				delete instance;
				instance = nullptr;

			}

		}

	};

	class EntityHandle {

	private:

		friend EntityStorage;

		Entity* ptr_ = nullptr;
		uint64_t id_ = 0;
		uint64_t nameHash_ = 0;

		EntityHandle(Entity* ptr, uint64_t id, uint64_t nameHash) :ptr_(ptr), id_(id), nameHash_(nameHash) {}
		EntityHandle() = default;

	public:

		bool IsValid() const {

			return EntityStorage::GetInstance()->IsValid(*this);

		}

		Entity* operator->() const {
			assert(ptr_);
			return ptr_;
		}

		explicit operator bool() {

			return IsValid();

		}

	};

}