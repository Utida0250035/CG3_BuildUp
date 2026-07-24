#include "System/Entity.h"
#include <ranges>

namespace Atrum {

	Entity::~Entity() {

		if (parent_) {
			parent_->RemoveChild(this);
		}

		for (auto& child : childs_) {

			delete child;

		}

		childs_.clear();

	}

	void Entity::Initialize() {

		for (auto& cmpBox : componentBoxes_) {
			cmpBox.component->Initialize();
		}

		state_ = State::Active;

	}

	void Entity::Execute() {

		if (state_ != State::Active) return;

		for (auto& cmpBox : componentBoxes_) {
			cmpBox.component->Execute();
		}

		for (auto* child : childs_) {

			child->Execute();

		}

	}

	void Entity::Finalize() {

		for (auto& cmpBox : componentBoxes_ | std::views::reverse) {

			cmpBox.component->Finalize();

		}

	}

	Math::Matrix4x4 Entity::GetWorldMatrix() const {

		Math::Matrix4x4 worldMatrix = transform_.MakeWorldMatrix();

		if (parent_) {

			worldMatrix = parent_->GetWorldMatrix() * worldMatrix;

		}

		return worldMatrix;

	}

	void Entity::SetParent(Entity* newParent) {

		if (this == newParent) return;

		if (parent_ == newParent) return;

		if (parent_) {

			parent_->RemoveChild(this);

		}

		parent_ = newParent;

		if (parent_) {

			parent_->childs_.push_back(this);

		}

	}

}