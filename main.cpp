#include "AtrumEngine.h"
#include "Log.h"
#include <numbers>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	LeakChecker leakChecker;

	// エンジンインスタンスの取得
	AtrumEngine* atrum = AtrumEngine::GetInstance();

	// エンジンの初期化
	atrum->Initialize("CG2", 1280, 720);

	// fps設定
	atrum->SetFps(60);

	/* 3dカメラ */

	// カメラの座標情報
	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -5.0f} };


	/* 三角形 */

	// 三角形の色
	Vector4 triangleColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };

	// 三角形の座標情報
	AtrumEngine::Transform triangleTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.03f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f } };

	// テクスチャ取得
	uint32_t triangleTexture = atrum->GetTexture("./Resources/Images/uvChecker.png");


	/* Sprite */

	// Spriteの座標情報
	AtrumEngine::Transform spriteTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f} };

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

			ImGui::ShowDemoWindow();

			ImGui::Begin("camera");

			ImGui::DragFloat3("rotate", &cameraTransform.rotate.x, 0.03125f);

			ImGui::DragFloat3("translate", &cameraTransform.translate.x, 0.03125f);

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

			AtrumEngine::VertexData vertexData[3]{};
			// 左下
			vertexData[0].position = Vector4{ -0.5f, -0.5f, 0.0f, 1.0f };
			vertexData[0].texCoord = Vector2{ 0.0f, 1.0f };
			// 上
			vertexData[1].position = Vector4{ 0.0f, 0.5f, 0.0f, 1.0f };
			vertexData[1].texCoord = Vector2{ 0.5f, 0.0f };
			// 右下
			vertexData[2].position = Vector4{ 0.5f, -0.5f, 0.0f, 1.0f };
			vertexData[2].texCoord = Vector2{ 1.0f, 1.0f };

			// 三角形の描画
			atrum->DrawTriangle(triangleTexture, triangleColor, triangleTransform, cameraTransform, vertexData);

			vertexData[0].position = { -0.5f, -0.5f, 0.5f, 1.0f };
			vertexData[0].texCoord = { 0.0f, 1.0f };
			
			vertexData[1].position = { 0.0f, 0.0f, 0.0f, 1.0f};
			vertexData[1].texCoord = { 0.5f ,0.0f };

			vertexData[2].position = { 0.5f, -0.5f, -0.5f, 1.0f};
			vertexData[2].texCoord = { 1.0f, 1.0f };

			// 三角形の描画
			atrum->DrawTriangle(triangleTexture, triangleColor, triangleTransform, cameraTransform, vertexData);

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