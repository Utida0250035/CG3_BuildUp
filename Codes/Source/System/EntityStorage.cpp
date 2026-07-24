#include "Hash/Hash64.h"
#include "System/Entity.h"
#include "System/EntityStorage.h"

#include <cassert>

namespace Atrum {

	EntityStorage* EntityStorage::instance = nullptr;

	void EntityStorage::Create(const std::string& name, std::unique_ptr<Entity>& pEntity) {

		assert(!hashToIdMap_.contains(Hash64(name)));

		uint64_t currentId = nextId_++;

		pEntity->SetHash(Hash64(name));
		pEntity->SetId(currentId);

		entitys_.emplace_back(std::move(pEntity));

		hashToIdMap_.emplace(Hash64(name), currentId);
		idToIndexMap_.emplace(currentId, entitys_.size() - 1);

	}

	EntityHandle EntityStorage::Find(const uint64_t id) {

		auto indexSearch = idToIndexMap_.find(id);

		if (indexSearch == idToIndexMap_.end()) {

			return EntityHandle();

		}

		assert(entitys_[indexSearch->second]);

		return EntityHandle(
			entitys_[indexSearch->second].get(),
			id,
			entitys_[indexSearch->second]->GetHash()
		);

	}

	EntityHandle EntityStorage::Find(const std::string& name) {

		auto idSearch = hashToIdMap_.find(Hash64(name));

		if (idSearch == hashToIdMap_.end()) return EntityHandle();

		auto indexSearch = idToIndexMap_.find(idSearch->second);

		if (indexSearch == idToIndexMap_.end()) return EntityHandle();

		assert(entitys_[indexSearch->second]);

		return EntityHandle(
			entitys_[indexSearch->second].get(),
			idSearch->second,
			Hash64(name)
		);

	}

	bool EntityStorage::IsValid(const EntityHandle& handle) {

		auto indexSearch = idToIndexMap_.find(handle.id_);

		if (indexSearch == idToIndexMap_.end())return false;

		if (handle.ptr_ != entitys_[indexSearch->second].get()) return false;

		return true;

	}

	void EntityStorage::RequestErase(EntityHandle& handle) {

		eraseRequestedHandles_.push_back(handle);

		handle.id_ = 0;
		handle.ptr_ = nullptr;

	}

	void EntityStorage::Erase(EntityHandle& handle) {

		auto indexSearch = idToIndexMap_.find(handle.id_);

		if (indexSearch == idToIndexMap_.end()) {

			handle.id_ = 0;
			handle.ptr_ = nullptr;

			return;

		}

		size_t indexToRemove = indexSearch->second;

		size_t lastIndex = entitys_.size() - 1;

		if (indexToRemove != lastIndex) {

			// 1. 最後尾のEntityを取得
			Entity* lastEntity = entitys_.back().get();

			// 移動させるEntityの名前（ハッシュ）を特定して、マップの値を更新
			// Entity自身が自分の名前/ハッシュを知っているとここが楽です
			idToIndexMap_[lastEntity->GetId()] = indexToRemove;

			// スワップ
			std::swap(entitys_[indexToRemove], entitys_[lastIndex]);
		}

		// 4. 削除
		hashToIdMap_.erase(handle.nameHash_);
		idToIndexMap_.erase(handle.id_);
		entitys_.pop_back();

		handle.id_ = 0;
		handle.ptr_ = nullptr;

	}

	void EntityStorage::EraseRequested() {

		for (auto& handle : eraseRequestedHandles_) {

			Erase(handle);

			assert(!handle);

		}

		eraseRequestedHandles_.clear();

	}

	void EntityStorage::Update() {

		EraseRequested();

	}

}