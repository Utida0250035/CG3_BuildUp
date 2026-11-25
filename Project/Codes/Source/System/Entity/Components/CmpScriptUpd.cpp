#include "System/Entity/Components/CmpScriptUpd.h"

#include "Engine/AtrumEngine.h"
#include "Engine/EngineAccess.h"
#include "System/DeltaTime.h"
#include "System/Entity/Entity.h"
#include "System/Entity/UpdCmpGroupUtil.h"
#include "System/Scripting/LuaJitScriptHost.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif

namespace Atrum {

namespace {

uint64_t ResolveEntityKey(const Entity *owner) {
  if (owner == nullptr) {
    return 0;
  }
  if (owner->GetId() != 0) {
    return owner->GetId();
  }
  return static_cast<uint64_t>(reinterpret_cast<std::uintptr_t>(owner));
}

} // namespace

void CmpScriptUpd::ConfigureUpdGroup(const bool useNumericPriority,
                                     const UpdCmpGroup enumGroup,
                                     const uint64_t numericPriority) {
  if (useNumericPriority) {
    updGroup_ = UpdCmpGroupFromNumericPriority(numericPriority);
    updSortPriority_ = numericPriority;
  } else {
    updGroup_ = enumGroup;
    updSortPriority_ = UpdCmpGroupToPriority(enumGroup);
  }
}

void CmpScriptUpd::SyncUpdSortPriorityToEntity() {
  RefOwner().SetUpdCmpPriority(this, updSortPriority_);
}

void CmpScriptUpd::ResolveDependence() {
  host_ = AtrumEngine::Instance()->GetLuaScriptHost();
  entityKey_ = ResolveEntityKey(&RefOwner());

  if (host_ == nullptr || !host_->IsReady() || scriptPath_.empty()) {
    disabled_ = true;
    return;
  }

  Script::LuaScriptInvokeContext context{};
  context.owner = &RefOwner();
  context.sourceComponent = this;
  context.entityKey = entityKey_;

  disabled_ = !host_->Invoke(scriptPath_, Script::LuaScriptHook::Resolve, context);
}

void CmpScriptUpd::Update() {
  if (disabled_ || host_ == nullptr || scriptPath_.empty()) {
    return;
  }

  FrameDeltaTime *frameDelta = EngineFrameDeltaTime();
  const float deltaTime =
      frameDelta != nullptr ? frameDelta->GetDeltaTime() : 0.0f;

  Script::LuaScriptInvokeContext context{};
  context.owner = &RefOwner();
  context.sourceComponent = this;
  context.entityKey = entityKey_;
  context.deltaTime = deltaTime;

  disabled_ = !host_->Invoke(scriptPath_, Script::LuaScriptHook::Update, context);
}

void CmpScriptUpd::Finalize() {
  if (host_ != nullptr && !scriptPath_.empty() && entityKey_ != 0) {
    host_->ReleaseInstance(scriptPath_, entityKey_);
  }
  host_ = nullptr;
}

void CmpScriptUpd::DevelopGui() {
#ifdef USE_IMGUI
  ImGui::Text("CmpScriptUpd");
  ImGui::TextDisabled("path: %s", scriptPath_.c_str());
  ImGui::TextDisabled("UpdGroup: %s (%llu)",
                      UpdCmpGroupToName(updGroup_),
                      static_cast<unsigned long long>(static_cast<uint64_t>(updGroup_)));
  if (disabled_) {
    ImGui::TextColored({1.0f, 0.35f, 0.35f, 1.0f}, "disabled");
  }
#endif
}

} // namespace Atrum