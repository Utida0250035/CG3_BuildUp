#include "SAT.h"

#include <algorithm>
#include <cfloat>
#include <cmath>


constexpr float kAxisEpsilon = 0.000001f;

namespace {

	//constexpr float kAxisEpsilon = 0.000001f;
	constexpr float kSameAxisDot = 0.999f;

	bool IsValidAxis(const Vector3& axis) {

		return VectorDot(axis, axis) > kAxisEpsilon;

	}

	Vector3 SafeNormalize(const Vector3& axis) {

		if (!IsValidAxis(axis)) {
			return {};
		}

		return VectorNormalize(axis);

	}

	bool IsSameAxis(const Vector3& a, const Vector3& b) {

		float dot = VectorDot(a, b);

		return std::fabs(dot) > kSameAxisDot;

	}

	void AddUniqueAxis(
		std::vector<Vector3>& axes,
		const Vector3& axis) {

		Vector3 n = SafeNormalize(axis);

		if (!IsValidAxis(n)) {
			return;
		}

		for (const Vector3& existing : axes) {

			if (IsSameAxis(existing, n)) {
				return;
			}

		}

		axes.push_back(n);

	}

}

Projection ProjectVertices(
	const std::vector<Vector3>& vertices,
	const Vector3& axis) {

	Projection result{};
	result.min = FLT_MAX;
	result.max = -FLT_MAX;

	for (const Vector3& v : vertices) {

		float p = VectorDot(v, axis);

		result.min = std::min(result.min, p);
		result.max = std::max(result.max, p);

	}

	return result;

}

float Overlap(
	const Projection& a,
	const Projection& b) {

	return std::min(a.max, b.max) - std::max(a.min, b.min);

}

std::vector<Vector3> GetFaceAxes(
	const HitMesh& mesh) {

	std::vector<Vector3> axes{};
	axes.reserve(mesh.faces.size());

	for (uint32_t i = 0; i < mesh.faces.size(); ++i) {

		Vector3 axis = mesh.GetFaceNormal(i);
		axis = SafeNormalize(axis);

		if (IsValidAxis(axis)) {
			axes.push_back(axis);
		}

	}

	return axes;

}

std::vector<Vector3> GetEdgeAxes(
	const HitMesh& meshA,
	const HitMesh& meshB) {

	std::vector<Vector3> axes{};

	axes.reserve(meshA.edges.size() * meshB.edges.size());

	for (const Edge& edgeA : meshA.edges) {

		Vector3 a0 = meshA.worldVertices[edgeA.v0];
		Vector3 a1 = meshA.worldVertices[edgeA.v1];

		Vector3 dirA = a1 - a0;

		for (const Edge& edgeB : meshB.edges) {

			Vector3 b0 = meshB.worldVertices[edgeB.v0];
			Vector3 b1 = meshB.worldVertices[edgeB.v1];

			Vector3 dirB = b1 - b0;

			Vector3 axis = VectorCross(dirA, dirB);
			axis = SafeNormalize(axis);

			if (IsValidAxis(axis)) {
				axes.push_back(axis);
			}

		}

	}

	return axes;

}

std::vector<Vector3> GetAxes(
	const HitMesh& meshA,
	const HitMesh& meshB) {

	std::vector<Vector3> axes{};

	for (uint32_t i = 0; i < meshA.faces.size(); ++i) {
		AddUniqueAxis(axes, meshA.GetFaceNormal(i));
	}

	for (uint32_t i = 0; i < meshB.faces.size(); ++i) {
		AddUniqueAxis(axes, meshB.GetFaceNormal(i));
	}

	for (const Edge& edgeA : meshA.edges) {

		Vector3 a0 = meshA.worldVertices[edgeA.v0];
		Vector3 a1 = meshA.worldVertices[edgeA.v1];
		Vector3 dirA = a1 - a0;

		for (const Edge& edgeB : meshB.edges) {

			Vector3 b0 = meshB.worldVertices[edgeB.v0];
			Vector3 b1 = meshB.worldVertices[edgeB.v1];
			Vector3 dirB = b1 - b0;

			AddUniqueAxis(axes, VectorCross(dirA, dirB));
		}
	}

	return axes;
}

//std::vector<Vector3> GetAxes(
//	const HitMesh& meshA,
//	const HitMesh& meshB) {
//
//	std::vector<Vector3> axes{};
//
//	for (uint32_t i = 0; i < meshA.faces.size(); ++i) {
//		AddUniqueAxis(axes, meshA.GetFaceNormal(i));
//	}
//
//	for (uint32_t i = 0; i < meshB.faces.size(); ++i) {
//		AddUniqueAxis(axes, meshB.GetFaceNormal(i));
//	}
//
//	return axes;
//}

SATResult TestSAT(
	const HitMesh& meshA,
	const HitMesh& meshB) {

	SATResult result{};
	result.isHit = false;
	result.depth = FLT_MAX;

	std::vector<Vector3> axes = GetAxes(meshA, meshB);

	if (axes.empty()) {
		return result;
	}

	Vector3 centerA = meshA.GetCenter();
	Vector3 centerB = meshB.GetCenter();

	for (Vector3 axis : axes) {

		axis = SafeNormalize(axis);

		if (!IsValidAxis(axis)) {
			continue;
		}

		Projection projectionA =
			ProjectVertices(meshA.worldVertices, axis);

		Projection projectionB =
			ProjectVertices(meshB.worldVertices, axis);

		float depth = Overlap(projectionA, projectionB);

		if (depth <= 0.0f) {

			result.isHit = false;
			result.depth = 0.0f;
			result.normal = {};

			return result;

		}

		if (depth < result.depth) {

			result.depth = depth;
			result.normal = axis;

		}

	}

	Vector3 direction = centerA - centerB;

	if (VectorDot(direction, result.normal) < 0.0f) {
		result.normal = result.normal * -1.0f;
	}

	result.isHit = true;

	Vector3 supportA = meshA.GetSupportPoint(result.normal * -1.0f);
	Vector3 supportB = meshB.GetSupportPoint(result.normal);

	//result.contactPoint = (supportA + supportB) * 0.5f;

	float minDot = FLT_MAX;
	Vector3 contactPoint{};

	for (const Vector3& v : meshB.worldVertices) {

		float d = VectorDot(v, result.normal);

		if (d < minDot) {
			minDot = d;
			contactPoint = v;
		}
	}

	result.contactPoint = contactPoint;

	return result;

}