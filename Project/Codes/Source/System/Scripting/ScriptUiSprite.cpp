#include "System/Scripting/ScriptUiSprite.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/EngineAccess.h"
#include "Engine/Resource/TextureStorage.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

#include <cstdint>
#include <filesystem>
#include <unordered_map>
#include <vector>

namespace Atrum::Script::UiSprite {

namespace {

struct QueuedSprite {
  uint32_t textureIndex = UINT32_MAX;
  Entry entry{};
};

std::vector<QueuedSprite> &Queue() {
  static std::vector<QueuedSprite> queue;
  return queue;
}

/// パス→テクスチャ index（存在しないファイルは UINT32_MAX を記憶して毎フレームの存在確認を避ける）
uint32_t ResolveTexture(const std::string &path) {
  static std::unordered_map<std::string, uint32_t> cache;
  const auto found = cache.find(path);
  if (found != cache.end()) {
    return found->second;
  }

  uint32_t index = UINT32_MAX;
  if (std::filesystem::exists(path)) {
    index = ::Atrum::EngineGetTexture(path);
  }
  cache.emplace(path, index);
  return index;
}

} // namespace

void BeginFrame() { Queue().clear(); }

void Push(const Entry &entry) {
  const uint32_t textureIndex = ResolveTexture(entry.texturePath);
  if (textureIndex == UINT32_MAX) {
    return;
  }
  Queue().push_back(QueuedSprite{textureIndex, entry});
}

void Draw(AtrumEngine *engine, const float screenWidth,
          const float screenHeight) {
  if (engine == nullptr || Queue().empty()) {
    return;
  }

  TextureStorage *const storage = ::Atrum::EngineTextureStorage();
  if (storage == nullptr) {
    return;
  }

  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();

  for (const QueuedSprite &sprite : Queue()) {
    const float width =
        static_cast<float>(storage->GetTextureWidth(sprite.textureIndex));
    const float height =
        static_cast<float>(storage->GetTextureHeight(sprite.textureIndex));
    if (width <= 0.0f || height <= 0.0f || sprite.entry.a <= 0.0f) {
      continue;
    }

    const float scale = sprite.entry.scale;
    const float left = sprite.entry.x * screenWidth - width * scale * 0.5f;
    const float top = sprite.entry.y * screenHeight - height * scale * 0.5f;

    // SceneScreenUi::DrawTextureTopLeft と同じ規約（ローカル頂点は texture px 基準）
    Math::Transform uvTransform{};
    Math::Transform rectTransform{};
    rectTransform.translate = {left, top, 0.0f};
    rectTransform.scale = {scale, scale, 1.0f};

    const Math::Vector4 color{sprite.entry.r, sprite.entry.g, sprite.entry.b,
                              sprite.entry.a};
    engine->DrawSpriteRect(sprite.textureIndex, color, uvTransform,
                           rectTransform, Math::Vector2{width, height},
                           Math::Vector2{0.0f, 0.0f});
  }
}

} // namespace Atrum::Script::UiSprite
