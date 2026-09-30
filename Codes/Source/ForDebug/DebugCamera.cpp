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

#ifdef DEVELOPMENT

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

#ifdef DEVELOPMENT

		if (input_->IsKeyTrigger(I::Key::F5)) {

			if (mode_ == DebugCameraMode::FREE) {

				mode_ = DebugCameraMode::ORBIT;

				distance_ = (pivot_ - translate_).Length();

				quaternion_ = M::Quaternion::FromLhLookAt(pivot_, translate_, M::Vector3::UpLh());

				quaternion_.LhZRemove();

			} else {

				mode_ = DebugCameraMode::FREE;

			}

		}

		const M::Vector2 bufferedCursorMove = input_->GetCursorDelta() * 0.01562f;

		if (input_->IsMousePress(I::Mouse::Right)) {

			deltaRotate_.y = bufferedCursorMove.x;

			deltaRotate_.x = bufferedCursorMove.y;

			M::Quaternion yawQ = M::Quaternion::FromAxisAngle(M::Vector3::UpLh(), deltaRotate_.y);
			M::Quaternion pitchQ = M::Quaternion::FromAxisAngle(quaternion_.RotateVector(M::Vector3::RightLh()).Normalized(), deltaRotate_.x);

			M::Vector3 newForward = (pitchQ * yawQ * quaternion_).RotateVector(M::Vector3::ForwardLh());

			quaternion_ = M::Quaternion::Slerp(M::Quaternion::Identity(), pitchQ, 1.0f - std::abs(newForward.y)) * yawQ * quaternion_;

			quaternion_.Normalize();

			quaternion_.LhZRemove();

		}

		if (mode_ == DebugCameraMode::ORBIT) {

			if (input_->GetMouseWheel() != 0) {

				distance_ += Cast::Float(input_->GetMouseWheel()) * -3.0f;
				distance_ = std::max(distance_, 1.0f);

			}

			M::Vector3 offset = { 0.0f, 0.0f, -distance_ };
			translate_ = quaternion_.RotateVector(offset) + pivot_;

		} else {

			if (!input_->IsMousePress(I::Mouse::Right)) {

				if (input_->GetMouseWheel() != 0) {

					M::Vector3 moveByWheel = Cast::Float(input_->GetMouseWheel()) * 3.0f * quaternion_.RotateVector(M::Vector3::ForwardLh());

					translate_ += moveByWheel;

				}

			}

			if (input_->IsMousePress(I::Mouse::Middle)) {

				M::Vector3 moveByDrag = { -bufferedCursorMove.x, bufferedCursorMove.y, 0.0f };

				if (moveByDrag.LengthSquare() > 0.0f) {

					moveByDrag = quaternion_.RotateVector(moveByDrag);

					translate_ += moveByDrag;

				}

			}

		}

#endif

		UpdateMatrix();

	}

}