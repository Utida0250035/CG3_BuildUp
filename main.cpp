#include "AtrumEngine.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// エンジンインスタンスの取得
	AtrumEngine* atrum = AtrumEngine::GetInstance();

	// エンジンの初期化
	atrum->Initialize("CG2", 1280, 720);

	while (atrum->IsProcess()) {
		// ウィンドウの×ボタンが押されるまでループ

		if (!atrum->MessageForOs()) {

			/*============== メインループ =================*/

			// 画面クリア
			atrum->ClearWindow();

			///
			/// ↓ 更新ここから
			///

			///
			/// ↑更新ここまで
			/// 

			///
			/// ↓描画ここから
			/// 

			///
			/// ↑描画ここまで
			/// 

		}

	}

	///
	/// ↓終了処理
	/// 

	atrum->Finalize();

	return 0;

}