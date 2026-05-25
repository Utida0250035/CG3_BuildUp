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
	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -500.0f} };

	/* Sprite */
	
	AtrumEngine::Transform spriteTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{256.0f, 128.0f} };

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

			ImGui::Begin("spriteTransform");

			ImGui::DragFloat3("scale", &spriteTransform.scale.x);
			ImGui::DragFloat3("rotate", &spriteTransform.rotate.x);
			ImGui::DragFloat3("translate", &spriteTransform.translate.x);

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



			// Sprite準備
			atrum->PrepareSprite();

			atrum->DrawSpriteRect(textureUvChecker, Vector4{ 1.0f, 1.0f, 1.0f, 1.0f }, spriteTransform, Vector2{512.0f, 256.0f});

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