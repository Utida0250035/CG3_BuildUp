#include "AtrumEngine.h"
#include "Bezier.h"
#include "Collision.h"
#include "Log.h"
#include "OBB.h"
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

	/* テクスチャ */

	uint32_t textureUvChecker = atrum->GetTexture("./Resources/Images/uvChecker.png");
	uint32_t textureMonsterBall = atrum->GetTexture("./Resources/Images/monsterBall.png");
	uint32_t textureUvCheckerExtra = atrum->GetTexture("./Resources/Images/uvChecker.png");

	/* 3dカメラ */

	// カメラの座標情報
	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -50.0f} };

	/* Triangle */

	AtrumEngine::Transform triangleTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };

	Vector4 triangleColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };

	std::array<AtrumEngine::VertexData, 3> triangleVertexData = {
		Vector4{-5.0f, -5.0f, 0.0f, 1.0f},Vector2{0.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{0.0f, 5.0f, 0.0f, 1.0f},Vector2{0.5f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{5.0f, -5.0f, 0.0f, 1.0f},Vector2{1.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f}
	};

	/* Sphere */

	AtrumEngine::Transform sphereTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };
	Vector4 sphereColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	float sphereRadius = 5.0f;

	/* Sprite */

	AtrumEngine::Transform spriteTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{256.0f, 128.0f} };
	Vector4 spriteColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	Vector2 spriteSize = Vector2{ 512.0f, 256.0f };

	/* DirectionalLight */

	AtrumEngine::DirectionalLightData directionalLightData = AtrumEngine::DirectionalLightData{
		.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
		.direction = Vector3{0.0f, -1.0f, 0.0f},
		.intensity = 10.0f,
		.lightModel = AtrumEngine::LightModel::HalfLambert
	};

	bool isLightingEnable = true;

	while (atrum->IsProcess()) {
		// ウィンドウの×ボタンが押されるまでループ

		if (atrum->IsFrameExecute()) {

			/*============== メインループ =================*/

#ifdef USE_IMGUI

			// ImGuiにフレーム開始を通知
			atrum->ImGuiNewFrame();

#endif

			///
			/// ↓ 更新ここから
			///

#ifdef USE_IMGUI

			ImGui::Begin("camera");

			ImGui::DragFloat3("rotate", &cameraTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &cameraTransform.translate.x, 0.03125f);

			ImGui::End();

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

				ImGui::Selectable ("halfLambert", lightMode == 1);
				if (ImGui::IsItemActivated()) {

					lightMode = 1;

				}

				directionalLightData.lightModel = static_cast<AtrumEngine::LightModel>(lightMode);

			}

			ImGui::End();

			ImGui::Begin("triangle");

			ImGui::Text("transform");
			ImGui::DragFloat3("scale", &triangleTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &triangleTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &triangleTransform.translate.x, 0.03125f);

			ImGui::Text("color, vertexData");
			ImGui::DragFloat4("color", &triangleColor.x, 0.03125f, 0.0f, 1.0f);

			for (size_t i = 0; i < 3; ++i) {

				ImGui::DragFloat3(("vertexPos" + std::to_string(i)).c_str(), &triangleVertexData[i].position.x, 0.03125f);
				ImGui::DragFloat2(("texCoord" + std::to_string(i)).c_str(), &triangleVertexData[i].texCoord.x, 0.03125f);

			}

			ImGui::End();

			ImGui::Begin("sphere");

			ImGui::Text("transform");
			ImGui::DragFloat3("scale", &sphereTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &sphereTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &sphereTransform.translate.x, 0.03125f);

			ImGui::Text("color, radius");
			ImGui::DragFloat4("color", &sphereColor.x, 0.03125f, 0.0f, 1.0f);
			ImGui::DragFloat("radius", &sphereRadius);

			ImGui::End();


			ImGui::Begin("sprite");

			ImGui::Text("transform");
			ImGui::DragFloat3("scale", &spriteTransform.scale.x, 0.03125f);
			ImGui::DragFloat3("rotate", &spriteTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &spriteTransform.translate.x);

			ImGui::Text("color, size");
			ImGui::DragFloat4("color", &spriteColor.x, 0.03125f, 0.0f, 1.0f);
			ImGui::DragFloat2("size", &spriteSize.x);

			ImGui::End();

			atrum->ImGuiRender();

#endif

			///
			/// ↑更新ここまで
			/// 

			///
			/// ↓描画ここから
			/// 

			// 描画処理(前)
			atrum->PreDraw();

			if (isLightingEnable) {

				atrum->DrawTriangle(textureUvChecker, triangleColor, triangleTransform, cameraTransform, triangleVertexData, directionalLightData);

				atrum->DrawSphere(textureMonsterBall, sphereColor, sphereTransform, cameraTransform, sphereRadius, 16, directionalLightData);

			} else {

				atrum->DrawTriangle(textureUvChecker, triangleColor, triangleTransform, cameraTransform, triangleVertexData);

				atrum->DrawSphere(textureMonsterBall, sphereColor, sphereTransform, cameraTransform, sphereRadius, 16);

			}

			// Sprite準備
			atrum->PrepareSprite();

			atrum->DrawSpriteRect(textureUvCheckerExtra, spriteColor, spriteTransform, spriteSize);

			// 描画処理(後)
			atrum->PostDraw();

			///
			/// ↑描画ここまで
			/// 

		}

	}

	///
	/// ↓終了処理
	/// 

	atrum->Finalize();

	AtrumEngine::Destroy();

	return 0;

}