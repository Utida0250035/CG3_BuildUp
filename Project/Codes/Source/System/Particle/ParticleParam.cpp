#include "System/Particle/ParticleParam.h"
#include "System/SimulationSpace.h"
#include <nlohmann/json.hpp>

namespace Atrum {

// 同一 TU で Param::from_json から参照できるようここで定義する
NLOHMANN_JSON_SERIALIZE_ENUM(
    SimulationSpace,
    {{SimulationSpace::LOCAL, "LOCAL"}, {SimulationSpace::WORLD, "WORLD"}})

} // namespace Atrum

namespace Atrum::Visual::Particle {

namespace {

// size_t は nlohmann::json::value / get の対象外のため、整数経由で読む
size_t ReadJsonSize(const nlohmann::json &j, const char *key, const size_t fallback) {
	if (!j.contains(key) || j.at(key).is_null()) {
		return fallback;
	}
	// JSON の整数は number_integer として入ることが多い
	return static_cast<size_t>(j.at(key).get<std::int64_t>());
}

// j.value() 経由だと nlohmann 標準の enum（数値）変換が選ばれるため ADL で読む
template <typename EnumT>
void AssignEnumIfPresent(const nlohmann::json &j, const char *key, EnumT &out) {
	if (!j.contains(key) || j.at(key).is_null()) {
		return;
	}
	from_json(j.at(key), out);
}

} // namespace

	NLOHMANN_JSON_SERIALIZE_ENUM(EmitMode, { { EmitMode::BURST, "BURST" },
		{ EmitMode::CONTINUOUS, "CONTINUOUS" } });

	NLOHMANN_JSON_SERIALIZE_ENUM(EmitShape, { { EmitShape::POINT, "POINT" },
		{ EmitShape::CIRCLE_XZ, "CIRCLE_XZ" },
		{ EmitShape::CIRCLE_XY, "CIRCLE_XY" },
		{ EmitShape::CIRCLE_YZ, "CIRCLE_YZ" },
		{ EmitShape::AABB, "AABB" },
		{ EmitShape::SPHERE, "SPHERE" } });

	NLOHMANN_JSON_SERIALIZE_ENUM(MovementMode,
		{ { MovementMode::DIRECTIONAL, "DIRECTIONAL" },
		{ MovementMode::RADIAL_CONVERGE, "RADIAL_CONVERGE" },
		{ MovementMode::RADIAL_SPREAD, "RADIAL_SPREAD" } });

	NLOHMANN_JSON_SERIALIZE_ENUM(KillFieldMode, { { KillFieldMode::OUTSIDE, "OUTSIDE" },
		{ KillFieldMode::INSIDE, "INSIDE" } });

	NLOHMANN_JSON_SERIALIZE_ENUM(KillFieldShape, { { KillFieldShape::SPHERE, "SPHERE" },
		{ KillFieldShape::AABB, "AABB" } });


	void to_json(nlohmann::json& j, const KillFieldParam& killFieldParam) {

		j = nlohmann::json{
			{ "isUse", killFieldParam.isUse },
			{ "mode", killFieldParam.mode },
			{ "shape", killFieldParam.shape },
			{ "localSphere", killFieldParam.localSphere },
			{ "localAabb", killFieldParam.localAabb },
			{ "simulationSpaceFromEmitter", killFieldParam.simulationSpaceFromEmitter },
			{ "position", killFieldParam.position } };
	}

	void from_json(const nlohmann::json& j, KillFieldParam& killFieldParam) {
		killFieldParam.isUse = j.value("isUse", killFieldParam.isUse);
		AssignEnumIfPresent(j, "mode", killFieldParam.mode);
		AssignEnumIfPresent(j, "shape", killFieldParam.shape);
		killFieldParam.localSphere =
			j.value("localSphere", killFieldParam.localSphere);
		killFieldParam.localAabb = j.value("localAabb", killFieldParam.localAabb);
		AssignEnumIfPresent(j, "simulationSpaceFromEmitter",
		                    killFieldParam.simulationSpaceFromEmitter);
		killFieldParam.position = j.value("position", killFieldParam.position);
	}


