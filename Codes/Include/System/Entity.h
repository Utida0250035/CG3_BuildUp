#pragma once

#include "Hash/Hash64.h"
#include "Math/Transform.h"
#include "System/Component.h"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <memory>
#include <string>
#include <vector>

namespace Atrum {

	// コンテナとなるクラス
	class Entity {

	public:

		enum class State {
			Initialize,
			Sleep,
			Active,
			Destroy
		};

	private:

		Entity* parent_ = nullptr;

		std::vector<Entity*> childs_{};

		uint64_t nameHash_ = 0;

		uint64_t id_ = 0;

		struct ComponentBox {

			uint32_t priority = 0;

			std::unique_ptr<Component> component = nullptr;

			ComponentBox(uint32_t p, std::unique_ptr<Component> c) : priority(p), component(std::move(c)) {}

		};

		Math::TransformQ transform_{};

		std::vector<ComponentBox> componentBoxes_{};

		State state_ = State::Initialize;

	public:

		~Entity();

		void Initialize();
		void Execute();
		void Finalize();

		template<typename T>
			requires std::derived_from<T, Component>
		T* GetComponent() {
			for (auto& component : componentBoxes_) {
				if (auto p = dynamic_cast<T*>(component.get())) {
					return p;
				}
			}

			return nullptr;

		}

		Math::TransformQ& RefTransform() { return transform_; }
		const Math::TransformQ& GetTransform() { return transform_; }

		template<typename T>
			requires std::derived_from<T, Component>
		bool HasComponent() const { return GetComponent<T>(); }

		template<typename T, typename... Args>
			requires std::derived_from<T, Component>
		void AddComponent(const uint32_t priority, Args&&... args) {

			if (HasComponent<T>()) {

				assert(false && "Component is already added to entity");

				return;

			}

			std::unique_ptr<Component> component = std::make_unique<T>(std::forward<Args>(args)...);

			component->SetOwner(this);

			componentBoxes_.emplace_back(priority, std::move(component));

			// 追加するたびに優先度でソート（昇順）
			std::sort(componentBoxes_.begin(), componentBoxes_.end(),
				[](const ComponentBox& a, const ComponentBox& b) {
				return a.priority < b.priority;
			});

		}

		explicit Entity(const std::string& name) : nameHash_(Hash64(name)) {}

		uint64_t GetHash() const { return nameHash_; }

		uint64_t GetId() const { return id_; }

		State GetState() const { return state_; }

		Math::Matrix4x4 GetWorldMatrix() const;

		bool IsActive() const { return state_ == State::Active; }

		void RemoveChild(Entity* target) { std::erase(childs_, target); }

		void SetParent(Entity* newParent);

		void SetHash(const uint64_t nameHash) { nameHash_ = nameHash; }

		void SetHash(const std::string& name) { nameHash_ = Hash64(name); }

		void SetId(const uint64_t id) { id_ = id; }

		void RequestDestroy() { state_ = State::Destroy; }

	};

}