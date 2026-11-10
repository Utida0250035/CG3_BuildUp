#include "System/Particle/ParticleEmitter.h"
#include "Camera/Camera.h"
#include "Engine/AtrumEngine.h"
#include "Engine/Renderer/BillboardMatrix.h"
#include "Engine/Resource/ParticleGpuData.h"
#include "Geometry/GeometryRandom.h"
#include "Math/Lerp.h"
#include "Math/Vector3.h"
#include "System/DeltaTime.h"
#include "System/Json/JsonHandler.h"
#include "System/Random.h"
#include "System/WhiteTexture.h"
#include <cassert>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <numbers>
#include <vector>

namespace {
	namespace Alg = Atrum::Algorithm;
	namespace M = Atrum::Math;
	namespace G = Atrum::Geometry;
	namespace fs = std::filesystem;
} // namespace

namespace Atrum::Visual::Particle {

namespace {

	namespace MH = Atrum::Math;
	namespace GH = Atrum::Geometry;

	// 点が軸平行 AABB 内にあるか（min ≤ p ≤ max）
	bool IsInsideAabb(const MH::Vector3 &p, const GH::AABB &aabb) {
		return p.x >= aabb.min.x && p.x <= aabb.max.x && p.y >= aabb.min.y &&
		       p.y <= aabb.max.y && p.z >= aabb.min.z && p.z <= aabb.max.z;
	}

	// killFieldParam の座標系における評価用位置（フィールド原点は position）
	MH::Vector3 ResolveKillFieldEvaluationPoint(
	    const MH::Vector3 &particleLocal, const MH::Vector3 &particleWorld,
	    const KillFieldParam &kf) {
		if (kf.simulationSpaceFromEmitter == SimulationSpace::LOCAL) {
			return particleLocal - kf.position;
		}
		return particleWorld - kf.position;
	}

	// キルフィールド形状の内外（形状ローカル座標＝ position 原点）
	bool IsInsideKillFieldShape(const MH::Vector3 &evaluationPoint,
	                            const KillFieldParam &kf) {
		switch (kf.shape) {
			case KillFieldShape::SPHERE: {
				const MH::Vector3 delta =
				    evaluationPoint - kf.localSphere.center;
				return delta.LengthSquare() <=
				       kf.localSphere.radius * kf.localSphere.radius;
			}
			case KillFieldShape::AABB:
				return IsInsideAabb(evaluationPoint, kf.localAabb);
			default:
				return false;
		}
	}

	// mode に応じて「消すべき」なら true
	bool ShouldKillByField(bool insideField, KillFieldMode mode) {
		switch (mode) {
			case KillFieldMode::INSIDE:
				return insideField;
			case KillFieldMode::OUTSIDE:
				return !insideField;
			default:
				return false;
		}
	}

	// Param の流体抵抗（空気抵抗を主とする）: 速度に対して指数減衰
	// damping は大きいほど速く止まる（単位: 1/s 程度の減衰率として扱う）
	void ApplyFluidDamping(MH::Vector3 &velocity, float dampingPerSecond,
	                       float deltaTime) {
		if (dampingPerSecond <= 0.0f || deltaTime <= 0.0f) {
			return;
		}
		const float factor = std::exp(-dampingPerSecond * deltaTime);
		velocity *= factor;
	}

} // namespace

	void Emitter::ConfigureLayerResources(Layer& layer) {
		layer.model = nullptr;
		layer.billboardTextureIndex = WhiteTextureIndex();

		switch (layer.param.renderMode) {
			case RenderMode::MODEL:
			{
				const std::string objPath =
					layer.param.objDirectoryPath + "/" + layer.param.objFileName;
				const std::string mtlPath =
					layer.param.mtlDirectoryPath + "/" + layer.param.mtlFileName;
				if (fs::exists(objPath) && fs::exists(mtlPath)) {
					layer.model = Engine::Instance()->GetModel(
						layer.param.objDirectoryPath, layer.param.objFileName,
						layer.param.mtlDirectoryPath, layer.param.mtlFileName);
				}
				break;
			}
			case RenderMode::BILLBOARD:
			{
				if (fs::exists(layer.param.textureFilePath)) {
					layer.billboardTextureIndex =
						Engine::Instance()->GetTexture(layer.param.textureFilePath);
				}
				break;
			}
			case RenderMode::BUILTIN_SPHERE:
			default:
				break;
		}
	}

