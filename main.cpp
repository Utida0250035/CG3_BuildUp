#include "AtrumEngine.h"
#include "Log.h"
#include <numbers>

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// ログ出力ファイルの初期化
	LogFile::GetInstance()->Initialize();

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

	// vertexResourceにデータを書き込む
	atrum->WriteVertexResource();


	// 三角形の色
	Vector4 triangleColor = Vector4{0.0f, 0.1f, 0.1f, 1.0f};

	// 三角形の座標情報
	AtrumEngine::Transform triangleTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.03f, 0.0f}, Vector3{0.0f, 0.0f, 0.0f } };

	// 三角形の回転速度
	float triangleRotateSpeedY = 0.03f; //std::numbers::pi_v<float> * 0.03125f;


	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -5.0f} };

	// カメラのワールド行列
	Matrix4x4 cameraWorldMatrix = atrum->CreateWorldMatrix(cameraTransform);

	// ビュー行列
	Matrix4x4 viewMatrix = MatrixInverse(cameraWorldMatrix);

	// 透視投影行列
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.5f, 1.77777f, 0.125f, 128.0f);

	// 三角形のTransform
	Matrix4x4 triangleWorldMatrix = atrum->CreateWorldMatrix(triangleTransform);

	// WvpMatrixを作る
	Matrix4x4 worldViewProjectionMatrix = triangleWorldMatrix * viewMatrix * projectionMatrix;


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

			triangleTransform.rotate.y += triangleRotateSpeedY;

			if (triangleTransform.rotate.y >= 2.0f * std::numbers::pi_v<float>) {

				triangleTransform.rotate.y -= 2.0f * std::numbers::pi_v<float>;

			}

			///
			/// ↑更新ここまで
			/// 

			///
			/// ↓描画ここから
			/// 

			triangleWorldMatrix = atrum->CreateWorldMatrix(triangleTransform);

			worldViewProjectionMatrix = triangleWorldMatrix * viewMatrix * projectionMatrix;;

			atrum->SetMaterialData(triangleColor);
			atrum->SetWvpData(worldViewProjectionMatrix);

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