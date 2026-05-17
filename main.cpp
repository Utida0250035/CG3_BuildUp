#include "AtrumEngine.h"
#include "Bezier.h"
#include "Collision.h"
#include "Log.h"
#include "OBB.h"
#include <numbers>

struct ObbObject {
	RigidBodyOBB body{};

	bool isExist = false;

	Vector4 color{};

};

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

	uint32_t textureWhite4x4 = atrum->GetTexture("./Resources/Images/white4x4.png");

	/* 3dカメラ */

	// カメラの座標情報
	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -500.0f} };

	/* ベジェ曲線 */

	// 線の色
	Vector4 lineSegmentColor{ 1.0f, 1.0f, 0.1f, 1.0f };

	// 線のテクスチャ
	uint32_t lineSegmentTexture = textureWhite4x4;

	// 分割数
	uint32_t divide = 64;

	// 分割されたt
	float dividedT = 0.015625f;

	// 3つの制御点
	Vector2 controlPoints[3] = { Vector2{256.0f, 256.0f}, Vector2{640.0f, 640.0f}, Vector2{1024.0f, 512.0f} };


	/* 矩形 */

	// 矩形の色(白)
	Vector4 rectColorWhite{ 1.0f, 1.0f, 1.0f, 1.0f };

	// 矩形の色(赤)
	Vector4 rectColorRed{ 1.0f, 0.1f, 0.1f, 1.0f };

	Vector4 rectColorGreen{ 0.1f, 1.0f, 0.1f, 1.0f };

	// 矩形のテクスチャ
	uint32_t rectTexture = textureWhite4x4;

	Vector2 boxSize{ 32.0f, 32.0f };

	ObbObject boxes[8]{};

	{

		int xRange = static_cast<int>(controlPoints[2].x - controlPoints[0].x);

		int randX = 0;
		int randY = 0;

		for (auto& obj : boxes) {

			auto& body = obj.body;

			body.axis[0] = Vector2{ 1.0f, 0.0f };
			body.axis[1] = Vector2{ 0.0f, 1.0f };

			body.size = boxSize;
			body.halfSize = body.size * 0.5f;

			randX = rand() % xRange + static_cast<int>(controlPoints[0].x);
			randY = rand() % 32 - 64;

			body.center = Vector2{ static_cast<float>(randX), static_cast<float>(randY)};

			body.angularVelocity = 0.0f;

			body.velocity = Vector2{ 0.0f, 0.0f };

			obj.isExist = true;

			obj.color = rectColorWhite;

			body.UpdateInertiaMoment();

		}

	}

	/* 重力(下向き) */

	// 重力加速度
	float gravity = 0.0625f;

	// 終端速度
	float terminalSpeed = 5.0f;


	/* 背景 */

	// 背景の色
	Vector4 backgroundColor{ 0.0f, 0.0f, 0.0f, 1.0f };

	// 背景の座標情報
	AtrumEngine::Transform backgroundTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{640.0f, 360.0f, -1000.0f} };

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

			for (auto& obj : boxes) {

				if (!obj.isExist) {

					continue;

				}

				auto& body = obj.body;

				body.velocity.y += gravity;

				if (body.velocity.y >= terminalSpeed) {

					body.velocity.y = terminalSpeed;

				}

				// 平行運動
				body.center += body.velocity;

				if (body.center.x < -body.size.x || body.center.x > 1280.0f + body.size.x) {

					obj.isExist = false;
					continue;

				}

				if (body.center.y > 720.0f + body.size.y) {

					obj.isExist = false;
					continue;

				}

				// 回転運動
				float theta = body.CalculateAngle();
				theta += body.angularVelocity;
				body.UpdateAxis(theta);

				// 当たり判定 / 衝突応答
				if (ResolveRigidBodyObbBezierResponseDetailed(body, controlPoints, 0.8f, 0.8f, 0.05f, 8)) {

					obj.color = rectColorRed;

				}

			}

