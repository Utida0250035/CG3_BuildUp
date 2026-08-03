#include "Engine/Alias/DebugAlias.h"

#include "ForDebug/Log.h"
#include "ForDebug/DebugCamera.h"
#include "Input/PlayInput.h"
#include "Cast/StaticCast.h"

#ifdef USE_IMGUI

#include "ForDebug/ImGui.h"


#endif // USE_IMGUI

#include <format>
#include <numbers>

namespace Atrum::Debug {

	void DebugCamera::Initialize() {

#ifdef _DEBUG

		input_ = I::PlayInput::GetInstance();

#endif

	}

	void DebugCamera::Update() {

#ifdef USE_IMGUI

		if (ImGui::GetIO().WantCaptureMouse) {

			UpdateMatrix();

			return;

		}

#endif

#ifdef _DEBUG

		if (input_->IsKeyTrigger(I::Key::F5)) {

			if (mode_ == DebugCameraMode::FREE) {

				mode_ = DebugCameraMode::ORBIT;

				distance_ = (pivot_ - translate_).Length();

				quaternion_ = M::Quaternion::FromLhLookAt(pivot_, translate_, M::Vector3::Up());

				rotate_ = quaternion_.ToEulerFirstPerson();

				pivotQuaternion_ = quaternion_.Conjugated();

			} else {

				mode_ = DebugCameraMode::FREE;

			}

		}

		const M::Vector2 bufferedCursorMove = input_->GetCursorDelta() * 0.01562f;

		if (mode_ == DebugCameraMode::ORBIT) {

			if (input_->GetMouseWheel() != 0) {

				distance_ += Cast::Float(input_->GetMouseWheel()) * -3.0f;
				distance_ = std::max(distance_, 1.0f);

			}

			if (input_->IsMousePress(I::Mouse::Right)) {

				rotate_.y -= bufferedCursorMove.x;

				if (rotate_.y >= std::numbers::pi_v<float> *2.0f || rotate_.y <= 0.0f) {

					rotate_.y = fmodf(rotate_.y, std::numbers::pi_v<float> *2.0f);

				}

				rotate_.x -= bufferedCursorMove.y;

				if (rotate_.x >= std::numbers::pi_v<float> *2.0f || rotate_.x <= 0.0f) {

					rotate_.x = fmodf(rotate_.x, std::numbers::pi_v<float> *2.0f);

				}

				M::Quaternion yawQ = M::Quaternion::FromAxisAngle(M::Vector3::Up(), rotate_.y);
				M::Quaternion pitchQ = M::Quaternion::FromAxisAngle(M::Vector3::Right(), rotate_.x);
				quaternion_ = (pitchQ * yawQ).Normalized();

				pivotQuaternion_ = quaternion_.Conjugated();

			}

			M::Vector3 offset = { 0.0f, 0.0f, -distance_ };
			translate_ = pivotQuaternion_.RotateVector(offset) + pivot_;

		} else {

			if (input_->IsMousePress(I::Mouse::Right)) {

				rotate_.y -= bufferedCursorMove.x;

				M::Quaternion yawQ = M::Quaternion::FromAxisAngle(M::Vector3::Up(), rotate_.y);

				rotate_.x -= bufferedCursorMove.y;
				rotate_.x = std::clamp(rotate_.x, std::numbers::pi_v<float> *-0.5f, std::numbers::pi_v<float> *0.5f);

				M::Quaternion pitchQ = M::Quaternion::FromAxisAngle(M::Vector3::Right(), rotate_.x);

				quaternion_ = (pitchQ * yawQ).Normalized();


			} else {

				if (input_->GetMouseWheel() != 0) {

					M::Vector3 moveByWheel = Cast::Float(input_->GetMouseWheel()) * -3.0f * quaternion_.RotateVector(M::Vector3::Forward());

					moveByWheel.z *= -1.0f;

					translate_ += moveByWheel;

				}

			}

			if (input_->IsMousePress(I::Mouse::Middle)) {

				M::Vector3 moveByDrag = { -bufferedCursorMove.x, bufferedCursorMove.y, 0.0f };

				if (moveByDrag.LengthSquare() > 0.0f) {

					moveByDrag = quaternion_.RotateVector(moveByDrag);
					moveByDrag.z *= -1.0f;

					translate_ += moveByDrag;

				}

			}

		}

		UpdateMatrix();

#endif

	}

}