	void Emitter::ResetLayerRuntimeState(Layer& layer, const size_t layerIndex) {
		layer.accumulator = 0.0f;
		layer.elapsedSeconds = 0.0f;
		layer.hasBurst = false;
		layer.isFinished = false;

		assert(layer.param.emitRate > 0.0f);
		assert(layer.param.duration > 0.0f);
		assert(layer.param.maxCount > 0);

		particles_[layerIndex].resize(layer.param.maxCount);
		for (Particle& particle : particles_[layerIndex]) {
			particle.isActive = false;
		}
	}

	void Emitter::InitializeFromParams(const std::vector<Param>& params) {
		frameDeltaTime_ = ::Atrum::EngineFrameDeltaTime();

		layers_.resize(params.size());
		particles_.resize(params.size());

		for (size_t i = 0; i < layers_.size(); ++i) {
			layers_[i].param = params[i];
			ConfigureLayerResources(layers_[i]);
			ResetLayerRuntimeState(layers_[i], i);
		}
	}

	void Emitter::Initialize(const char* jsonFilePath) {
		std::vector<Param> params{};

		if (fs::exists(jsonFilePath)) {
			params = Json::JsonHandler::LoadFromFile<std::vector<Param>>(jsonFilePath);
		} else {
#if defined(DEVELOPMENT) && !defined(NDEBUG)
			assert(false && "Particle JSON が見つかりません（ビルド後の Assets コピーを確認）");
#endif
			params.emplace_back();
		}

		InitializeFromParams(params);
	}

	void Emitter::RebuildFromParams(const std::vector<Param>& params) {
		InitializeFromParams(params);
		RestartPreview();
	}

	std::vector<Param> Emitter::ExportParams() const {
		std::vector<Param> params;
		params.reserve(layers_.size());
		for (const Layer& layer : layers_) {
			params.push_back(layer.param);
		}
		return params;
	}

	void Emitter::RestartPreview() {
		PrepareForOneShot();
		Emit();
	}

	void Emitter::PrepareForOneShot() {
		for (size_t i = 0; i < layers_.size(); ++i) {
			Layer& layer = layers_[i];
			layer.accumulator = 0.0f;
			layer.elapsedSeconds = 0.0f;
			layer.hasBurst = false;
			layer.isFinished = false;

			for (Particle& particle : particles_[i]) {
				particle.isActive = false;
			}
		}
	}

	void Emitter::Emit() {

		for (size_t i = 0; i < layers_.size(); ++i) {

			if (layers_[i].param.emitMode == EmitMode::CONTINUOUS)
				continue;

			EmitOneLayer(i);
		}
	}

	void Emitter::Update() {

	  /* レイヤー管理 */

		bool isLayerAnyParticleActive = false;

		for (size_t i = 0; i < layers_.size(); ++i) {
			auto& layer = layers_[i];

			if (layer.isFinished)
				continue;

			auto& layerParticles = particles_[i];

			ResolveContinuousEmit(i);

			isLayerAnyParticleActive = false;

			const float deltaTime = frameDeltaTime_->GetDeltaTime();
			const M::Matrix4x4 emitterWorld = transform_.MakeWorldMatrix();
			const M::Matrix4x4 emitterWorldInverse = emitterWorld.Inversed();

			for (auto& p : layerParticles) {
				if (!p.isActive)
					continue;

				p.lifeTimePassed += deltaTime;

				if (p.lifeTimePassed >= p.lifeDuration) {
					p.isActive = false;
					continue;
				}

				isLayerAnyParticleActive = true;

				// 発生座標系（LOCAL）とワールド座標の両方（キルフィールド用）
				M::Vector3 particleWorld{};
				M::Vector3 particleLocal{};
				if (layer.param.simulationSpaceFromEmitter == SimulationSpace::WORLD) {
					particleWorld = p.position;
					particleLocal = emitterWorldInverse.Transform(particleWorld);
				} else {
					particleLocal = p.position;
					particleWorld = emitterWorld.Transform(particleLocal);
				}

				const KillFieldParam& killField = layer.param.killFieldParam;
				if (killField.isUse) {
					const M::Vector3 evalPoint = ResolveKillFieldEvaluationPoint(
					    particleLocal, particleWorld, killField);
					const bool insideField =
					    IsInsideKillFieldShape(evalPoint, killField);
					if (ShouldKillByField(insideField, killField.mode)) {
						p.isActive = false;
						continue;
					}
				}

				p.velocity += p.acceleration * deltaTime;
				ApplyFluidDamping(p.velocity, layer.param.damping, deltaTime);
				p.position += p.velocity * deltaTime;

				p.currentScale = Math::Lerp(p.startScale, p.endScale,
					p.lifeTimePassed / p.lifeDuration);

				p.currentColor = Math::Lerp(p.startColor, p.endColor,
					p.lifeTimePassed / p.lifeDuration);
			}

			layer.elapsedSeconds += deltaTime;

			if (layer.param.emitMode == EmitMode::BURST) {
				if (!isLayerAnyParticleActive && layer.hasBurst) {
					layer.isFinished = true;
				}
			} else if (layer.param.emitMode == EmitMode::CONTINUOUS) {
				if (!isLayerAnyParticleActive &&
					layer.elapsedSeconds >= layer.param.duration && !layer.param.isLooping) {
					layer.isFinished = true;
				}
			}
		}
	}

