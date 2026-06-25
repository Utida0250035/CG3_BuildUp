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

	if (ImGui::GetIO().WantCaptureMouse) return;

	// 1. マウスホイールで距離調整
	distance_ += (float)input_->GetMouseWheel() * 3.0f;

	distance_ = std::max(distance_, 1.0f);

	if (input_->IsMousePress(Mouse::Middle))
	{
		Vector2 move = input_->GetCursorDelta() * 0.01f;

		Quaternion yawQ =
			Quaternion::FromAxisAngle({ 0,1,0 }, move.x);

		Vector3 right =
			quaternion_.rotate_vector({ 1,0,0 });

		Quaternion pitchQ =
			Quaternion::FromAxisAngle(right, move.y);

		quaternion_ =
			(pitchQ * yawQ * quaternion_).normalized();
	
	}

	// 3. 原点から「見て」カメラ位置を算出
	// {0,0,distance_} をピボット回転させることでカメラの「位置」が決まる
	Vector3 offset = { 0.0f, 0.0f, distance_ };
	translate_ = quaternion_.rotate_vector(offset);

	assert(std::abs(VectorLength(translate_) - distance_) < 0.01f);

	Vector3 up =
		quaternion_.rotate_vector({ 0,1,0 });

	// 4. View行列の構築 (LookAtを使用)
	// カメラ位置(translate_)から原点(0,0,0)を見る行列を作成
	viewMatrix_ = MakeLookAtMatrix(translate_, { 0.0f, 0.0f, 0.0f }, up);

	viewMatrix_ = MakeIdentity4x4();

}