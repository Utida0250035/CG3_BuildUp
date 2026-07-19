#include "Hash/Hash64.h"
#include "System/Entity.h"
#include "System/EntityStorage.h"

#include <cassert>

namespace Atrum {

    void EntityStorage::StoreEntity(const std::string& name, std::unique_ptr<Entity>& pEntity) {

        assert(!entityMap_.contains(Hash64(name)));

        entitys_.emplace_back(std::move(pEntity));

        entityMap_.emplace(Hash64(name), entitys_.size());

    }

    void EntityStorage::EraseEntity(const std::string& name) {

        size_t hash = Hash64(name);
        size_t indexToRemove = entityMap_.at(hash);
        size_t lastIndex = entitys_.size() - 1;

        if (indexToRemove != lastIndex) {

            // 1. 最後尾のEntityを取得
            Entity* lastEntity = entitys_.back().get();

            // 移動させるEntityの名前（ハッシュ）を特定して、マップの値を更新
            // Entity自身が自分の名前/ハッシュを知っているとここが楽です
            entityMap_[lastEntity->GetHash()] = indexToRemove;

            // スワップ
            std::swap(entitys_[indexToRemove], entitys_[lastIndex]);
        }

        // 4. 削除
        entityMap_.erase(hash);
        entitys_.pop_back();

    }

    Entity* EntityStorage::Find(const std::string& name) {

        auto search = entityMap_.find(Hash64(name));

        assert(search != entityMap_.end());

        assert(entitys_[search->second]);

        return entitys_[search->second].get();

    }

}