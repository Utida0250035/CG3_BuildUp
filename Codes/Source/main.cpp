#include "AtrumEngine.h"
#include "Bezier.h"
#include "Collision.h"
#include "DeltaTime.h"
#include "Log.h"
#include "OBB.h"
#include "StaticCast.h"
#include <numbers>

enum class Scene {
	kTriangleScene,
	kPerformanceScene
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
	srand(static_cast<unsigned int>(time(nullptr)));

	/* テクスチャ */

	const char* textureFilePaths[3] = {
		"./Resources/Images/ForStudy/uvChecker.png",
		"./Resources/Images/ForStudy/monsterBall.png",
		"./Resources/Images/white4x4.png"
	};

	uint32_t textureWhite = atrum->GetTexture(textureFilePaths[2]);

	/* 3dカメラ */

	// カメラの座標情報
	AtrumEngine::Transform cameraTransform{ Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.0f, 0.0f}, Vector3{0.0f, 0.0f, -50.0f} };

	/* Triangle */

	AtrumEngine::Transform triangleTransform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.03f, 0.0f}, Vector3{} };

	Vector4 triangleColor = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	AtrumEngine::Transform triangleUvTransform{};

	std::array<AtrumEngine::VertexData, 3> triangleVertexData = {
		Vector4{-5.0f, -5.0f, 0.0f, 1.0f},Vector2{0.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{0.0f, 5.0f, 0.0f, 1.0f},Vector2{0.5f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{5.0f, -5.0f, 0.0f, 1.0f},Vector2{1.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f}
	};

	uint32_t triangleTexture = textureWhite;

	/* Triangle2 */

	AtrumEngine::Transform triangle2Transform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{0.0f, 0.1f, 0.0f}, Vector3{} };

	Vector4 triangle2Color = Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
	AtrumEngine::Transform triangle2UvTransform{};

	std::array<AtrumEngine::VertexData, 3> triangle2VertexData = {
		Vector4{-5.0f, -5.0f, 0.0f, 1.0f},Vector2{0.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{0.0f, 5.0f, 0.0f, 1.0f},Vector2{0.5f, 0.0f}, Vector3{0.0f, 0.0f, -1.0f},
		Vector4{5.0f, -5.0f, 0.0f, 1.0f},Vector2{1.0f, 1.0f}, Vector3{0.0f, 0.0f, -1.0f}
	};

	uint32_t triangle2Texture = textureWhite;

	/* 正四面体 */

	constexpr size_t tetrasMaxCount = 64;

	AtrumEngine::Transform tetraTransforms[tetrasMaxCount]{};

	for (auto& transform : tetraTransforms) {

		transform = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };

	}

	Vector4 defaultTetraColor = { 0.2f, 0.6f, 0.4f };

	Vector4 tetraColors[tetrasMaxCount]{};

	for (auto& color : tetraColors) {

		color = defaultTetraColor;

	}

	float tetraLifeCount[tetrasMaxCount] = { 0.0f };
	float tetraLife[tetrasMaxCount] = { 0.0f };

	Vector3 tetraVelocity[tetrasMaxCount]{};
	Vector3 tetraRotateVelocity[tetrasMaxCount]{};

	for (size_t i = 0; i < tetrasMaxCount; ++i) {

		// 寿命リセット
		tetraLifeCount[i] = 0.0f;
		tetraLife[i] = Float(rand() % 3 + 1);

		// 速度リセット
		tetraVelocity[i] = Vector3{ Float(rand() % 33 - 16) * 0.01562f, Float(rand() % 33 - 16) * 0.01562f, Float(rand() % 33 - 16) * 0.01562f };

		// 回転リセット
		tetraRotateVelocity[i] = Vector3{ Float(rand() % 9 - 4) * 0.01562f, Float(rand() % 9 - 4) * 0.01562f, Float(rand() % 9 - 4) * 0.01562f };

		tetraTransforms[i] = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };

	}

	/* DirectionalLight */

	AtrumEngine::DirectionalLightData directionalLightData = AtrumEngine::DirectionalLightData{
		.color = Vector4{1.0f, 1.0f, 1.0f, 1.0f},
		.direction = Vector3{0.0f, -1.0f, 0.0f},
		.intensity = 10.0f,
		.lightModel = AtrumEngine::LightModel::HalfLambert
	};

	bool isLightingEnable = true;

	/* Scene */

	Scene scene = Scene::kTriangleScene;

	/* deltatime */

	std::unique_ptr<DeltaTime> deltaTime = std::make_unique<DeltaTime>();

	/* 乱数初期化 */

	srand(static_cast<unsigned int>(time(nullptr)));

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

			deltaTime->CalcDeltaTime();

