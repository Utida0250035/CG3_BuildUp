#include "AtrumEngine.h"
#include "Audio.h"
#include "Bezier.h"
#include "Collision.h"
#include "DeltaTime.h"
#include "Log.h"
#include "OBB.h"
#include "DirectInput.h"
#include "HitMesh.h"
#include "PyramidMesh.h"
#include "CollisionMeshToTriangle.h"
#include "PlayInput.h"
#include "StaticCast.h"
#include "DebugCamera.h"
#include <memory>
#include <numbers>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	LeakChecker leakChecker;

	// エンジンインスタンスの取得
	AtrumEngine* atrum = AtrumEngine::GetInstance();

	// エンジンの初期化
	atrum->Initialize("CG2", 1280, 720);

	// fps設定
	atrum->SetFps(60);

	// 乱数シード生成
	srand(static_cast<unsigned int>(time(nullptr)));

	/* プレイヤー入力 */

	PlayInput* playInput = PlayInput::GetInstance();

	/* テクスチャ */

	const char* textureFilePaths[3] = {
		"./Resources/Images/ForStudy/uvChecker.png",
		"./Resources/Images/ForStudy/monsterBall.png",
		"./Resources/Images/white4x4.png"
	};

	uint32_t textureWhite = atrum->GetTexture(textureFilePaths[2]);

	/* 音源 */

	std::unique_ptr<Audio> audio = std::make_unique<Audio>();
	audio->Initialize();

	[[maybe_unused]] size_t seAlarm = audio->LoadSe("./Resources/Audios/Alarm01.wav");

	size_t seCat = audio->LoadSe("./Resources/Audios/seCat.mp3");


	/* Triangle */

	Transform triangleTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{std::numbers::pi_v<float> * 0.5f, 0.0f, 0.0f}, Vector3{0.0f, -1.0f, 0.0f} };

	Vector4 triangleColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	Transform triangleUvTransform{};

	Triangle triangle{
		{-5.0f, -5.0f, 0.0f},
		{0.0f, 5.0f, 0.0f},
		{5.0f, -5.0f, 0.0f},
		{0.0f, 0.0f, -1.0f}
	};

	std::array<VertexData, 3> triangleVertexData = {
		Vector4{triangle.v0.x, triangle.v0.y, triangle.v0.z, 1.0f},Vector2{0.0f, 1.0f}, triangle.normal,
		Vector4{triangle.v1.x, triangle.v1.y, triangle.v1.z, 1.0f},Vector2{0.5f, 0.0f}, triangle.normal,
		Vector4{triangle.v2.x, triangle.v2.y, triangle.v2.z, 1.0f},Vector2{1.0f, 1.0f}, triangle.normal
	};

	uint32_t triangleTexture = textureWhite;


	/* HitMesh */

	PyramidMesh pyramidMesh{};
	HitMesh hitMeshPyramid = CreateAsymmetricPyramid();
	hitMeshPyramid.position = { 0.0f, 10.0f, 0.0f };
	hitMeshPyramid.velocity = { 0.0f, -0.1f, 0.0f };
	hitMeshPyramid.rotation = { 0.0f, 0.0f, 0.0f };
	hitMeshPyramid.angularVelocity = {};

	/* DirectionalLight */

	DirectionalLightData directionalLightData = DirectionalLightData{
		.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
		.direction = Vector3{0.0f, -1.0f, 0.0f},
		.intensity = 10.0f,
		.lightModel = LightModel::HalfLambert
	};

	bool isLightingEnable = true;

	/* deltaTime */

	std::unique_ptr<DeltaTime> deltaTimeCalc = std::make_unique<DeltaTime>();
	float deltaTime = 0.0f;

	/* タイムカウント */

	float timeCount = 0.0f;

	/* 乱数初期化 */

	srand(static_cast<unsigned int>(time(nullptr)));

	/* デバッグカメラ */

	std::unique_ptr<DebugCamera> debugCamera = std::make_unique<DebugCamera>();

	debugCamera->Initialize();
	debugCamera->CreateOrthographicMatrix(1280, 720);

	/* ImGui */

