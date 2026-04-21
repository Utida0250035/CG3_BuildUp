#include "AtrumEngine.h"
#include "Log.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ログ出力ファイルの初期化
	LogFile::GetInstance()->Initialize();

	// エンジンインスタンスの取得
	AtrumEngine* atrum = AtrumEngine::GetInstance();

	// エンジンの初期化
	atrum->Initialize("CG2",1280, 720);

	// fps設定
	atrum->SetFps(0.016666f);


	// PSOを生成
	atrum->CreatePSO();

	// VertexResourceを生成
	atrum->CreateVertexResource();

	// vertexBufferViewを作成
	atrum->CreateVertexBufferView();

	// vertexResourceにデータを書き込む
	atrum->WriteVertexResource();


	// 三角形の色
	Vector4 triangleColor = Vector4{0.0f, 0.1f, 0.1f, 1.0f};

	while (atrum->IsProcess()) {
		// ウィンドウの×ボタンが押されるまでループ

		if (atrum->IsExecuteFrame()) {

			/*============== メインループ =================*/

			// 画面更新
			atrum->UpdateWindow();

			///
			/// ↓ 更新ここから
			///

			triangleColor.x += 1.0f / 180.0f;

			if (triangleColor.x >= 0.8f) {

				triangleColor.x = 0.0f;

			}

			///
			/// ↑更新ここまで
			/// 

			///
			/// ↓描画ここから
			/// 

			atrum->SetMaterialData(triangleColor);

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