#include "System/Particle/EmitterManager.h"

#include "Camera/Camera.h"
#include "System/Hash64.h"

#include <algorithm>
#include <cassert>
#include <string>

namespace Atrum::Particle::Emitter {

	std::unique_ptr<EmitterManager> EmitterManager::instance_ = nullptr;

	EmitterManager* EmitterManager::Instance() {
		if (!instance_) {
			instance_ = std::make_unique<EmitterManager>(Token{});
		}

		return instance_.get();
	}

	void EmitterManager::Destroy() {
		if (instance_) {
			instance_.reset();
		}
	}

	void EmitterManager::Register(
		const std::string& name,
		std::unique_ptr<Atrum::Visual::Particle::Emitter>&& emitter) {
		assert(emitter != nullptr);

		const uint64_t nameHash = Hash64(name.c_str());

		emitter->SetName(name);
		if (camera_ != nullptr) {
			emitter->SetCamera(camera_);
		}

		RegisteredEmitter entry{};
		entry.nameHash = nameHash;
		entry.emitter = std::move(emitter);

		emitters_.emplace_back(std::move(entry));
	}

	namespace {

		bool IsEmitterParticleInactive(
			const Atrum::Visual::Particle::Emitter& emitter) {
			for (const auto& layerParticles : emitter.GetParticles()) {
				for (const auto& particle : layerParticles) {
					if (particle.isActive) {
						return false;
					}
				}
			}
			return true;
		}

	} // namespace

	Atrum::Visual::Particle::Emitter*
		EmitterManager::AddOneShotEmitter(const std::string& emitterName,
			const char* jsonFilePath,
			const Math::Vector3& worldPosition) {
		auto emitter = std::make_unique<Atrum::Visual::Particle::Emitter>();
		emitter->Initialize(jsonFilePath);

		Atrum::Visual::Particle::Emitter* rawEmitter = emitter.get();
		Register(emitterName, std::move(emitter));

		Math::TransformLH transform{};
		transform.translate = worldPosition;
		rawEmitter->SetTransform(transform);
		if (camera_ != nullptr) {
			rawEmitter->SetCamera(camera_);
		}
		rawEmitter->Emit();
		return rawEmitter;
	}

	Atrum::Visual::Particle::Emitter* EmitterManager::Find(const std::string& name) {
		return FindByNameHash(Hash64(name.c_str()));
	}

	Atrum::Visual::Particle::Emitter*
		EmitterManager::FindByNameHash(const uint64_t nameHash) {
		for (RegisteredEmitter& entry : emitters_) {
			if (entry.emitter != nullptr && entry.nameHash == nameHash) {
				return entry.emitter.get();
			}
		}
		return nullptr;
	}

	void EmitterManager::SetCamera(Camera* camera) {
		camera_ = camera;
		for (RegisteredEmitter& entry : emitters_) {
			if (entry.emitter) {
				entry.emitter->SetCamera(camera_);
			}
		}
	}

	void EmitterManager::Emit(const std::string& name) {
		const uint64_t nameHash = Hash64(name.c_str());
		for (RegisteredEmitter& entry : emitters_) {
			if (entry.emitter != nullptr && entry.nameHash == nameHash) {
				entry.emitter->Emit();
			}
		}
	}

	void EmitterManager::EmitAll() {
		for (RegisteredEmitter& entry : emitters_) {
			if (entry.emitter) {
				entry.emitter->Emit();
			}
		}
	}

	void EmitterManager::Update() {
		for (RegisteredEmitter& entry : emitters_) {
			if (entry.emitter) {
				entry.emitter->Update();
			}
		}
	}

	void EmitterManager::Draw() {
		for (RegisteredEmitter& entry : emitters_) {
			if (entry.emitter) {
				entry.emitter->Draw();
			}
		}
	}

	void EmitterManager::RemoveInactive() {
		const auto isActiveEntry = [](const RegisteredEmitter& entry) {
			return entry.emitter != nullptr &&
				!IsEmitterParticleInactive(*entry.emitter);
		};

		emitters_.erase(
			std::remove_if(emitters_.begin(), emitters_.end(),
			[&isActiveEntry](const RegisteredEmitter& entry) {
			return !isActiveEntry(entry);
		}),
			emitters_.end());
	}

	void EmitterManager::Clear() {
		emitters_.clear();
	}

} // namespace Atrum::Particle::Emitter