#ifdef USE_IMGUI

	ImVec2 imguiChildSize = ImVec2(0.0f, 0.0f);
	ImGuiChildFlags imguiChildFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

#endif

	while (atrum->Process()) {
		// ウィンドウの×ボタンが押されるまでループ

		/*============== メインループ =================*/

#ifdef USE_IMGUI

		// ImGuiにフレーム開始を通知
		atrum->ImGuiNewFrame();

#endif

		///
		/// ↓ 更新ここから
		///

		deltaTimeCalc->CalcDeltaTime();
		deltaTime = deltaTimeCalc->GetDeltaTime();

		timeCount += deltaTime;

		if (playInput->IsKeyTrigger(Key::SPACE)) {

			audio->PlaySe(seCat);

		}

		Vector2 cursorPos = playInput->GetCursorPos();

		Vector2 cursorDelta = playInput->GetCursorDelta();

		[[maybe_unused]] int32_t mouseWheel = playInput->GetMouseWheel();

#ifdef USE_IMGUI

		ImGui::Begin("debug");

		ImGui::Text("timeCount: %f", timeCount);

		ImGui::DragInt("mouseWheel", &mouseWheel);

		ImGui::DragFloat2("cursorPos", &cursorPos.x);

		ImGui::DragFloat2("cursorDelta", &cursorDelta.x);

		ImGui::SmallButton("seCat");

		if (ImGui::IsItemActivated()) {

			audio->PlaySe(seCat);

		}

		ImGui::SmallButton("seAlarm");

		if (ImGui::IsItemActivated()) {

			audio->PlaySe(seAlarm);

		}

		ImGui::End();

		ImGui::Begin("DebugCamera");

		ImGui::Text("mode: %d", debugCamera->GetMode());

		ImGui::DragFloat("distance", &debugCamera->RefDistance());

		ImGui::DragFloat3("translate", &debugCamera->RefTranslate().x);

		ImGui::DragFloat4("quaternion", &debugCamera->RefQuaternion().x, 0.03125f);

		ImGui::DragFloat4("pivotQuaternion", &debugCamera->RefPivotQuaternion().x, 0.03125f);

		ImGui::DragFloat3("pivot", &debugCamera->RefPivot().x, 0.03125f);

		const Vector3& pivot = debugCamera->RefPivot();
		const Vector3& translate = debugCamera->RefTranslate();

		Vector3 direction = VectorNormalize(pivot - translate);
		Vector3 calculatedDirection = debugCamera->RefQuaternion().rotate_vector({ 0.0f, 0.0f, -1.0f });

		ImGui::DragFloat3("direction", &direction.x);
		ImGui::DragFloat3("direction(calc)", &calculatedDirection.x);

		if (ImGui::IsItemActive()) {

			debugCamera->RefQuaternion().normalize();

		}

		if (ImGui::IsItemActive()) {

			debugCamera->RefPivotQuaternion().normalize();

		}

		ImGui::End();


		/* 光源GUI */

		ImGui::Begin("directionalLight");

		ImGui::DragFloat4("color", &directionalLightData.color.x, 0.03125f);

		ImGui::DragFloat3("direction", &directionalLightData.direction.x, 0.03125f);

		if (ImGui::IsItemActive()) {

			directionalLightData.direction = VectorNormalize(directionalLightData.direction);

		}

		ImGui::DragFloat("intensity", &directionalLightData.intensity, 0.03125f);

		ImGui::Checkbox("isLightingEnable", &isLightingEnable);

		if (isLightingEnable) {

			int lightMode = static_cast<int>(directionalLightData.lightModel);

			ImGui::Selectable("lambert", lightMode == 0);
			if (ImGui::IsItemActivated()) {

				lightMode = 0;

			}

			ImGui::Selectable("halfLambert", lightMode == 1);
			if (ImGui::IsItemActivated()) {

				lightMode = 1;

			}

			directionalLightData.lightModel = static_cast<LightModel>(lightMode);

		}

		ImGui::End();

		/* HitMeshのGUI */

		ImGui::Begin("HitMesh");

		ImGui::SmallButton("reset");

		if (ImGui::IsItemActivated()) {

			hitMeshPyramid.position = { 0.0f, 1.0f, 0.0f };
			hitMeshPyramid.angularVelocity = {};
			hitMeshPyramid.rotation = {};

		}

		Vector3 buffer = hitMeshPyramid.position;

		ImGui::DragFloat3("pos", &buffer.x);

		hitMeshPyramid.position = buffer;

		ImGui::End();

		/* 三角形GUI */

		ImGui::Begin("triangle");


		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &triangleTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &triangleTransform.rotate.x, 0.03125f);

			ImGui::DragFloat3("translate", &triangleTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		Vector3 vertices[3]{};

		for (size_t i = 0; i < 3; ++i) {

			const auto& v = triangleVertexData[i];

			vertices[i] = { v.position.x, v.position.y, v.position.w };

		}

		triangle.v0 = VectorTransform(vertices[0], MakeWorldMatrix(triangleTransform));
		triangle.v1 = VectorTransform(vertices[1], MakeWorldMatrix(triangleTransform));
		triangle.v2 = VectorTransform(vertices[2], MakeWorldMatrix(triangleTransform));

		triangle.normal = VectorTransform(triangleVertexData[0].normal, MakeRotateMatrix(triangleTransform.rotate));

		for (size_t i = 0; i < 3; ++i) {

			ImGui::DragFloat3(("vertexPos" + std::to_string(i)).c_str(), &triangleVertexData[i].position.x, 0.03125f);
			ImGui::DragFloat2(("texCoord" + std::to_string(i)).c_str(), &triangleVertexData[i].texCoord.x, 0.03125f);

		}


		if (ImGui::BeginChild("uvTransform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("uvTransform");

			ImGui::DragFloat2("scale", &triangleUvTransform.scale.x, 0.03125f);
			ImGui::DragFloat("rotate", &triangleUvTransform.rotate.z, 0.03125f);
			ImGui::DragFloat2("translate", &triangleUvTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();


		if (ImGui::BeginChild("texture", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("texture");

			for (const auto& path : textureFilePaths) {

				ImGui::Selectable(path);

				if (ImGui::IsItemActivated()) {

					triangleTexture = atrum->GetTexture(path);

				}

			}

			ImGui::DragFloat4("color", &triangleColor.x, 0.03125f, 0.0f, 1.0f);

		}

		ImGui::EndChild();

		ImGui::End();

		atrum->ImGuiRender();

		debugCamera->Update();

#endif

		hitMeshPyramid.velocity.y += -5.0f * deltaTime;
		hitMeshPyramid.Update(deltaTime);

		ResolveCollision(hitMeshPyramid, { triangle });

		playInput->EndOfFrame();

		///
		/// ↑更新ここまで
		/// 

		///
		/// ↓描画ここから
		/// 

		atrum->SetViewMatrix(debugCamera->GetViewMatrix());

		// 光源データの設定
		atrum->SetDirectionalLightData(directionalLightData);

		// 描画処理(前)
		atrum->PreDraw();


		atrum->DrawTriangle(triangleTexture, triangleColor, triangleUvTransform, triangleTransform, triangleVertexData, isLightingEnable);

		atrum->DrawAsymmetricPyramid(textureWhite, Vec4Red(), Transform{}, Transform{ {1.0f, 1.0f, 1.0f}, hitMeshPyramid.rotation, hitMeshPyramid.position }, pyramidMesh, isLightingEnable);

		// Sprite準備
		atrum->PrepareSprite();


		// 描画処理(後)
		atrum->PostDraw();

		///
		/// ↑描画ここまで
		/// 

	}

	///
	/// ↓終了処理
	/// 

	atrum->Finalize();

	AtrumEngine::Destroy();

	return 0;

}