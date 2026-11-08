#include "System/Entity/Entity.h"

#include <ranges>

namespace Atrum {

Entity::~Entity() {
  if (parent_) {
    parent_->RemoveChild(this);
    parent_ = nullptr;
  }

  // 子の実体は CmpChildEntitys や CmpParry が持つ
  for (auto &child : childs_) {
    if (child) {
      child->parent_ = nullptr;
    }
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

Math::Vector3 Entity::TransformLocalPointToWorld(
    const Math::Vector3 &localPoint) const {
  Math::TransformLH localTransform{};
  localTransform.translate = localPoint;
  const Math::Matrix4x4 worldMatrix =
      localTransform.MakeWorldMatrix() * GetWorldMatrix();
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

void Entity::AddTag(const std::string &tagName) {
  if (tagName.empty()) {
    return;
  }
  tagHashes_.insert(Hash64(tagName));
}

void Entity::RemoveTag(const std::string &tagName) {
  if (tagName.empty()) {
    return;
  }
  tagHashes_.erase(Hash64(tagName));
}

bool Entity::HasTag(const std::string &tagName) const {
  if (tagName.empty()) {
    return false;
  }
  return tagHashes_.find(Hash64(tagName)) != tagHashes_.end();
}

void Entity::SetTagsFromNames(const std::vector<std::string> &tagNames) {
  tagHashes_.clear();
  for (const std::string &tagName : tagNames) {
    if (tagName.empty()) {
      continue;
    }
    tagHashes_.insert(Hash64(tagName));
  }
}

void Entity::SetTagHashes(const std::vector<uint64_t> &tagHashes) {
  tagHashes_.clear();
  for (const uint64_t tagHash : tagHashes) {
    tagHashes_.insert(tagHash);
  }
}

void Entity::AttachUpdComponent(std::unique_ptr<ComponentUpd> component) {
  component->SetOwner(this);
  updCmpBoxes_.emplace_back(static_cast<uint64_t>(component->UpdGroup()),
                            component.get());
  updComponents_.emplace_back(std::move(component));
  SortUpdCmpBoxes();
}

void Entity::AttachDrawComponent(std::unique_ptr<ComponentDraw> component) {
  component->SetOwner(this);
  drawCmpBoxes_.emplace_back(static_cast<uint64_t>(component->DrawGroup()),
                             component.get());
  drawComponents_.emplace_back(std::move(component));
  SortDrawCmpBoxes();
}

void Entity::SortUpdCmpBoxes() {
  std::sort(updCmpBoxes_.begin(), updCmpBoxes_.end(),
            [](const UpdCmpBox &a, const UpdCmpBox &b) {
              return a.priority < b.priority;
            });
}

void Entity::SortDrawCmpBoxes() {
  std::sort(drawCmpBoxes_.begin(), drawCmpBoxes_.end(),
            [](const DrawCmpBox &a, const DrawCmpBox &b) {
              return a.priority < b.priority;
            });
}

void Entity::SortComponents() {
  SortUpdCmpBoxes();
  SortDrawCmpBoxes();
}

void Entity::SetUpdCmpPriority(ComponentUpd *component,
                               const uint64_t priority) {
  if (component == nullptr) {
    return;
  }
  for (UpdCmpBox &box : updCmpBoxes_) {
    if (box.component == component) {
      box.priority = priority;
      SortUpdCmpBoxes();
      return;
    }
  }
}

void Entity::SetDrawCmpPriority(ComponentDraw *component,
                                const uint64_t priority) {
  if (component == nullptr) {
    return;
  }
  for (DrawCmpBox &box : drawCmpBoxes_) {
    if (box.component == component) {
      box.priority = priority;
      SortDrawCmpBoxes();
      return;
    }
  }
}

} // namespace Atrum