	size_t Emitter::ResolveSpawnCount(Layer& layer) {

		if (layer.param.emitMode == EmitMode::BURST) {
			return layer.param.burstCount;

		} else if (layer.param.emitMode == EmitMode::CONTINUOUS) {
			return 1;
		}

		return 0;
	}

	Math::Vector3 Emitter::ResolveSpawnLocalPos(Layer& layer) {

		switch (layer.param.emitShape) {

			case EmitShape::AABB:
				return Geometry::Random::InAABB(layer.param.emitAabb);

			case EmitShape::CIRCLE_XZ:

				return Geometry::Random::InCircleXZ(layer.param.emitCircle.radius,
					layer.param.emitCircle.center);

			case EmitShape::CIRCLE_XY:
				return Geometry::Random::InCircleXY(layer.param.emitCircle.radius,
					layer.param.emitCircle.center);

			case EmitShape::CIRCLE_YZ:

				return Geometry::Random::InCircleYZ(layer.param.emitCircle.radius,
					layer.param.emitCircle.center);

			case EmitShape::SPHERE:
				return Geometry::Random::InSphere(layer.param.emitSphere);

			case EmitShape::POINT:
				return layer.param.emitPointPos;
			default:
				return Math::Vector3::Zero();
		}
	}

	Math::Vector3
		Emitter::ResolveSimulationSpace(SimulationSpace& simulationSpaceFromEmitter,
			Math::Vector3 localPos) {
		if (simulationSpaceFromEmitter == SimulationSpace::WORLD) {
			return transform_.MakeWorldMatrix().Transform(localPos);
		} else {
			return localPos;
		}
	}

	void Emitter::ParticleInit(Particle& particle, Layer& layer) {

		Math::Vector3 localPos = ResolveSpawnLocalPos(layer);

		particle.isActive = true;

		particle.position =
			ResolveSimulationSpace(layer.param.simulationSpaceFromEmitter, localPos);

		particle.lifeDuration = Alg::Random::Float(layer.param.minLifeDuration,
			layer.param.maxLifeDuration);

		Math::Vector3 randomVel{ Alg::Random::Float(-layer.param.velocityRandom.x,
												   layer.param.velocityRandom.x),
								Alg::Random::Float(-layer.param.velocityRandom.y,
												   layer.param.velocityRandom.y),
								Alg::Random::Float(-layer.param.velocityRandom.z,
												   layer.param.velocityRandom.z) };

		switch (layer.param.movementMode) {

			case MovementMode::RADIAL_CONVERGE:
			{
				Math::Vector3 dir = -localPos;
				if (dir.LengthSquare() > 1e-5f) {
					dir.Normalize();
				} else {
					dir = { 0.0f, 1.0f, 0.0f };
				}
				float speed = layer.param.baseVelocity.Length();
				particle.velocity = (dir * speed) + randomVel;
				break;
			}

			case MovementMode::RADIAL_SPREAD:
			{
				Math::Vector3 dir = localPos;
				if (dir.LengthSquare() > 1e-5f) {
					dir.Normalize();
				} else {
					dir = { 0.0f, 1.0f, 0.0f };
				}
				float speed = layer.param.baseVelocity.Length();
				particle.velocity = (dir * speed) + randomVel;
				break;
			}

			case MovementMode::DIRECTIONAL:
				[[fallthrough]];
			default:
				particle.velocity = layer.param.baseVelocity + randomVel;
				break;
		}

		particle.lifeTimePassed = 0.0f;

		particle.startColor = layer.param.startColor;
		particle.endColor = layer.param.endColor;
		particle.currentColor = particle.startColor;

		particle.startScale = layer.param.startScale;
		particle.endScale = layer.param.endScale;
		particle.currentScale = particle.startScale;

		particle.acceleration = layer.param.acceleration;
		particle.damping = layer.param.damping;
	}

