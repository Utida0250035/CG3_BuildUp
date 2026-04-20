#include "AtrumEngine.h"
#include "Log.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ログ出力ファイルの初期化
	LogFile::GetInstance()->Initialize();

	// エンジンインスタンスの取得
	AtrumEngine* atrum = AtrumEngine::GetInstance();

	// エンジンの初期化
	atrum->Initialize("CG2", 1280, 720);


	// PSOを生成
	atrum->CreatePSO();

	// VertexResourceを生成
	atrum->CreateVertexResource();

	// vertexBufferViewを作成
	atrum->CreateVertexBufferView();

	// vertexResourceにデータを書き込む
	atrum->WriteVertexResource();


	while (atrum->IsProcess()) {
		// ウィンドウの×ボタンが押されるまでループ

		if (!atrum->MessageForOs()) {

			/*============== メインループ =================*/

			// 画面クリア
			atrum->UpdateWindow();

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