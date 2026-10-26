#pragma once

#include "Math/Transform.h"
#include "Math/Vector3.h"
#include "System/Hash64.h"
#include "System/Particle/Particle.h"
#include "System/Particle/ParticleParam.h"
#include <memory>
#include <random>
#include <vector>

namespace Atrum {
	class FrameDeltaTime;
	class AssetModel;
	class Camera;
} // namespace Atrum

namespace Atrum::Visual::Particle {

	class Emitter {
	private:
		FrameDeltaTime* frameDeltaTime_ = nullptr;

		struct Layer {
			Param param;

			// CONTINUOUS型 断続発射用の蓄積タイマー
			float accumulator = 0.0f;
			// CONTINUOUS型 レイヤー発生からの経過時間(パーティクルの寿命とは別)
			float elapsedSeconds = 0.0f;
			// BURST型 バースト済みフラグ
			bool hasBurst = false;
			// 共通 終了フラグ
			bool isFinished = false;
			// MODEL 用（renderMode == MODEL のとき）
			std::shared_ptr<AssetModel> model = nullptr;
			// BILLBOARD 用テクスチャインデックス
			uint32_t billboardTextureIndex = 0;
		};

#ifdef DEVELOPMENT

		std::string name_ = "";

#endif

		Camera* camera_ = nullptr;
		uint64_t nameHash_ = 0;
		std::vector<Layer> layers_{};
		std::vector<std::vector<Particle>> particles_{};
		Math::TransformLH transform_{};
		bool isAlive_ = false;
		bool isEmitting_ = false;

		size_t ResolveSpawnCount(Layer& layer);
		Math::Vector3 ResolveSpawnLocalPos(Layer& layer);
		Math::Vector3
			ResolveSimulationSpace(SimulationSpace& simulationSpaceFromEmitter,
				Math::Vector3 localPos);

		void ParticleInit(Particle& particle, Layer& layer);

		void EmitOneLayer(const size_t layerIndex);

		void ResolveContinuousEmit(const size_t layerIndex);

		void ConfigureLayerResources(Layer &layer);

		void ResetLayerRuntimeState(Layer &layer, const size_t layerIndex);

	public:
		Emitter() = default;
		~Emitter() = default;

		void SetName(const std::string& name) {
		
#ifdef DEVELOPMENT
			name_ = name;
#endif

			nameHash_ = Hash64(name);


		};

		bool IsAlive() const { return isAlive_; }
		const std::vector<std::vector<Particle>>& GetParticles() const {
			return particles_;
		}

		void Initialize(const char* jsonFilePath);

		/// メモリ上の Param 配列からレイヤーを構築する（エディタ用）
		void InitializeFromParams(const std::vector<Param> &params);

		/// Param を差し替えてプレビューを最初から再生する
		void RebuildFromParams(const std::vector<Param> &params);

		[[nodiscard]] std::vector<Param> ExportParams() const;

		/// ワンショット再発火用にレイヤー状態と粒子をリセットする
		void PrepareForOneShot();

		/// バースト発火＋連続発生タイマーのリセット（プレビュー再生）
		void RestartPreview();
		void Emit();
		void Update();
		void Draw();

#ifdef DEVELOPMENT
		const std::string& GetName() const { return name_; }
#endif

		const Math::TransformLH& GetTransform() const { return transform_; }
		Math::TransformLH& RefTransform() { return transform_; }

		void SetTransform(const Math::TransformLH& transform) {
			transform_ = transform;
		}

		void SetCamera(Camera* camera) { camera_ = camera; }
	};

} // namespace Atrum::Visual::Particle