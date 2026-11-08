#include "UX/OperatorGuidanceSystem.h"

#include "Scene/SceneScreenUi.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Platform/Window/Window.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/WhiteTexture.h"
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <numbers>

namespace Atrum::OperatorGuidance {

namespace {

void DrawScreenRect(AtrumEngine *engine, const Math::Vector4 &color, const float x,
                    const float y, const float width, const float height) {
  Math::Transform uvTransform{};
  Math::Transform rectTransform{};
  rectTransform.translate = {x, y, 0.0f};
  rectTransform.scale = {width, height, 1.0f};

  engine->DrawSpriteRect(WhiteTextureIndex(), color, uvTransform, rectTransform,
                         Math::Vector2{1.0f, 1.0f}, Math::Vector2{0.0f, 0.0f});
}

uint32_t LoadBodyTexture(AtrumEngine *engine,
                         const std::vector<std::string> &paths) {
  if (engine == nullptr) {
    return UINT32_MAX;
  }
  for (const std::string &path : paths) {
    const uint32_t index = engine->GetTexture(path);
    if (index != UINT32_MAX) {
      return index;
    }
  }
  return UINT32_MAX;
}

} // namespace

System &System::Instance() {
  static System instance;
  return instance;
}

bool System::LoadFromJsonFile(const std::string &path) {
  hints_.clear();
  std::ifstream stream(path);
  if (!stream) {
    return false;
  }

  nlohmann::json json;
  try {
    stream >> json;
  } catch (...) {
    return false;
  }

  for (const auto &item : json.at("hints")) {
    HintDefinition hint{};
    hint.id = item.at("id").get<std::string>();
    hint.actionId = item.at("actionId").get<std::string>();
    hint.message = item.value("message", std::string{});
    hint.idleBeforeShowSeconds = item.value("idleBeforeShowSeconds", 15.0f);
    hint.displayDurationSeconds = item.value("displayDurationSeconds", 6.0f);
    hint.anchorX = item.value("anchorX", 0.75f);
    hint.anchorY = item.value("anchorY", 0.32f);
    hint.bodyScale = item.value("bodyScale", 0.42f);
    if (item.contains("bodyTexturePaths")) {
      for (const auto &p : item.at("bodyTexturePaths")) {
        hint.bodyTexturePaths.push_back(p.get<std::string>());
      }
    }
    hints_.push_back(std::move(hint));
    secondsSinceLastAction_[hint.actionId] = 0.0f;
  }
  return !hints_.empty();
}

void System::ResetSession() {
  activeHintId_.clear();
  activeMessage_.clear();
  activeDisplayRemaining_ = 0.0f;
  for (auto &entry : secondsSinceLastAction_) {
    entry.second = 0.0f;
  }
  textureCache_.clear();
}

void System::NotifyAction(const std::string &actionId) {
  secondsSinceLastAction_[actionId] = 0.0f;
  for (const HintDefinition &hint : hints_) {
    if (hint.actionId == actionId && activeHintId_ == hint.id) {
      activeHintId_.clear();
      activeMessage_.clear();
      activeDisplayRemaining_ = 0.0f;
    }
  }
}

void System::Update(const float deltaSeconds) {
  for (auto &entry : secondsSinceLastAction_) {
    entry.second += deltaSeconds;
  }

  if (activeDisplayRemaining_ > 0.0f) {
    activeDisplayRemaining_ -= deltaSeconds;
    if (activeDisplayRemaining_ <= 0.0f) {
      activeHintId_.clear();
      activeMessage_.clear();
      activeDisplayRemaining_ = 0.0f;
    }
    return;
  }

  const HintDefinition *best = nullptr;
  float bestIdle = 0.0f;
  for (const HintDefinition &hint : hints_) {
    const auto it = secondsSinceLastAction_.find(hint.actionId);
    if (it == secondsSinceLastAction_.end()) {
      continue;
    }
    if (it->second < hint.idleBeforeShowSeconds) {
      continue;
    }
    if (best == nullptr || it->second > bestIdle) {
      best = &hint;
      bestIdle = it->second;
    }
  }

  if (best != nullptr) {
    activeHintId_ = best->id;
    activeMessage_ = best->message;
    activeDisplayRemaining_ = best->displayDurationSeconds;
  }
}

void System::Draw(AtrumEngine *engine, Camera *camera) const {
  if (engine == nullptr || camera == nullptr || activeDisplayRemaining_ <= 0.0f) {
    return;
  }

  const HintDefinition *active = nullptr;
  for (const HintDefinition &hint : hints_) {
    if (hint.id == activeHintId_) {
      active = &hint;
      break;
    }
  }
  if (active == nullptr) {
    return;
  }

  SceneScreenUi::PrepareDraw(engine, camera);
  engine->SetBlendModeForFrame(BlendMode::NORMAL);

  const float screenW = static_cast<float>(Window::kDefaultClientWidth);
  const float screenH = static_cast<float>(Window::kDefaultClientHeight);
  const float anchorX = screenW * active->anchorX;
  const float anchorY = screenH * active->anchorY;

  constexpr float kBubblePad = 12.0f;
  constexpr float kBodyW = 220.0f;
  constexpr float kBodyH = 120.0f;
  const float scaledW = kBodyW * active->bodyScale;
  const float scaledH = kBodyH * active->bodyScale;
  const float bubbleW = scaledW + kBubblePad * 2.0f;
  const float bubbleH = scaledH + kBubblePad * 2.0f + 18.0f;
  const float bubbleLeft = anchorX - bubbleW * 0.5f;
  const float bubbleTop = anchorY - bubbleH;

  const Math::Vector4 bubbleFill{0.08f, 0.12f, 0.16f, 0.88f};
  const Math::Vector4 bubbleBorder{0.55f, 0.78f, 0.95f, 0.95f};

  DrawScreenRect(engine, bubbleFill, bubbleLeft, bubbleTop, bubbleW, bubbleH);
  DrawScreenRect(engine, bubbleBorder, bubbleLeft, bubbleTop, bubbleW, 3.0f);
  DrawScreenRect(engine, bubbleBorder, bubbleLeft, bubbleTop + bubbleH - 3.0f,
                 bubbleW, 3.0f);
  DrawScreenRect(engine, bubbleBorder, bubbleLeft, bubbleTop, 3.0f, bubbleH);
  DrawScreenRect(engine, bubbleBorder, bubbleLeft + bubbleW - 3.0f, bubbleTop, 3.0f,
                 bubbleH);

  const float tailCenterX = anchorX;
  const float tailTop = bubbleTop + bubbleH - 2.0f;
  constexpr float kTailW = 22.0f;
  constexpr float kTailH = 16.0f;
  Math::Transform uvTransform{};
  Math::Transform rectTransform{};
  rectTransform.translate = {tailCenterX - kTailW * 0.5f, tailTop, 0.0f};
  rectTransform.scale = {kTailW, kTailH, 1.0f};
  rectTransform.rotate = {0.0f, 0.0f, std::numbers::pi_v<float> * 0.25f};
  engine->DrawSpriteRect(WhiteTextureIndex(), bubbleFill, uvTransform, rectTransform,
                         Math::Vector2{1.0f, 1.0f}, Math::Vector2{0.0f, 0.0f});

  uint32_t textureIndex = UINT32_MAX;
  const auto cached = textureCache_.find(active->id);
  if (cached != textureCache_.end()) {
    textureIndex = cached->second;
  } else {
    textureIndex = LoadBodyTexture(engine, active->bodyTexturePaths);
    textureCache_[active->id] = textureIndex;
  }

  if (textureIndex != UINT32_MAX) {
    const float bodyLeft = bubbleLeft + kBubblePad;
    const float bodyTop = bubbleTop + kBubblePad;
    Math::Transform bodyUv{};
    Math::Transform bodyXform{};
    bodyXform.translate = {bodyLeft, bodyTop, 0.0f};
    bodyXform.scale = {scaledW, scaledH, 1.0f};
    engine->DrawSpriteRect(textureIndex, Math::Vector4::White(), bodyUv, bodyXform,
                           Math::Vector2{1.0f, 1.0f}, Math::Vector2{0.0f, 0.0f});
  }

  engine->SetBlendModeForFrame(BlendMode::NONE);
}

} // namespace Atrum::OperatorGuidance
