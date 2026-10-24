#include "Input/AbstractInput.h"
#include "Input/PlayInput.h"
#include "System/DeltaTime.h"

namespace {

namespace I = Atrum::Input;

}

namespace Atrum {

void AbstractInput::Initialize() { input_ = I::PlayInput::Instance(); }

void AbstractInput::Update() {

  data_ = {};
  leftStickInput_ = {};

  // コントローラー未実装の間はキーボード入力を使う
  isKeyboardInput_ = true;

  if (isKeyboardInput_) {

    // キーボード入力の抽象データ化

    if (input_->IsKeyTrigger(I::Key::SPACE)) {
      data_.isSubmitTrigger = true;
    }

    if (input_->IsKeyTrigger(I::Key::ESCAPE)) {
      data_.isPauseTrigger = true;
    }

    if (input_->IsKeyTrigger(I::Key::SPACE)) {
      data_.isJumpTrigger = true;
    }

    if (input_->IsKeyPress(I::Key::A) || input_->IsKeyPress(I::Key::LEFT)) {
      data_.vector.x -= 1.0f;
    }

    if (input_->IsKeyPress(I::Key::D) || input_->IsKeyPress(I::Key::RIGHT)) {
      data_.vector.x += 1.0f;
    }

    if (input_->IsKeyPress(I::Key::S) || input_->IsKeyPress(I::Key::DOWN)) {
      data_.vector.y -= 1.0f;
    }

    if (input_->IsKeyPress(I::Key::W) || input_->IsKeyPress(I::Key::UP)) {
      data_.vector.y += 1.0f;
    }

  } else {

    // コントローラー入力の抽象データ化
  }

  if (spacedVectorTimer_ <= 0.0f) {

    if (data_.vector.LengthSquare() > 1e-6f) {

      spacedVectorTimer_ = spacedVectorTime_;

      data_.timeSpacedVector = data_.vector;
    }
  }

  if (spacedVectorTimer_ > 0.0f) {

    spacedVectorTimer_ -= FrameDeltaTime::Instance()->GetDeltaTime();

    if (spacedVectorTimer_ <= 0.0f) {

      spacedVectorTimer_ = 0.0f;
    }
  }
}

} // namespace Atrum