	void Emitter::EmitOneLayer(const size_t layerIndex) {

		auto& layer = layers_[layerIndex];
		auto& layerParticles = particles_[layerIndex];

		if (layer.hasBurst)
			return;

		size_t spawnCount = ResolveSpawnCount(layer);

		size_t spawnedCount = 0;

		for (auto& p : layerParticles) {
			if (!p.isActive) {

				ParticleInit(p, layer);

				++spawnedCount;

				if (spawnedCount >= spawnCount) {
					break;
				}
			}
		}

		if (layer.param.emitMode == EmitMode::BURST && spawnedCount > 0) {
			layer.hasBurst = true;
		}
	}

	void Emitter::ResolveContinuousEmit(const size_t layerIndex) {

		auto& layer = layers_[layerIndex];

		if (layer.elapsedSeconds >= layer.param.duration && !layer.param.isLooping)
			return;

		if (layer.param.emitMode == EmitMode::CONTINUOUS) {

			layer.accumulator += frameDeltaTime_->GetDeltaTime();

			while (layer.accumulator >= 1.0f / layer.param.emitRate) {
				EmitOneLayer(layerIndex);
				layer.accumulator -= 1.0f / layer.param.emitRate;
			}
		}
	}

	void Emitter::Draw() {

		Atrum::Engine* engine = ::Atrum::EngineInstance();
		if (engine == nullptr) {
			return;
		}

		if (camera_ != nullptr) {
			camera_->UpdateMatrix();
			engine->SetCamera(camera_);
		}

		// 戦闘演出は同フレーム大量描画になりやすいため细分を抑える
		constexpr uint32_t kBuiltinSphereSubdivision = 2;

		for (size_t i = 0; i < layers_.size(); ++i) {

			const auto& layer = layers_[i];

			if (layer.isFinished)
				continue;

			const auto& layerParticles = particles_[i];

			std::vector<Atrum::ParticleInstanceGpuData> batch;
			batch.reserve(layer.param.maxCount);

			Math::TransformLH particleTransform{};

			for (const auto& p : layerParticles) {
				if (!p.isActive)
					continue;

				particleTransform.translate = p.position;
				particleTransform.quaternion = p.rotation;
				particleTransform.scale = p.currentScale;

				Atrum::ParticleInstanceGpuData gpu{};
				gpu.color = p.currentColor;
				gpu.radius = 1.0f;

				switch (layer.param.renderMode) {
					case RenderMode::BUILTIN_SPHERE:
					{
						gpu.radius =
							std::max({ p.currentScale.x, p.currentScale.y, p.currentScale.z });
						particleTransform.scale = Math::Vector3::One();
						gpu.world = particleTransform.MakeWorldMatrix();
						break;
					}
					case RenderMode::MODEL:
						gpu.world = particleTransform.MakeWorldMatrix();
						break;
					case RenderMode::BILLBOARD:
					{
						if (camera_ != nullptr) {
							gpu.world = Renderer::MakeViewPlaneParallelBillboardWorld(
								p.position, p.currentScale, camera_->GetViewMatrix());
						} else {
							gpu.world = Math::Matrix4x4::Identity();
						}
						break;
					}
					default:
						continue;
				}
				batch.push_back(gpu);
			}

			if (batch.empty()) {
				continue;
			}

			switch (layer.param.renderMode) {
				case RenderMode::BUILTIN_SPHERE:
					engine->DrawSphereParticlesInstanced(
						WhiteTextureIndex(), Math::Transform{}, batch,
						kBuiltinSphereSubdivision, false);
					break;
				case RenderMode::MODEL:
					if (layer.model != nullptr) {
						engine->DrawModelParticlesInstanced(
							layer.model.get(), batch, false);
					}
					break;
				case RenderMode::BILLBOARD:
					engine->DrawBillboardParticlesInstanced(
						layer.billboardTextureIndex, Math::Transform{}, batch, false);
					break;
				default:
					break;
			}
		}
	}

} // namespace Atrum::Visual::Particle