	void to_json(nlohmann::json& j, const EmitCircle& emitCircle) {
		j = nlohmann::json{ { "center", emitCircle.center },
						   { "radius", emitCircle.radius } };
	}

	void from_json(const nlohmann::json& j, EmitCircle& emitCircle) {
		emitCircle.center = j.value("center", emitCircle.center);
		emitCircle.radius = j.value("radius", emitCircle.radius);
	}

	void to_json(nlohmann::json& j, const Param& param) {
		j = nlohmann::json{
			{ "simulationSpaceFromEmitter", param.simulationSpaceFromEmitter },
			{ "emitMode", param.emitMode },
			{ "emitShape", param.emitShape },
			{ "emitSphere", param.emitSphere },
			{ "emitAabb", param.emitAabb },
			{ "emitCircle", param.emitCircle },
			{ "emitPointPos", param.emitPointPos },
			{ "movementMode", param.movementMode },
			{ "emitRate", param.emitRate },
			{ "burstCount", param.burstCount },
			{ "isLooping", param.isLooping },
			{ "duration", param.duration },
			{ "minLifeDuration", param.minLifeDuration },
			{ "maxLifeDuration", param.maxLifeDuration },
			{ "baseVelocity", param.baseVelocity },
			{ "velocityRandom", param.velocityRandom },
			{ "acceleration", param.acceleration },
			{ "damping", param.damping },
			{ "startScale", param.startScale },
			{ "endScale", param.endScale },
			{ "startColor", param.startColor },
			{ "endColor", param.endColor },
			{ "killFieldParam", param.killFieldParam },
			{ "maxCount", param.maxCount },
			{ "objDirectoryPath", param.objDirectoryPath },
			{ "objFileName", param.objFileName },
			{ "mtlDirectoryPath", param.mtlDirectoryPath },
			{ "mtlFileName", param.mtlFileName } };
	}

	void from_json(const nlohmann::json& j, Param& param) {

		AssignEnumIfPresent(j, "simulationSpaceFromEmitter",
		                    param.simulationSpaceFromEmitter);
		AssignEnumIfPresent(j, "emitMode", param.emitMode);
		AssignEnumIfPresent(j, "emitShape", param.emitShape);
		param.emitSphere = j.value("emitSphere", param.emitSphere);
		param.emitAabb = j.value("emitAabb", param.emitAabb);
		param.emitCircle = j.value("emitCircle", param.emitCircle);
		param.emitPointPos = j.value("emitPointPos", param.emitPointPos);
		AssignEnumIfPresent(j, "movementMode", param.movementMode);
		param.emitRate = j.value("emitRate", param.emitRate);
		param.burstCount = ReadJsonSize(j, "burstCount", param.burstCount);
		param.isLooping = j.value("isLooping", param.isLooping);
		param.duration = j.value("duration", param.duration);
		param.minLifeDuration = j.value("minLifeDuration", param.minLifeDuration);
		param.maxLifeDuration = j.value("maxLifeDuration", param.maxLifeDuration);
		param.baseVelocity = j.value("baseVelocity", param.baseVelocity);
		param.velocityRandom = j.value("velocityRandom", param.velocityRandom);
		param.acceleration = j.value("acceleration", param.acceleration);
		param.damping = j.value("damping", param.damping);
		param.startScale = j.value("startScale", param.startScale);
		param.endScale = j.value("endScale", param.endScale);
		param.startColor = j.value("startColor", param.startColor);
		param.endColor = j.value("endColor", param.endColor);
		param.killFieldParam = j.value("killFieldParam", param.killFieldParam);
		param.maxCount = ReadJsonSize(j, "maxCount", param.maxCount);
		param.objDirectoryPath = j.value("objDirectoryPath", param.objDirectoryPath);
		param.objFileName = j.value("objFileName", param.objFileName);
		param.mtlDirectoryPath = j.value("mtlDirectoryPath", param.mtlDirectoryPath);
		param.mtlFileName = j.value("mtlFileName", param.mtlFileName);
	}

} // namespace Atrum::Visual::Particle