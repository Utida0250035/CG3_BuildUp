#include "Scene/ChangeSceneFade.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Platform/Window/Window.h"
#include "Math/Easing.h"
#include "Math/Transform.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"
#include "System/DeltaTime.h"

namespace Atrum {

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

		waitCount_ += FrameDeltaTime::Instance()->GetDeltaTime();

		if (waitCount_ >= 0.25f) {

			if (!isSeOpenTrigger_) {

				isSeOpenTrigger_ = true;
			}

			waitCount_ = 1.0f;

			timeRatio_ -= 1.0f / 0.25f * FrameDeltaTime::Instance()->GetDeltaTime();

			if (timeRatio_ <= 0.0f) {

				isProcess_ = false;
			}
		}

	} else {

		timeRatio_ += 1.0f / 0.5f * FrameDeltaTime::Instance()->GetDeltaTime();

		if (timeRatio_ >= 1.0f) {

			timeRatio_ = 1.0f;

			isSceneChange_ = true;
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