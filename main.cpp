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
	srand( static_cast<unsigned int>(time(nullptr)));

	/* テクスチャ */

	uint32_t textureUvChecker = atrum->GetTexture("./Resources/Images/uvChecker.png");

	/* 3dカメラ */

	// カメラの座標情報
	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -300.0f} };

	/* Triangle */

	AtrumEngine::Transform triangleTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };
	
	Vector4 triangleColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	
	std::array<AtrumEngine::VertexData, 3> triangleVertexData = {
		Vector4{-50.0f, -50.0f, 0.0f, 1.0f},Vector2{0.0f, 1.0f},
		Vector4{0.0f, 50.0f, 0.0f, 1.0f},Vector2{0.5f, 0.0f},
		Vector4{50.0f, -50.0f, 0.0f, 1.0f},Vector2{1.0f, 1.0f}
	};

	/* Sphere */

	AtrumEngine::Transform sphereTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };
	Vector4 sphereColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	float sphereRadius = 50.0f;

	/* Sprite */
	
	AtrumEngine::Transform spriteTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{256.0f, 128.0f} };
	Vector4 spriteColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	Vector2 spriteSize = Vector2{ 512.0f, 256.0f };

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

			atrum->DrawTriangle(textureUvChecker, triangleColor, triangleTransform, cameraTransform, triangleVertexData);

			atrum->DrawSphere(textureUvChecker, sphereColor, sphereTransform, cameraTransform, sphereRadius, 16);

			// Sprite準備
			atrum->PrepareSprite();

			atrum->DrawSpriteRect(textureUvChecker, spriteColor, spriteTransform, spriteSize);

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