#ifdef USE_IMGUI

			ImGui::Begin("sceneChange");

			ImGui::SmallButton("triangle");

			if(ImGui::IsItemActivated()) {

				scene = Scene::kTriangleScene;

			}

			ImGui::SmallButton("performance");

			if (ImGui::IsItemActivated()) {

				scene = Scene::kPerformanceScene;

				for (size_t i = 0; i < tetrasMaxCount; ++i) {

					// 寿命 リセット
					tetraLifeCount[i] = 0.0f;
					tetraLife[i] = Float(rand() % 3 + 1);

					// 速度 リセット
					tetraVelocity[i] = Vector3{ Float(rand() % 33 - 16) * 0.01562f, Float(rand() % 33 - 16) * 0.01562f, Float(rand() % 33 - 16) * 0.01562f };

					// 回転 リセット
					tetraRotateVelocity[i] = Vector3{ Float(rand() % 9 - 4) * 0.01562f, Float(rand() % 9 - 4) * 0.01562f, Float(rand() % 9 - 4) * 0.01562f };

					// トランスフォーム リセット
					tetraTransforms[i] = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };

				}

			}

			ImGui::End();



			ImGui::Begin("camera");

			ImGui::DragFloat3("rotate", &cameraTransform.rotate.x, 0.03125f);
			ImGui::DragFloat3("translate", &cameraTransform.translate.x, 0.03125f);

			ImGui::End();

			ImGui::Begin("directionalLight");

			ImGui::DragFloat4("color", &directionalLightData.color.x, 0.03125f);

			ImGui::DragFloat3("direction", &directionalLightData.direction.x, 0.03125f);

			if (ImGui::IsItemActive()) {

				directionalLightData.direction = VectorNormalize(directionalLightData.direction);

			}

			ImGui::DragFloat("intensity", &directionalLightData.intensity, 0.03125f);

			ImGui::Checkbox("isLightingEnable", &isLightingEnable);

			if (isLightingEnable) {

				int lightMode = static_cast<int>(directionalLightData.lightModel);

				ImGui::Selectable("lambert", lightMode == 0);
				if (ImGui::IsItemActivated()) {

					lightMode = 0;

				}

				ImGui::Selectable ("halfLambert", lightMode == 1);
				if (ImGui::IsItemActivated()) {

					lightMode = 1;

				}

				directionalLightData.lightModel = static_cast<AtrumEngine::LightModel>(lightMode);

			}

			ImGui::End();

			if (scene == Scene::kTriangleScene) {


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

				ImGui::Text("uvTransform");

				ImGui::DragFloat2("uvScale", &triangleUvTransform.scale.x, 0.03125f);
				ImGui::DragFloat("uvRotate", &triangleUvTransform.rotate.z, 0.03125f);
				ImGui::DragFloat2("uvTranslate", &triangleUvTransform.translate.x, 0.03125f);

				ImGui::Text("texture");

				for (const auto& path : textureFilePaths) {

					ImGui::Selectable(path);

					if (ImGui::IsItemActivated()) {

						triangleTexture = atrum->GetTexture(path);

					}

				}

				ImGui::End();


				ImGui::Begin("triangle2");

				ImGui::Text("transform");
				ImGui::DragFloat3("scale", &triangle2Transform.scale.x, 0.03125f);
				ImGui::DragFloat3("rotate", &triangle2Transform.rotate.x, 0.03125f);
				ImGui::DragFloat3("translate", &triangle2Transform.translate.x, 0.03125f);

				ImGui::Text("color, vertexData");
				ImGui::DragFloat4("color", &triangle2Color.x, 0.03125f, 0.0f, 1.0f);

				for (size_t i = 0; i < 3; ++i) {

					ImGui::DragFloat3(("vertexPos" + std::to_string(i)).c_str(), &triangle2VertexData[i].position.x, 0.03125f);
					ImGui::DragFloat2(("texCoord" + std::to_string(i)).c_str(), &triangle2VertexData[i].texCoord.x, 0.03125f);

				}

				ImGui::Text("uvTransform");

				ImGui::DragFloat2("uvScale", &triangle2UvTransform.scale.x, 0.03125f);
				ImGui::DragFloat("uvRotate", &triangle2UvTransform.rotate.z, 0.03125f);
				ImGui::DragFloat2("uvTranslate", &triangle2UvTransform.translate.x, 0.03125f);
				
				ImGui::Text("texture");

				for (const auto& path : textureFilePaths) {

					ImGui::Selectable(path);

					if (ImGui::IsItemActivated()) {

						triangle2Texture = atrum->GetTexture(path);

					}

				}

				ImGui::End();

#endif

			} else if (scene == Scene::kPerformanceScene) {

				for (size_t i = 0; i < tetrasMaxCount; ++i) {

					tetraTransforms[i].rotate += tetraRotateVelocity[i];
					tetraTransforms[i].translate += tetraVelocity[i];

					tetraColors[i] = defaultTetraColor *  (tetraLife[i] - tetraLifeCount[i]) / tetraLife[i];

					tetraLifeCount[i] += deltaTime->GetDeltaTime();

					if (tetraLifeCount[i] >= tetraLife[i]) {

						// 寿命 リセット
						tetraLifeCount[i] = 0.0f;
						tetraLife[i] = Float(rand() % 3 + 1);

						// 速度 リセット
						tetraVelocity[i] = Vector3{Float(rand() % 33 - 16) * 0.01562f, Float(rand() % 33 - 16) * 0.01562f, Float(rand() % 33 - 16) * 0.01562f };

						// 回転速度 リセット
						tetraRotateVelocity[i] = Vector3{ Float(rand() % 9 - 4) * 0.01562f, Float(rand() % 9 - 4) * 0.01562f, Float(rand() % 9 - 4) * 0.01562f };
						
						// トランスフォーム リセット
						tetraTransforms[i] = { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} };

					}

				}

			}

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

			if (scene == Scene::kTriangleScene) {
				
				if (isLightingEnable) {

					atrum->DrawTriangle(triangleTexture, triangleColor, triangleUvTransform, triangleTransform, cameraTransform, triangleVertexData, directionalLightData);

					atrum->DrawTriangle(triangle2Texture, triangle2Color, triangle2UvTransform, triangle2Transform, cameraTransform, triangle2VertexData, directionalLightData);

				} else {

					atrum->DrawTriangle(triangleTexture, triangleColor, triangleUvTransform, triangleTransform, cameraTransform, triangleVertexData);

					atrum->DrawTriangle(triangle2Texture, triangle2Color, triangle2UvTransform, triangle2Transform, cameraTransform, triangle2VertexData);

				}

			} else if (scene == Scene::kPerformanceScene) {

				if (isLightingEnable) {

					for (size_t i = 0; i < 64; ++i) {

						atrum->DrawRegularTetrahedron(textureWhite, tetraColors[i], { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} }, tetraTransforms[i], cameraTransform, 1.0f, directionalLightData);

					}

				} else {

					for (size_t i = 0; i < 64; ++i) {

						atrum->DrawRegularTetrahedron(textureWhite, tetraColors[i], { Vector3{1.0f, 1.0f, 1.0f}, Vector3{}, Vector3{} }, tetraTransforms[i], cameraTransform, 1.0f);

					}

				}

			}

			// Sprite準備
			atrum->PrepareSprite();

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