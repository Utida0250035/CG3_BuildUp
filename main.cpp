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

	/* テクスチャ */

	uint32_t textureWhite4x4 = atrum->GetTexture("./Resources/Images/white4x4.png");

	/* 3dカメラ */

	// カメラの座標情報
	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -500.0f} };


	/* 矩形 */

	// 矩形の色
	Vector4 rectColor{ 1.0f, 1.0f, 1.0f, 1.0f };

	// 矩形の座標情報
	AtrumEngine::Transform rectTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{640.0f, 360.0f, 0.0f} };

	// 矩形のテクスチャ
	uint32_t rectTexture = textureWhite4x4;

	/* 線分 */

	// 線分の色
	Vector4 lineSegmentColor{ 1.0f, 1.0f, 0.1f, 1.0f };

	// 線のテクスチャ
	uint32_t lineSegmentTexture = textureWhite4x4;


	/* 背景 */

	// 背景の色
	Vector4 backgroundColor{ 0.0f, 0.0f, 0.0f, 1.0f };

	// 背景の座標情報
	AtrumEngine::Transform backgroundTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{640.0f, 360.0f, 0.0f} };

	// 背景のサイズ
	Vector2 backgroundSize{ 1280.0f, 720.0f };

	uint32_t backgroundTexture = textureWhite4x4;


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

			atrum->DrawSpriteRect(backgroundTexture, backgroundColor, backgroundTransform, backgroundSize);

			atrum->DrawSpriteLine(lineSegmentTexture, lineSegmentColor, Vector2{ 32.0f, 32.0f }, Vector2{ 128.0f, 128.0f }, 4.0f);

			// 矩形の描画
			atrum->DrawSpriteRect(rectTexture, rectColor, rectTransform, Vector2{ 128.0f, 128.0f });

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