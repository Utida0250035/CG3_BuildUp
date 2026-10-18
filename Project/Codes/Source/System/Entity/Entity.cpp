#include "System/Entity/Entity.h"

#include <ranges>

namespace Atrum {

Entity::~Entity() {
  if (parent_) {
    parent_->RemoveChild(this);
  }

  for (auto &child : childs_) {
    delete child;
  }

  childs_.clear();
}

void Entity::Initialize() {

  transform_ = initialTransform_;

  for (auto &cmpBox : updCmpBoxes_) {
    cmpBox.component->Initialize();
  }

  for (auto &cmpBox : drawCmpBoxes_) {
    cmpBox.component->Initialize();
  }

  for (auto &child : childs_) {

    child->Initialize();
  }

  state_ = State::Active;
}

void Entity::Update() {
  if (state_ != State::Active)
    return;

  for (auto &cmpBox : updCmpBoxes_) {
    cmpBox.component->Update();
  }

  for (auto *child : childs_) {
    child->Update();
  }
}

void Entity::Draw() {
  if (state_ != State::Active)
    return;

  for (auto &cmpBox : drawCmpBoxes_) {
    cmpBox.component->Draw();
  }

  for (auto *child : childs_) {
    child->Draw();
  }
}

void Entity::Finalize() {
  for (auto &cmpBox : updCmpBoxes_ | std::views::reverse) {
    cmpBox.component->Finalize();
  }
}

Math::Matrix4x4 Entity::GetWorldMatrix() const {
  Math::Matrix4x4 worldMatrix = transform_.MakeWorldMatrix();

  if (parent_) {

    if (isInheritParentPositionOnly_) {

      Math::Vector3 parentWorldPos = parent_->GetWorldPosition();

      worldMatrix[3][0] += parentWorldPos.x;
      worldMatrix[3][1] += parentWorldPos.y;
      worldMatrix[3][2] += parentWorldPos.z;

    } else {

      worldMatrix = worldMatrix * parent_->GetWorldMatrix();
    }
  }

  return worldMatrix;
}

Math::Vector3 Entity::GetWorldPosition() const {

  Math::Matrix4x4 worldMatrix = GetWorldMatrix();

  return {worldMatrix[3][0], worldMatrix[3][1], worldMatrix[3][2]};
}

void Entity::SetParent(Entity *newParent) {
  if (this == newParent)
    return;

  if (parent_ == newParent)
    return;

  if (parent_) {
    parent_->RemoveChild(this);
  }

  parent_ = newParent;

  if (parent_) {
    parent_->childs_.push_back(this);
  }
}

void Entity::ResolveDependencies() {
  for (auto &cmpBox : updCmpBoxes_) {
    cmpBox.component->ResolveDependence();
  }

  for (auto &cmpbox : drawCmpBoxes_) {
    cmpbox.component->ResolveDependence();
  }

  for (auto *child : childs_) {
    child->ResolveDependencies();
  }
}

} // namespace Atrum