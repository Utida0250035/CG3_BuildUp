#include "AtrumEngine.h"
#include "Log.h"
#include <numbers>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	LeakChecker leakChecker;
	
	// エンジンインスタンスの取得
	AtrumEngine* atrum = AtrumEngine::GetInstance();

	// エンジンの初期化
	atrum->Initialize("CG2",1280, 720);

	// fps設定
	atrum->SetFps(60);


	// PSOを生成
	atrum->CreatePSO();

	// VertexResourceを生成
	atrum->CreateVertexResource();

	// vertexBufferViewを作成
	atrum->CreateVertexBufferView();

	// 三角形の色
	Vector4 triangleColor = Vector4{1.0f, 1.0f, 1.0f, 1.0f};

	// 三角形の座標情報
	AtrumEngine::Transform triangleTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.03f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f } };


	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -5.0f} };

	// テクスチャ取得
	uint32_t triangleTexture = atrum->GetTexture("./Resources/Images/uvChecker.png");

	while (atrum->IsProcess()) {
		// ウィンドウの×ボタンが押されるまでループ

		if (atrum->IsExecuteFrame()) {

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


			ImGui::Begin("triangle");

			ImGui::DragFloat3("scale", &triangleTransform.scale.x, 0.0625f);

			ImGui::DragFloat3("rotate", &triangleTransform.rotate.x, 0.03125f);

			ImGui::DragFloat3("translate", &triangleTransform.translate.x, 0.0625f);

			ImGui::DragFloat4("RGBA", &triangleColor.x, 0.001f, 0.0f, 1.0f);

			ImGui::End();


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

			// 三角形の描画
			atrum->DrawTriangle(triangleTexture, triangleColor, triangleTransform, cameraTransform);

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