#include "Audio/Audio.h"
#include "Camera/Camera.h"
#include "Cast/StaticCast.h"
#include "Collision/CollisionManager.h"
#include "Collision/HitMesh.h"
#include "Collision/HitMeshBuilder.h"
#include "Debug/DebugCamera.h"
#include "Debug/Log.h"
#include "Engine/AtrumEngine.h"
#include "Geometry/OBB.h"
#include "Geometry/PyramidMesh.h"
#include "Geometry/Triangle.h"
#include "Input/DirectInput.h"
#include "Input/PlayInput.h"
#include "Json/JsonTest.h"
#include "Time/DeltaTime.h"
#include <format>
#include <iostream>
#include <memory>
#include <numbers>

namespace {

	using Atrum::LeakChecker;

	using Atrum::Input::PlayInput;
	using Atrum::Input::Key;

	using Atrum::Audio::AudioManager;
	using Atrum::Audio::AudioHandle;
	using Atrum::Audio::To100nsPositive;

	using Atrum::Math::Transform;

	using Atrum::Math::Vector2;
	using Atrum::Math::Vector3;
	using Atrum::Math::Vector4;

	using Atrum::Math::Matrix4x4;
	using Atrum::Math::Quaternion;

	using Atrum::Physics::HitMesh;
	using Atrum::Physics::HitMeshBuilder;
	using Atrum::Physics::CollisionManager;

	using Atrum::Geometry::Triangle;
	using Atrum::Geometry::PyramidMesh;


	using Atrum::VertexData;
	using Atrum::LightModel;
	using Atrum::DirectionalLightData;

	using Atrum::DeltaTime;

	using Atrum::Json::JsonTest;

#ifdef  _DEBUG

	using Atrum::Debug::DebugCamera;

#else

