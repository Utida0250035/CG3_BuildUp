#include "Audio/Audio.h"
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

	std::unique_ptr<AudioManager> audio = std::make_unique<AudioManager>();
	audio->Initialize();

	[[maybe_unused]] size_t seAlarm = audio->LoadSe("./Assets/Audios/Alarm01.wav");

	size_t seCat = audio->LoadSe("./Assets/Audios/seCat.mp3");

	/* Triangle */

	Transform triangleTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{std::numbers::pi_v<float> *0.5f, 0.0f, 0.0f}, Vector3{0.0f, -1.0f, 0.0f} };

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

	/* HitMesh */

	PyramidMesh pyramidMesh{};
	HitMesh hitMeshPyramid = HitMeshBuilder::CreateFromPyramid(PyramidMesh{});
	hitMeshPyramid.position = { 0.0f, 10.0f, 0.0f };
	hitMeshPyramid.velocity = { 0.0f, -0.1f, 0.0f };
	hitMeshPyramid.rotation = Quaternion::Identity();
	hitMeshPyramid.angularVelocity = {};
	hitMeshPyramid.inverseMass = 1.0f;
	hitMeshPyramid.mass = 3.0f;

	HitMesh hitMeshTriangle = HitMeshBuilder::CreateFromTriangle(triangle);

	/* Collision */

	CollisionManager colM{};

	/* DirectionalLight */

	DirectionalLightData directionalLightData = DirectionalLightData{
		.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
		.direction = Vector3{0.0f, -1.0f, 0.0f},
		.intensity = 10.0f,
		.lightModel = LightModel::HalfLambert
	};

	bool isLightingEnable = true;

	/* gravity */

	float gravity = -8.0f;

	/* deltaTime */

	std::unique_ptr<DeltaTime> deltaTimeCalc = std::make_unique<DeltaTime>();
	float deltaTime = 0.0f;

	/* 物理実行フラグ */

	bool isPhysicsMove = false;

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

	ImVec2 imguiChildSize = ImVec2(0.0f, 0.0f);
	ImGuiChildFlags imguiChildFlags = ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY;

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

		if (playInput->IsKeyTrigger(Key::SPACE)) {

			audio->PlaySe(seCat);

		}

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

		ImGui::SmallButton("seCat");

		if (ImGui::IsItemActivated()) {

			audio->PlaySe(seCat);

		}

		ImGui::SmallButton("seAlarm");

		if (ImGui::IsItemActivated()) {

			audio->PlaySe(seAlarm);

		}

		ImGui::DragFloat("gravity", &gravity);

		ImGui::SmallButton("physicsSwitch");

		if (ImGui::IsItemActivated()) {

			isPhysicsMove = !isPhysicsMove;

		}

		if (isPhysicsMove) {

			ImGui::Text("now on");

		} else {

			ImGui::Text("now off");

		}

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

		ImGui::DragFloat4("color", &directionalLightData.color.x, 0.03125f);

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

		/* HitMeshのGUI */

		ImGui::Begin("HitMesh");

		ImGui::SmallButton("reset(transform)");

		if (ImGui::IsItemActivated()) {

			hitMeshPyramid.position = { 0.0f, 1.0f, 0.0f };
			hitMeshPyramid.angularVelocity = {};
			hitMeshPyramid.rotation = {};
			hitMeshPyramid.velocity = {};

		}

		ImGui::SmallButton("reset(velocity, position)");

		if (ImGui::IsItemActivated()) {

			hitMeshPyramid.position = { 0.0f, 1.0f, 0.0f };
			hitMeshPyramid.velocity = {};

		}

		ImGui::DragFloat3("pos", &hitMeshPyramid.position.x);
		ImGui::DragFloat3("velocity", &hitMeshPyramid.velocity.x);
		ImGui::DragFloat4("rotate(Quaternion)", &hitMeshPyramid.rotation.x);

		if (ImGui::IsItemActive()) {

			hitMeshPyramid.rotation.Normalize();

		}

		ImGui::DragFloat3("angularVelocity", &hitMeshPyramid.angularVelocity.x);

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

		/* 三角形GUI */

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

		atrum->ImGuiRender();

#endif

		camera->Update();

		Vector3 vertices[3]{};

		for (size_t i = 0; i < 3; ++i) {

			const auto& v = triangleVertexData[i];

			vertices[i] = { v.position.x, v.position.y, v.position.w };

		}

		if (isPhysicsMove) {

			Matrix4x4 worldMat = triangleTransform.MakeWorldMatrix();

			triangle.v0 = worldMat.Transform(vertices[0]);
			triangle.v1 = worldMat.Transform(vertices[1]);
			triangle.v2 = worldMat.Transform(vertices[2]);

			triangle.normal = (triangle.v1 - triangle.v0).Cross(triangle.v2 - triangle.v0);
			triangle.normal.Normalize();

			hitMeshTriangle = HitMeshBuilder::CreateFromTriangle(triangle);

			hitMeshPyramid.velocity.y += gravity * deltaTime;

			hitMeshPyramid.velocity.y = std::clamp(hitMeshPyramid.velocity.y, -5.0f, 5.0f);

			hitMeshPyramid.Update(deltaTime);

			for (size_t i = 0; i < hitMeshPyramid.localVertices.size(); ++i) {
				Vector3 w = hitMeshPyramid.worldMatrix.Transform(hitMeshPyramid.localVertices[i]);

			}

			colM.Clear();

			colM.AddBody(&hitMeshTriangle);

			colM.AddBody(&hitMeshPyramid);

			colM.CheckCollision();

		}

		playInput->EndOfFrame();

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

		atrum->DrawTriangle(triangleTexture, triangleColor, triangleUvTransform, triangleTransform, triangleVertexData, isLightingEnable);

		atrum->DrawAsymmetricPyramid(textureWhite, Vector4::Red(), Transform{}, { 1.0f, 1.0f, 1.0f }, hitMeshPyramid.rotation, hitMeshPyramid.position, pyramidMesh, isLightingEnable);


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