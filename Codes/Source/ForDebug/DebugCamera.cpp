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

				deltaRotate_ = quaternion_.ToEulerXYZ();

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

				deltaRotate_.y = bufferedCursorMove.x;

				deltaRotate_.x = bufferedCursorMove.y;

				M::Quaternion yawQ = M::Quaternion::FromAxisAngle(M::Vector3::Up(), deltaRotate_.y);
				M::Quaternion pitchQ = M::Quaternion::FromAxisAngle(quaternion_.RotateVector(M::Vector3::Right()), deltaRotate_.x);
				quaternion_ = (pitchQ * yawQ * quaternion_).Normalized();

			}

			M::Vector3 offset = { 0.0f, 0.0f, -distance_ };
			translate_ = quaternion_.RotateVector(offset) + pivot_;

		} else {

			if (input_->IsMousePress(I::Mouse::Right)) {

				deltaRotate_.y = bufferedCursorMove.x;

				M::Quaternion yawQ = M::Quaternion::FromAxisAngle(M::Vector3::Up(), deltaRotate_.y);

				deltaRotate_.x = bufferedCursorMove.y;

				M::Quaternion pitchQ = M::Quaternion::FromAxisAngle(quaternion_.RotateVector(M::Vector3::Right()), deltaRotate_.x);

				quaternion_ = (pitchQ * yawQ * quaternion_).Normalized();


			} else {

				if (input_->GetMouseWheel() != 0) {

					M::Vector3 moveByWheel = Cast::Float(input_->GetMouseWheel()) * 3.0f * quaternion_.RotateVector(M::Vector3::Forward());

					translate_ += moveByWheel;

				}

			}

			if (input_->IsMousePress(I::Mouse::Middle)) {

				M::Vector3 moveByDrag = { bufferedCursorMove.x, bufferedCursorMove.y, 0.0f };

				if (moveByDrag.LengthSquare() > 0.0f) {

					moveByDrag = quaternion_.RotateVector(moveByDrag);

					translate_ += moveByDrag;

				}

			}

		}

		UpdateMatrix();

#endif

	}

}