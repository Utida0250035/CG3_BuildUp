#include "System/Entity.h"
#include <ranges>

namespace Atrum {

	void Entity::Initialize() {

		for (auto& component : components_) {
			component->Initialize();
		}

		state_ = State::Active;

	}

	void Entity::Execute() {

		for (auto& component : components_) {
			component->Execute();
		}

	}

	void Entity::Finalize() {

		for (auto& component : components_ | std::views::reverse) {

			component->Finalize();

		}

	}

}