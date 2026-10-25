#include "Scene/SceneScreenUi.h"

#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Blend/BlendMode.h"
#include "Engine/Platform/Window/Window.h"
#include "Engine/Resource/TextureStorage.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"

#include <filesystem>
#include <string>

namespace Atrum::SceneScreenUi {

namespace {

struct UiTextures {
  uint32_t title = UINT32_MAX;
  uint32_t howToNext = UINT32_MAX;
  uint32_t clear = UINT32_MAX;
};

UiTextures uiTextures_{};

std::string ResolveExistingImagePath(
    const std::initializer_list<const char *> fileNames) {
  for (const char *fileName : fileNames) {
    if (fileName == nullptr) {
      continue;
    }

    const std::string assetPath =
        std::string{"./Assets/Images/"} + fileName;
    if (std::filesystem::exists(assetPath)) {
      return assetPath;
    }

    const std::string resourcePath =
        std::string{"./Resources/"} + fileName;
    if (std::filesystem::exists(resourcePath)) {
      return resourcePath;
    }
  }

  return {};
}

uint32_t LoadTextureFromFileNames(
    const std::initializer_list<const char *> fileNames) {
  const std::string resolvedPath = ResolveExistingImagePath(fileNames);
  if (resolvedPath.empty()) {
    return UINT32_MAX;
  }

  AtrumEngine *engine = AtrumEngine::Instance();
  if (engine == nullptr) {
    return UINT32_MAX;
  }

  return engine->GetTexture(resolvedPath);
}

float GetTextureWidth(const uint32_t textureIndex) {
  if (textureIndex == UINT32_MAX) {
    return 0.0f;
  }

  return static_cast<float>(
      TextureStorage::Instance()->GetTextureWidth(textureIndex));
}

float GetTextureHeight(const uint32_t textureIndex) {
  if (textureIndex == UINT32_MAX) {
    return 0.0f;
  }

  return static_cast<float>(
      TextureStorage::Instance()->GetTextureHeight(textureIndex));
}

void DrawTextureTopLeft(AtrumEngine *engine, const uint32_t textureIndex,
                        const float left, const float top,
                        const Math::Vector4 &color = {1.0f, 1.0f, 1.0f, 1.0f},
                        const float displayScale = 1.0f) {
  if (engine == nullptr || textureIndex == UINT32_MAX) {
    return;
  }

  const float width = GetTextureWidth(textureIndex);
  const float height = GetTextureHeight(textureIndex);
  if (width <= 0.0f || height <= 0.0f) {
    return;
  }

  // ローカル頂点はテクスチャ px 基準（0〜width/height）、scale は 1（BattleHud / Fade と同系）
  Math::Transform uvTransform{};
  Math::Transform rectTransform{};
  rectTransform.translate = {left, top, 0.0f};
  rectTransform.scale = {displayScale, displayScale, 1.0f};

  engine->DrawSpriteRect(textureIndex, color, uvTransform, rectTransform,
                         Math::Vector2{width, height},
                         Math::Vector2{0.0f, 0.0f});
}

void DrawTextureCenter(AtrumEngine *engine, const uint32_t textureIndex,
                       const float centerX, const float centerY,
                       const Math::Vector4 &color = {1.0f, 1.0f, 1.0f, 1.0f},
                       const float displayScale = 1.0f) {
  const float width = GetTextureWidth(textureIndex) * displayScale;
  const float height = GetTextureHeight(textureIndex) * displayScale;
  DrawTextureTopLeft(engine, textureIndex, centerX - width * 0.5f,
                     centerY - height * 0.5f, color, displayScale);
}

/// 手前の雲帯と被りにくいよう下余白を多めに取る
constexpr float kHowToNextBottomMargin = 112.0f;

float ComputeScreenAnchoredCenterX(const float screenCenterX,
                                   const uint32_t textureIndex,
                                   const float textCenterRatioX,
                                   const float displayScale) {
  if (textureIndex == UINT32_MAX) {
    return screenCenterX;
  }

  const float displayWidth = GetTextureWidth(textureIndex) * displayScale;
  return screenCenterX + (0.5f - textCenterRatioX) * displayWidth;
}

float ComputeHowToNextCenterY(const float screenHeight,
                              const uint32_t howToTexture,
                              const float displayScale = 1.0f) {
  const float howToHeight = GetTextureHeight(howToTexture) * displayScale;
  return screenHeight - howToHeight * 0.5f - kHowToNextBottomMargin;
}

} // namespace

void PreloadTitleUiTextures() {
  uiTextures_.title = LoadTextureFromFileNames({"title.png"});
  uiTextures_.howToNext =
      LoadTextureFromFileNames({"howToNext.png", "howtoNext.png"});
}

void PreloadResultUiTextures() {
  uiTextures_.clear = LoadTextureFromFileNames({"clear.png"});
  uiTextures_.howToNext =
      LoadTextureFromFileNames({"howToNext.png", "howtoNext.png"});
}

void PreloadGameIntroUiTextures() {
  uiTextures_.howToNext =
      LoadTextureFromFileNames({"howToNext.png", "howtoNext.png"});
}

void PrepareDraw(AtrumEngine *engine, Camera *camera) {
  if (engine == nullptr || camera == nullptr) {
    return;
  }

  engine->SetCamera(camera);
  engine->SetBlendModeForFrame(BlendMode::NORMAL);
  engine->PrepareSprite();
}

void DrawTitleSceneUi(AtrumEngine *engine, Camera *camera) {
  PrepareDraw(engine, camera);

  const float screenWidth =
      static_cast<float>(Window::kDefaultClientWidth);
  const float screenHeight =
      static_cast<float>(Window::kDefaultClientHeight);

  DrawTextureCenter(engine, uiTextures_.title, screenWidth * 0.5f,
                    screenHeight * 0.38f);

  DrawTextureCenter(engine, uiTextures_.howToNext, screenWidth * 0.5f,
                    ComputeHowToNextCenterY(screenHeight, uiTextures_.howToNext));
}

void DrawResultSceneUi(AtrumEngine *engine, Camera *camera) {
  PrepareDraw(engine, camera);

  const float screenWidth =
      static_cast<float>(Window::kDefaultClientWidth);
  const float screenHeight =
      static_cast<float>(Window::kDefaultClientHeight);

  constexpr float kClearDisplayScale = 2.0f;
  // clear.png は文字が左寄り＋右に余白があるため、画像中心ではなく文字中心を画面中央へ
  constexpr float kClearTextCenterRatioX = 0.24f;
  const float clearDisplayWidth =
      GetTextureWidth(uiTextures_.clear) * kClearDisplayScale;
  const float clearDrawCenterX =
      screenWidth * 0.5f +
      (0.5f - kClearTextCenterRatioX) * clearDisplayWidth;

  DrawTextureCenter(engine, uiTextures_.clear, clearDrawCenterX,
                    screenHeight * 0.4f, {1.0f, 1.0f, 1.0f, 1.0f},
                    kClearDisplayScale);

  constexpr float kHowToNextTextCenterRatioX = 0.30f;
  const float howToDrawCenterX = ComputeScreenAnchoredCenterX(
      screenWidth * 0.5f, uiTextures_.howToNext, kHowToNextTextCenterRatioX,
      1.0f);

  DrawTextureCenter(engine, uiTextures_.howToNext, howToDrawCenterX,
                    ComputeHowToNextCenterY(screenHeight, uiTextures_.howToNext));

}

void DrawGameIntroHowToNextUi(AtrumEngine *engine, Camera *camera) {
  if (uiTextures_.howToNext == UINT32_MAX) {
    return;
  }

  PrepareDraw(engine, camera);

  constexpr float kMargin = 16.0f;
  const float screenWidth =
      static_cast<float>(Window::kDefaultClientWidth);
  const float width = GetTextureWidth(uiTextures_.howToNext);

  DrawTextureTopLeft(engine, uiTextures_.howToNext,
                     screenWidth - width - kMargin, kMargin);
}

} // namespace Atrum::SceneScreenUi
