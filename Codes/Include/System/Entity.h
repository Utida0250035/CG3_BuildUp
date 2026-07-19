#pragma once

#include "System/Component.h"
#include "Hash/Hash64.h"

#include <cassert>
#include <vector>
#include <memory>
#include <concepts>

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

		uint64_t nameHash_ = 0;

		struct ComponentBox {

			uint32_t priority = 0;

			std::unique_ptr<Component> component = nullptr;
		
		};

		std::vector<std::unique_ptr<Component>> components_{};

		State state_ = State::Initialize;

	public:

		void Initialize();
		void Execute();
		void Finalize();

		template<typename T>
			requires std::derived_from<T, Component>
		T* GetComponent() {
			for (auto& component : components_) {
				if (auto p = dynamic_cast<T*>(component.get())) {
					return p;
				}
			}

			return nullptr;

		}

		template<typename T>
			requires std::derived_from<T, Component>
		bool HasComponent() const { return GetComponent<T>(); }

		template<typename T, typename... Args>
		requires std::derived_from<T, Component>
		void AddComponent(Args&&... args) {
			
			if (HasComponent<T>()){
				
				assert(false && "Component is already added to entity");
				
				return;

			}

			std::unique_ptr<Component> component = std::make_unique<T>(std::forward<Args>(args)...);

			component->SetOwner(this);

			components_.emplace_back(component);

		}

		explicit Entity(const std::string& name) : nameHash_(Hash64(name)) {}

		uint64_t GetHash() const { return nameHash_; }

		State GetState() const { return state_; }

		bool IsActive() const { return state_ == State::Active; }

		void RequestDestroy() { state_ = State::Destroy; }

	};

}