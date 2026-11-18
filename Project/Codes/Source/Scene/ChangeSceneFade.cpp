#include "Scene/ChangeSceneFade.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Platform/Window/Window.h"
#include "Math/Easing.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/DeltaTime.h"
#include "System/PlaySe.h"

namespace Atrum {

namespace {

constexpr float kFadeOutSeconds = 0.5f;
/// 暗転完了後、シーン入替えまで画面を真っ黒で留める（中間フレームでの EnterScene を避ける）
constexpr float kFullBlackHoldBeforeSceneChangeSeconds = 0.08f;
constexpr float kFadeInDelaySeconds = 0.25f;
constexpr float kFadeInSeconds = 0.25f;

} // namespace

ChangeSceneFade::ChangeSceneFade() {

	timeRatio_ = 0.0f;
	waitCount_ = 0.0f;
	isSeOpenTrigger_ = false;

	textureIndex_ = AtrumEngine::Instance()->GetTexture("./Assets/Images/white4x4.png");
}

void ChangeSceneFade::Update() {

	if (!isProcess_) {

		isProcess_ = true;
	}

	if (isSceneChange_) {

		waitCount_ += ::Atrum::EngineFrameDeltaTime()->GetDeltaTime();

		if (waitCount_ >= kFadeInDelaySeconds) {

			if (!isSeOpenTrigger_) {
				isSeOpenTrigger_ = true;
				PlaySceneTransitionSe();
			}

			waitCount_ = kFadeInDelaySeconds;

			timeRatio_ -= 1.0f / kFadeInSeconds *
			              ::Atrum::EngineFrameDeltaTime()->GetDeltaTime();

			if (timeRatio_ <= 0.0f) {

				isProcess_ = false;
			}
		}

	} else {

		const float deltaTime = ::Atrum::EngineFrameDeltaTime()->GetDeltaTime();
		timeRatio_ += 1.0f / kFadeOutSeconds * deltaTime;

		if (timeRatio_ >= 1.0f) {
			timeRatio_ = 1.0f;
			waitCount_ += deltaTime;
			if (waitCount_ >= kFullBlackHoldBeforeSceneChangeSeconds) {
				isSceneChange_ = true;
				waitCount_ = 0.0f;
			}
		}
	}
}

void ChangeSceneFade::Draw() {

	AtrumEngine* engine = AtrumEngine::Instance();

	// アルファでフェードするため、この描画だけ通常合成にする
	engine->SetBlendModeForFrame(BlendMode::NORMAL);
	engine->PrepareSprite();

	Math::Transform uvTransform{};

	// ローカル頂点は1px。拡縮でクライアント全面を覆う
	Math::Transform rectTransform{};
	rectTransform.scale = {
		static_cast<float>(Window::kDefaultClientWidth),
		static_cast<float>(Window::kDefaultClientHeight),
		1.0f
	};

	const Math::Vector4 color{
		0.0f,
		0.0f,
		0.0f,
		Math::Ease::InCirc(timeRatio_)
	};

	engine->DrawSpriteRect(
		textureIndex_,
		color,
		uvTransform,
		rectTransform,
		Math::Vector2{1.0f, 1.0f},
		Math::Vector2{0.0f, 0.0f}
	);
}

} // namespace Atrum