	using Atrum::Camera;

#endif //  _DEBUG

}

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	LeakChecker leakChecker;

	// エンジンインスタンスの取得
	Atrum::Engine* atrum = Atrum::Engine::GetInstance();

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
		"./Assets/Images/ForStudy/uvChecker.png",
		"./Assets/Images/ForStudy/monsterBall.png",
		"./Assets/Images/white4x4.png"
	};

	uint32_t textureWhite = atrum->GetTexture(textureFilePaths[2]);

	/* 音源 */

	AudioManager* audio = AudioManager::GetInstance();


	[[maybe_unused]] size_t bgmShiningStarMp3 = 0;
	[[maybe_unused]] size_t bgmShiningStarM4a = 0;

	{

		std::string name = "./Assets/Audios/bgmShiningStar";

		bgmShiningStarMp3 = audio->Load(name + ".mp3");
		bgmShiningStarM4a = audio->Load(name + ".m4a");

	}

	AudioHandle bgmPlayHandle{};


	/* 平面3dModel */

	auto planeModel = atrum->GetModel("./Assets/Objects/ForStudy", "plane.obj", "./Assets/Objects/ForStudy", "plane.mtl");
	Transform planeModelTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };
	planeModelTransform.translate.z = -20.0f;

	/* 複数メッシュ3dModel */

	auto multiMeshModel = atrum->GetModel("./Assets/Objects/ForStudy", "multiMesh.obj", "./Assets/Objects/ForStudy", "multiMesh.mtl");
	Transform multiMeshModelTransform{};

	/* 複数マテリアル3dModel */

	auto multiMtlModel = atrum->GetModel("./Assets/Objects/ForStudy", "multiMaterial.obj", "./Assets/Objects/ForStudy", "multiMaterial.mtl");
	Transform multiMtlModelTransform{};

	/* Sprite */

	Transform spriteTransform{};
	Transform spriteUvTransform{};
	Vector2 spriteSize{ 64.0f, 64.0f };
	uint32_t spriteTexture = textureWhite;
	Vector4 spriteColor = Vector4::White();


	/* DirectionalLight */

	DirectionalLightData directionalLightData = DirectionalLightData{
		.color = Vector3{1.0f, 1.0f, 1.0f},
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

#ifdef _DEBUG

	std::unique_ptr<DebugCamera> camera = nullptr;

	camera = std::make_unique<DebugCamera>();

#else

	std::unique_ptr<Camera> camera = nullptr;

	camera = std::make_unique<Camera>();

#endif

	camera->Initialize();



	camera->CreateOrthographicMatrix(1280, 720);

#ifdef USE_IMGUI

	/* ImGui */

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	/* Json入出力テスト */

	JsonTest jsonTest;

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

		Vector2 cursorPos = playInput->GetCursorPos();

		Vector2 cursorDelta = playInput->GetCursorDelta();

		[[maybe_unused]] int32_t mouseWheel = playInput->GetMouseWheel();

#ifdef USE_IMGUI

		jsonTest.jsonVal.ImGui("./Assets/Sample/test.json");

		ImGui::Begin("debug");

		ImGui::Text("timeCount: %f", timeCount);

		ImGui::DragInt("mouseWheel", &mouseWheel);

		ImGui::DragFloat2("cursorPos", &cursorPos.x);

		ImGui::DragFloat2("cursorDelta", &cursorDelta.x);

		ImGui::End();

		ImGui::Begin("audio");

		if (ImGui::BeginChild("shiningStar", ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text("shiningStar");

			const char* extensions[] = {
				"mp3",
				"m4a"
			};

			const size_t soundIndices[] = {
				bgmShiningStarMp3,
				bgmShiningStarM4a
			};

			for (size_t i = 0; i < 2; ++i) {

				ImGui::SmallButton(extensions[i]);

				if (ImGui::IsItemActivated()) {

					if (audio->IsPlaying(bgmPlayHandle)) {

						audio->Stop(bgmPlayHandle);

					}

					bgmPlayHandle = audio->Play(soundIndices[i], false, To100nsPositive(0.5f));

				}

			}

		}

		ImGui::EndChild();

		ImGui::End();


		ImGui::Begin("DebugCamera");

		ImGui::Text("mode: %d", camera->GetMode());

		ImGui::DragFloat("distance", &camera->RefDistance());

		ImGui::DragFloat3("translate", &camera->RefTranslate().x);

		ImGui::DragFloat4("quaternion", &camera->RefQuaternion().x, 0.03125f);

		ImGui::DragFloat4("pivotQuaternion", &camera->RefPivotQuaternion().x, 0.03125f);

		ImGui::DragFloat3("pivot", &camera->RefPivot().x, 0.03125f);

		const Vector3& pivot = camera->RefPivot();
		const Vector3& translate = camera->RefTranslate();

		Vector3 direction = (pivot - translate).Normalized();
		Vector3 calculatedDirection = camera->RefQuaternion().RotateVector({ 0.0f, 0.0f, -1.0f });

		ImGui::DragFloat3("direction", &direction.x);
		ImGui::DragFloat3("direction(calc)", &calculatedDirection.x);

		if (ImGui::IsItemActive()) {

			camera->RefQuaternion().Normalize();

		}

		if (ImGui::IsItemActive()) {

			camera->RefPivotQuaternion().Normalize();

		}

		ImGui::End();


		/* 光源GUI */

		ImGui::Begin("directionalLight");

		ImGui::DragFloat3("color", &directionalLightData.color.x, 0.03125f, 0.0f, 1.0f);

		ImGui::DragFloat3("direction", &directionalLightData.direction.x, 0.03125f);

		if (ImGui::IsItemActive()) {

			directionalLightData.direction.Normalize();

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

		/* 平面3dモデルGUI */

		ImGui::Begin("planeModel");

		if (ImGui::BeginChild("transform", ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &planeModelTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &planeModelTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &planeModelTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		ImGui::End();

		/* 複数メッシュ3dモデルGUI */

		ImGui::Begin("multiMeshModel");

		if (ImGui::BeginChild("transform", ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &multiMeshModelTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &multiMeshModelTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &multiMeshModelTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		ImGui::End();

		/* 複数マテリアル3dモデルGUI */

		ImGui::Begin("multiMaterialModel");

		if (ImGui::BeginChild("transform", ImGui::kChildSize, ImGui::kChildFlags)) {

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

		if (ImGui::BeginChild("transform", ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text("transform");

			ImGui::DragFloat3("scale", &spriteTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &spriteTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &spriteTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		if (ImGui::BeginChild("uvTransform", ImGui::kChildSize, ImGui::kChildFlags)) {

			ImGui::Text("uvTransform");

			ImGui::DragFloat2("scale", &spriteUvTransform.scale.x, 0.03125f);
			ImGui::DragFloat("rotate", &spriteUvTransform.rotate.x, 0.03125f);
			ImGui::DragFloat2("translate", &spriteUvTransform.translate.x, 0.03125f);

		}

		ImGui::EndChild();

		if (ImGui::BeginChild("texture", ImGui::kChildSize, ImGui::kChildFlags)) {

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

		camera->Update();

		///
		/// ↑更新ここまで
		/// 

		///
		/// ↓描画ここから
		/// 

		atrum->SetViewMatrix(camera->GetViewMatrix());

		// 光源データの設定
		atrum->SetDirectionalLightData(directionalLightData);

		// 描画処理(前)
		atrum->PreDraw();

		atrum->DrawModel(planeModel.get(), planeModelTransform, isLightingEnable);
		atrum->DrawModel(multiMeshModel.get(), multiMeshModelTransform, isLightingEnable);
		atrum->DrawModel(multiMtlModel.get(), multiMtlModelTransform, isLightingEnable);


		// Sprite準備
		atrum->PrepareSprite();

		atrum->DrawSpriteRect(spriteTexture, spriteColor, spriteUvTransform, spriteTransform, spriteSize);


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

	Atrum::Engine::Destroy();

	return 0;

}