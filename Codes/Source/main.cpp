#include "AtrumEngine.h"
#include "Audio.h"
#include "Bezier.h"
#include "Collision.h"
#include "DeltaTime.h"
#include "Log.h"
#include "OBB.h"
#include "DirectInput.h"
#include "PlayInput.h"
#include "StaticCast.h"
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

	/* 3dカメラ */

	// カメラの座標情報
	Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -50.0f} };

	/* Triangle */

	Transform triangleTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.03f, 0.0f}, Vector3{} };

	Vector4 triangleColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	Transform triangleUvTransform{};

	std::array<VertexData, 3> triangleVertexData = {
		Vector4{-5.0f, -5.0f, 0.0f, 1.0f},Vector2{0.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{0.0f, 5.0f, 0.0f, 1.0f},Vector2{0.5f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{5.0f, -5.0f, 0.0f, 1.0f},Vector2{1.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f}
	};

	uint32_t triangleTexture = textureWhite;

	/* Triangle2 */

	Transform triangle2Transform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.1f, 0.0f}, Vector3{} };

	Vector4 triangle2Color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	Transform triangle2UvTransform{};

	std::array<VertexData, 3> triangle2VertexData = {
		Vector4{-5.0f, -5.0f, 0.0f, 1.0f},Vector2{0.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{0.0f, 5.0f, 0.0f, 1.0f},Vector2{0.5f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{5.0f, -5.0f, 0.0f, 1.0f},Vector2{1.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f}
	};

	uint32_t triangle2Texture = textureWhite;


	/* Sphere */

	Transform sphereTransform{};
	Transform sphereUvTransform{};
	float sphereRadius = 5.0f;
	uint32_t sphereSubdivision = 12;
	uint32_t sphereTexture = textureWhite;
	Vector4 sphereColor = Vec4White();


	/* 平面3dModel */

	auto planeModel = atrum->CreateModel("./Resources/Objects/ForStudy", "plane.obj", "./Resources/Objects/ForStudy", "plane.mtl");
	Transform planeModelTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };
	planeModelTransform.translate.z = -20.0f;

	/* 複数メッシュ3dModel */

	auto multiMeshModel = atrum->CreateModel("./Resources/Objects/ForStudy", "multiMesh.obj", "./Resources/Objects/ForStudy", "multiMesh.mtl");
	Transform multiMeshModelTransform{};

	/* 複数マテリアル3dModel */

	auto multiMtlModel = atrum->CreateModel("./Resources/Objects/ForStudy", "multiMaterial.obj", "./Resources/Objects/ForStudy", "multiMaterial.mtl");
	Transform multiMtlModelTransform{};

	/* Sprite */

	Transform spriteTransform{};
	Transform spriteUvTransform{};
	Vector2 spriteSize{ 64.0f, 64.0f };
	uint32_t spriteTexture = textureWhite;
	Vector4 spriteColor = Vec4White();


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

		/* カメラGUI */
		ImGui::Begin("camera");

		ImGui::DragFloat3("rotate", &cameraTransform.rotate.x, 0.03125f);
		ImGui::DragFloat3("translate", &cameraTransform.translate.x, 0.03125f);

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

		/* 三角形(1)GUI */

		ImGui::Begin("triangle");


		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &triangleTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &triangleTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &triangleTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

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


		/* 三角形(2)GUI */

		ImGui::Begin("triangle2");

		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &triangle2Transform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &triangle2Transform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &triangle2Transform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		for (size_t i = 0; i < 3; ++i) {

			ImGui::DragFloat3(("vertexPos" + std::to_string(i)).c_str(), &triangle2VertexData[i].position.x, 0.03125f);
			ImGui::DragFloat2(("texCoord" + std::to_string(i)).c_str(), &triangle2VertexData[i].texCoord.x, 0.03125f);

		}

		if (ImGui::BeginChild("uvTransform"), imguiChildSize, imguiChildFlags) {

			ImGui::Text("uvTransform");

			ImGui::DragFloat2("scale", &triangle2UvTransform.scale.x, 0.03125f);
			ImGui::DragFloat("rotate", &triangle2UvTransform.rotate.z, 0.03125f);
			ImGui::DragFloat2("translate", &triangle2UvTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		if (ImGui::BeginChild("texture", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("texture");

			for (const auto& path : textureFilePaths) {

				ImGui::Selectable(path);

				if (ImGui::IsItemActivated()) {

					triangle2Texture = atrum->GetTexture(path);

				}

			}

			ImGui::DragFloat4("color", &triangle2Color.x, 0.03125f, 0.0f, 1.0f);

		}

		ImGui::EndChild();

		ImGui::End();


		/* 球GUI */

		ImGui::Begin("sphere");

		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &sphereTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &sphereTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &sphereTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		if (ImGui::BeginChild("uvTransform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("uvTransform");

			ImGui::DragFloat2("scale", &sphereUvTransform.scale.x, 0.03125f);
			ImGui::DragFloat("rotate", &sphereUvTransform.rotate.x, 0.03125f);
			ImGui::DragFloat2("translate", &sphereUvTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		if (ImGui::BeginChild("texture", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("texture");

			for (const auto& path : textureFilePaths) {

				ImGui::Selectable(path);

				if (ImGui::IsItemActivated()) {

					sphereTexture = atrum->GetTexture(path);

				}

			}

			ImGui::DragFloat4("color", &sphereColor.x, 0.03125f);

		}

		ImGui::EndChild();

		ImGui::DragFloat("radius", &sphereRadius, 0.03125f);

		int subdivision = sphereSubdivision;

		ImGui::DragInt("subdivision", &subdivision, 0.03125f);

		sphereSubdivision = subdivision;

		ImGui::End();


		/* 平面3dモデルGUI */

		ImGui::Begin("planeModel");

		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &planeModelTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &planeModelTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &planeModelTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		ImGui::End();

		/* 複数メッシュ3dモデルGUI */

		ImGui::Begin("multiMeshModel");

		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &multiMeshModelTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &multiMeshModelTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &multiMeshModelTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		ImGui::End();

		/* 複数マテリアル3dモデルGUI */

		ImGui::Begin("multiMaterialModel");

		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &multiMtlModelTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &multiMtlModelTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &multiMtlModelTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		ImGui::Text(("texture: " + multiMtlModel->GetTexturePath()).c_str());

		ImGui::End();

		/* スプライトGUI */

		ImGui::Begin("sprite");

		if (ImGui::BeginChild("transform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &spriteTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &spriteTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &spriteTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		if (ImGui::BeginChild("uvTransform", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("uvTransform");

			ImGui::DragFloat2("scale", &spriteUvTransform.scale.x, 0.03125f);
			ImGui::DragFloat("rotate", &spriteUvTransform.rotate.x, 0.03125f);
			ImGui::DragFloat2("translate", &spriteUvTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		if (ImGui::BeginChild("texture", imguiChildSize, imguiChildFlags)) {

			ImGui::Text("texture");

			for (const auto& path : textureFilePaths) {

				ImGui::Selectable(path);

				if (ImGui::IsItemActivated()) {

					spriteTexture = atrum->GetTexture(path);

				}

			}

			ImGui::DragFloat4("color", &spriteColor.x, 0.03125f);

		}

		ImGui::EndChild();

		ImGui::End();


		atrum->ImGuiRender();

#endif

		///
		/// ↑更新ここまで
		/// 

		///
		/// ↓描画ここから
		/// 

		// 光源データの設定
		atrum->SetDirectionalLightData(directionalLightData);

		// 描画処理(前)
		atrum->PreDraw();

		atrum->DrawSphere(sphereTexture, sphereColor, sphereUvTransform, sphereTransform, cameraTransform, sphereRadius, sphereSubdivision, isLightingEnable);
		atrum->DrawTriangle(triangleTexture, triangleColor, triangleUvTransform, triangleTransform, cameraTransform, triangleVertexData, isLightingEnable);
		atrum->DrawTriangle(triangle2Texture, triangle2Color, triangle2UvTransform, triangle2Transform, cameraTransform, triangle2VertexData, isLightingEnable);

		atrum->DrawModel(planeModel.get(), planeModelTransform, cameraTransform, isLightingEnable);
		atrum->DrawModel(multiMeshModel.get(), multiMeshModelTransform, cameraTransform, isLightingEnable);
		atrum->DrawModel(multiMtlModel.get(), multiMeshModelTransform, cameraTransform, isLightingEnable);

		// Sprite準備
		atrum->PrepareSprite();

		atrum->DrawSpriteRect(spriteTexture, spriteColor, sphereUvTransform, spriteTransform, spriteSize);

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