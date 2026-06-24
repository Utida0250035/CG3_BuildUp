#include "DebugCamera.h"
#include "PlayInput.h"
#include "StaticCast.h"

void DebugCamera::Initialize() {

	input_ = PlayInput::GetInstance();

}

void DebugCamera::Update() {

	float speed = cast::Float(input_->GetMouseWheel()) * 0.25f;

	Matrix4x4 rotateMatrix = quaternion_.create_rotate_matrix();

	if (speed != 0.0f) {

		Vector3 move = { 0, 0, speed };

		move = VectorTransform(move, rotateMatrix);

		translate_ += move;

	}

	if (input_->IsMousePress(Mouse::Left)) {

		const Vector2 move2d = input_->GetCursorDelta() * 0.03125f;

		Vector3 move = { move2d.x, move2d.y, 0.0f };

		move = VectorTransform(move, rotateMatrix);

		translate_ += move;

	} else if(input_->IsMousePress(Mouse::Middle)) {

		const Vector2 move2d = input_->GetCursorDelta() * 0.01562f;

		quaternion_ *= Quaternion::FromEulerRotateVector(Vector3{ move2d.x, move2d.y, 0.0f });

	}

	UpdateMatrix();

	if (input_->IsMousePress(Mouse::Right)) {

		viewMatrix_ = MakeLookAtMatrix(translate_, { 0.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f});

	}

}