#include "DebugCamera.h"
#include "PlayInput.h"
#include "StaticCast.h"

#ifdef USE_IMGUI

#include "ImGui.h"


#endif // USE_IMGUI

void DebugCamera::Initialize() {

	input_ = PlayInput::GetInstance();

}

void DebugCamera::Update() {

	if (ImGui::GetIO().WantCaptureMouse) {

		UpdateMatrix();

		return;

	}

	if (input_->IsKeyTrigger(Key::F5)) {

		if (mode_ == DebugCameraMode::FREE) {

			mode_ = DebugCameraMode::ORBIT;

			distance_ = VectorLength(pivot_ - translate_);

			quaternion_ = Quaternion::FromLookAt(pivot_, translate_, Vector3{ 0.0f, 1.0f, 0.0f });

			pivotQuaternion_ = quaternion_.conjugated();

			Vector3 offset = { 0.0f, 0.0f, distance_ };
			translate_ = pivotQuaternion_.rotate_vector(offset) + pivot_;

		} else {

			mode_ = DebugCameraMode::FREE;

		}

	}

	const Vector2 bufferedCursorMove = input_->GetCursorDelta() * 0.01562f;

	if (mode_ == DebugCameraMode::ORBIT) {

		if (input_->GetMouseWheel() != 0) {

			distance_ += cast::Float(input_->GetMouseWheel()) * -3.0f;
			distance_ = std::max(distance_, 1.0f);

		}

		if (input_->IsMousePress(Mouse::Middle)) {
			Quaternion yawQ = Quaternion::FromAxisAngle({ 0.0f, 1.0f, 0.0f }, -bufferedCursorMove.x);
			Quaternion pitchQ = Quaternion::FromAxisAngle({ 1.0f, 0.0f, 0.0f }, -bufferedCursorMove.y);
			quaternion_ = (pitchQ * quaternion_ * yawQ).normalized();
			pivotQuaternion_ = quaternion_.conjugated();

		}

		Vector3 offset = { 0.0f, 0.0f, -distance_ };
		translate_ = pivotQuaternion_.rotate_vector(offset) + pivot_;

	} else {

		if (input_->IsMousePress(Mouse::Middle)) {

			Quaternion yawQ = Quaternion::FromAxisAngle({ 0,1,0 }, bufferedCursorMove.x);

			Vector3 right = quaternion_.rotate_vector({ 1, 0, 0 });
			Quaternion pitchQ = Quaternion::FromAxisAngle(right, bufferedCursorMove.y);

			quaternion_ = (yawQ * quaternion_ * pitchQ).normalized();
			quaternion_.z = 0.0f;
			quaternion_.normalize();

		} else {

			if (input_->GetMouseWheel() != 0) {

				Vector3 moveByWheel = cast::Float(input_->GetMouseWheel()) * -3.0f * quaternion_.rotate_vector({ 0.0f, 0.0f, 1.0f });

				moveByWheel.z *= -1.0f;

				translate_ += moveByWheel;

			}

		}

		if (input_->IsMousePress(Mouse::Left)) {

			Vector3 moveByDrag = { -bufferedCursorMove.x, bufferedCursorMove.y, 0.0f };
			moveByDrag = quaternion_.rotate_vector(moveByDrag);

			translate_ += moveByDrag;

		}

	}

	UpdateMatrix();

}