#ifdef USE_IMGUI

			ImGui::Begin("box[0]");

			ImGui::DragFloat2("pos", &boxes[0].body.center.x);

			ImGui::SmallButton("resetPos");

			if (ImGui::IsItemActivated()) {

				boxes[0].body.center = Vector2{};

			}

			ImGui::DragFloat2("size", &boxes[0].body.size.x);

			if (ImGui::IsItemActive()) {

				boxes[0].body.halfSize = boxes[0].body.size * 0.5f;

				boxes[0].body.UpdateInertiaMoment();

			}

			ImGui::DragFloat2("velocity", &boxes[0].body.velocity.x);

			ImGui::SmallButton("resetVel");

			if (ImGui::IsItemActivated()) {

				boxes[0].body.velocity = Vector2{};

			}

			ImGui::DragFloat("angularVelocity", &boxes[0].body.angularVelocity);

			ImGui::SmallButton("resetAngularVel");

			if (ImGui::IsItemActivated()) {

				boxes[0].body.angularVelocity = 0.0f;

			}

			float angle = boxes[0].body.CalculateAngle();

			ImGui::DragFloat("angle", &angle);

			boxes[0].body.UpdateAxis(angle);

			ImGui::SmallButton("resetAngle");

			if (ImGui::IsItemActivated()) {

				boxes[0].body.UpdateAxis(0.0f);

			}

			ImGui::Checkbox("isExist", &boxes[0].isExist);

			ImGui::SmallButton("resetAll");

			if(ImGui::IsItemActivated()) {

				boxes[0].body.angularVelocity = 0.0f;
				boxes[0].body.velocity = Vector2{};
				boxes[0].body.UpdateAxis(0.0f);
				boxes[0].body.center = Vector2{};

			}

			ImGui::End();

			ImGui::Begin("allBox");

			ImGui::SmallButton("resetAll");

			if (ImGui::IsItemActivated()) {

				for (auto& obj : boxes) {

					obj.isExist = false;

					auto& body = obj.body;

					body.angularVelocity = 0.0f;
					body.center = Vector2{};
					body.velocity = Vector2{};
					body.UpdateAxis(0.0f);

				}

			}

			ImGui::SmallButton("generate");

			if (ImGui::IsItemActivated()) {

				int xRange = static_cast<int>(controlPoints[2].x - controlPoints[0].x);

				int randX = 0;
				int randY = 0;

				for (auto& obj : boxes) {

					auto& body = obj.body;

					body.axis[0] = Vector2{ 1.0f, 0.0f };
					body.axis[1] = Vector2{ 0.0f, 1.0f };

					randX = rand() % 96 + 32;

					randY = rand() % 96 + 32;

					body.size = { static_cast<float>(randX), static_cast<float>(randY)};
					body.halfSize = body.size * 0.5f;

					randX = rand() % xRange + static_cast<int>(controlPoints[0].x);
					randY = rand() % 32 - 64;

					body.center = Vector2{ static_cast<float>(randX), static_cast<float>(randY) };

					body.angularVelocity = 0.0f;

					body.velocity = Vector2{ 0.0f, 0.0f };

					obj.isExist = true;

					obj.color = rectColorWhite;

					body.UpdateInertiaMoment();

				}

			}
			
			ImGui::DragFloat("gravity", &gravity);

			ImGui::DragFloat("terminalSpeed", &terminalSpeed);

			ImGui::End();

			ImGui::Begin("existCount");

			uint32_t existCount = 0;

			for (const auto& obj : boxes) {

				if (!obj.isExist) {

					continue;

				}

				existCount++;

			}

			ImGui::Text("%d", existCount);

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

			atrum->DrawSpriteRect(backgroundTexture, backgroundColor, backgroundTransform, backgroundSize);

			Vector2 passPoint0 = controlPoints[0];
			Vector2 passPoint1{};

			float t = 0.0f;

			for (uint32_t i = 0; i < divide; i++) {

				t += dividedT;

				passPoint1 = CalcBezier2(controlPoints, t);

				atrum->DrawSpriteLine(lineSegmentTexture, lineSegmentColor, passPoint0, passPoint1, 4.0f, -50.0f);

				passPoint0 = passPoint1;

			}

			// 矩形の座標情報
			AtrumEngine::Transform rectTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{0.0f, 0.0f, 0.0f} };

			for (auto& obj : boxes) {

				if (!obj.isExist) {

					continue;

				}

				auto& body = obj.body;

				rectTransform.translate.x = body.center.x;
				rectTransform.translate.y = body.center.y;

				rectTransform.rotate.z = body.CalculateAngle();

				// 矩形の描画
				atrum->DrawSpriteRect(rectTexture, rectColorGreen, rectTransform, body.size * 1.1f);

				// 矩形の描画
				atrum->DrawSpriteRect(rectTexture, obj.color, rectTransform, body.size);

			}

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