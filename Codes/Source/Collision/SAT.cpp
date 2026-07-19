#include "Engine/Alias/PhysicsAlias.h"

#include "Collision/SAT.h"
#include <algorithm>
#include <cfloat>
#include <cmath>

namespace {

	namespace M = Atrum::Math;
	namespace P = Atrum::Physics;
	namespace G = Atrum::Geometry;

	constexpr float kAxisEpsilon = 0.000001f;
	constexpr float kContactTolerance = 0.05f;

	bool IsValidAxis(const M::Vector3& axis) {

		return axis.Length() > kAxisEpsilon;
	}

	bool ContainsNearPoint(
		const std::vector<P::ContactPoint>& contacts,
		const M::Vector3& point,
		float epsilon) {

		for (const P::ContactPoint& contact : contacts) {

			if ((contact.position - point).Length() <= epsilon) {
				return true;
			}
		}

		return false;
	}

	void AddUniquePoint(
		std::vector<P::ContactPoint>& contacts,
		const M::Vector3& point,
		float penetration,
		float epsilon) {

		if (ContainsNearPoint(contacts, point, epsilon)) {
			return;
		}

		P::ContactPoint contact{};
		contact.position = point;
		contact.penetration = penetration;

		contacts.push_back(contact);
	}

	M::Vector3 CalculateAveragePoint(
		const std::vector<P::ContactPoint>& contacts) {

		if (contacts.empty()) {
			return {};
		}

		M::Vector3 result{};

		for (const P::ContactPoint& contact : contacts) {
			result += contact.position;
		}

		result /= static_cast<float>(contacts.size());

		return result;
	}

	struct FaceQuery {
		int index = -1;
		M::Vector3 normal{};
		M::Vector3 center{};
		float dot = -FLT_MAX;
	};

	struct ReferenceFace {
		const P::HitMesh* mesh = nullptr;
		int faceIndex = -1;
		M::Vector3 normal{};
		M::Vector3 center{};
		bool referenceIsA = true;
	};

	M::Vector3 CalculateFaceCenter(
		const P::HitMesh& mesh,
		const G::Face& face) {

		M::Vector3 center{};

		if (face.indices.empty()) {
			return center;
		}

		for (uint32_t index : face.indices) {

			if (index >= mesh.worldVertices.size()) {
				continue;
			}

			center += mesh.worldVertices[index];
		}

		center /= static_cast<float>(face.indices.size());

		return center;
	}

	M::Vector3 CalculateOutwardFaceNormal(
		const P::HitMesh& mesh,
		const G::Face& face) {

		if (face.indices.size() < 3) {
			return {};
		}

		uint32_t i0 = face.indices[0];
		uint32_t i1 = face.indices[1];
		uint32_t i2 = face.indices[2];

		if (i0 >= mesh.worldVertices.size() ||
			i1 >= mesh.worldVertices.size() ||
			i2 >= mesh.worldVertices.size()) {
			return {};
		}

		const M::Vector3& v0 = mesh.worldVertices[i0];
		const M::Vector3& v1 = mesh.worldVertices[i1];
		const M::Vector3& v2 = mesh.worldVertices[i2];

		M::Vector3 normal = (v1 - v0).Cross(v2 - v0);

		if (!IsValidAxis(normal)) {
			return {};
		}

		normal = normal.Normalized();

		M::Vector3 faceCenter =
			CalculateFaceCenter(mesh, face);

		M::Vector3 meshCenter =
			mesh.GetCenter();

		M::Vector3 outwardDirection =
			faceCenter - meshCenter;

		if (normal.Dot(outwardDirection) < 0.0f) {
			normal = -normal;
		}

		return normal;
	}

	FaceQuery FindMostAlignedFace(
		const P::HitMesh& mesh,
		const M::Vector3& direction) {

		FaceQuery result{};

		M::Vector3 dir = direction.Normalized();

		for (size_t i = 0; i < mesh.faces.size(); ++i) {

			const G::Face& face = mesh.faces[i];

			M::Vector3 normal =
				CalculateOutwardFaceNormal(mesh, face);

			if (!IsValidAxis(normal)) {
				continue;
			}

			float d = normal.Dot(dir);

			if (d > result.dot) {

				result.index = static_cast<int>(i);
				result.normal = normal;
				result.center = CalculateFaceCenter(mesh, face);
				result.dot = d;
			}
		}

		return result;
	}

	FaceQuery FindMostAntiParallelFace(
		const P::HitMesh& mesh,
		const M::Vector3& referenceNormal) {

		FaceQuery result{};
		result.dot = FLT_MAX;

		M::Vector3 refN = referenceNormal.Normalized();

		for (size_t i = 0; i < mesh.faces.size(); ++i) {

			const G::Face& face = mesh.faces[i];

			M::Vector3 normal = CalculateOutwardFaceNormal(mesh, face);

			if (!IsValidAxis(normal)) {
				continue;
			}

			float d = normal.Dot(refN);

			if (d < result.dot) {

				result.index = static_cast<int>(i);
				result.normal = normal;
				result.center = CalculateFaceCenter(mesh, face);
				result.dot = d;
			}
		}

		return result;
	}

	std::vector<M::Vector3> GetFaceVertices(
		const P::HitMesh& mesh,
		const G::Face& face) {

		std::vector<M::Vector3> vertices;

		for (uint32_t index : face.indices) {

			if (index >= mesh.worldVertices.size()) {
				continue;
			}

			vertices.push_back(mesh.worldVertices[index]);
		}

		return vertices;
	}

	std::vector<M::Vector3> ClipPolygonByPlane(
		const std::vector<M::Vector3>& polygon,
		const M::Vector3& planePoint,
		const M::Vector3& planeNormal) {

		std::vector<M::Vector3> result;

		if (polygon.empty()) {
			return result;
		}

		constexpr float kPlaneEpsilon = 0.0001f;

		for (size_t i = 0; i < polygon.size(); ++i) {

			const M::Vector3& current = polygon[i];

			const M::Vector3& next = polygon[(i + 1) % polygon.size()];

			float currentDistance = (current - planePoint).Dot(planeNormal);

			float nextDistance = (next - planePoint).Dot(planeNormal);

			bool currentInside = currentDistance <= kPlaneEpsilon;

			bool nextInside = nextDistance <= kPlaneEpsilon;

			if (currentInside && nextInside) {

				result.push_back(next);

			} else if (currentInside && !nextInside) {

				float t =
					currentDistance /
					(currentDistance - nextDistance);

				M::Vector3 intersection =
					current + (next - current) * t;

				result.push_back(intersection);

			} else if (!currentInside && nextInside) {

				float t =
					currentDistance /
					(currentDistance - nextDistance);

				M::Vector3 intersection =
					current + (next - current) * t;

				result.push_back(intersection);
				result.push_back(next);
			}
		}

		return result;
	}

	std::vector<M::Vector3> ClipIncidentFaceByReferenceFace(
		const P::HitMesh& referenceMesh,
		const G::Face& referenceFace,
		const M::Vector3& referenceNormal,
		const std::vector<M::Vector3>& incidentPolygon) {

		std::vector<M::Vector3> clipped =
			incidentPolygon;

		if (referenceFace.indices.size() < 3) {
			return clipped;
		}

		M::Vector3 referenceCenter =
			CalculateFaceCenter(referenceMesh, referenceFace);

		for (size_t i = 0; i < referenceFace.indices.size(); ++i) {

			uint32_t index0 =
				referenceFace.indices[i];

			uint32_t index1 =
				referenceFace.indices[(i + 1) % referenceFace.indices.size()];

			if (index0 >= referenceMesh.worldVertices.size() ||
				index1 >= referenceMesh.worldVertices.size()) {
				continue;
			}

			M::Vector3 v0 =
				referenceMesh.worldVertices[index0];

			M::Vector3 v1 =
				referenceMesh.worldVertices[index1];

			M::Vector3 edge =
				v1 - v0;

			if (!IsValidAxis(edge)) {
				continue;
			}

			M::Vector3 sideNormal =
				edge.Cross(referenceNormal);

			if (!IsValidAxis(sideNormal)) {
				continue;
			}

			sideNormal.Normalize();

			float centerSide = (referenceCenter - v0).Dot(sideNormal);

			if (centerSide > 0.0f) {
				sideNormal = -sideNormal;
			}

			clipped =
				ClipPolygonByPlane(
					clipped,
					v0,
					sideNormal);

			if (clipped.empty()) {
				break;
			}
		}

		return clipped;
	}

} // namespace

namespace Atrum::Physics::SAT {

	G::Projection ProjectVertices(
		const std::vector<M::Vector3>& vertices,
		const M::Vector3& axis) {

		G::Projection result{};

		if (vertices.empty()) {
			return result;
		}

		float first = vertices[0].Dot(axis);

		result.min = first;
		result.max = first;

		for (size_t i = 1; i < vertices.size(); ++i) {

			float projection = vertices[i].Dot(axis);

			result.min = std::min(result.min, projection);
			result.max = std::max(result.max, projection);

		}

		return result;
	}

	float Overlap(
		const G::Projection& a,
		const G::Projection& b) {

		return std::min(a.max, b.max) - std::max(a.min, b.min);
	}

	std::vector<M::Vector3> GetFaceAxes(
		const P::HitMesh& mesh) {

		std::vector<M::Vector3> axes;

		for (size_t i = 0; i < mesh.faces.size(); ++i) {

			M::Vector3 axis = mesh.GetFaceNormal(static_cast<int>(i));

			if (!IsValidAxis(axis)) {
				continue;
			}

			axis.Normalize();

			axes.push_back(axis);
		}

		return axes;
	}

	std::vector<M::Vector3> GetEdgeAxes(
		const P::HitMesh& a,
		const P::HitMesh& b) {

		std::vector<M::Vector3> axes;

		for (const G::Edge& edgeA : a.edges) {

			M::Vector3 a0 = a.worldVertices[edgeA.start];
			M::Vector3 a1 = a.worldVertices[edgeA.end];

			M::Vector3 dirA = a1 - a0;

			if (!IsValidAxis(dirA)) {
				continue;
			}

			dirA.Normalize();

			for (const G::Edge& edgeB : b.edges) {

				M::Vector3 b0 = b.worldVertices[edgeB.start];
				M::Vector3 b1 = b.worldVertices[edgeB.end];

				M::Vector3 dirB = b1 - b0;

				if (!IsValidAxis(dirB)) {
					continue;
				}

				dirB.Normalize();

				M::Vector3 axis = dirA.Cross(dirB);

				if (!IsValidAxis(axis)) {
					continue;
				}

				axis.Normalize();

				axes.push_back(axis);
			}
		}

		return axes;
	}

	std::vector<M::Vector3> GetAxes(
		const P::HitMesh& a,
		const P::HitMesh& b) {

		std::vector<M::Vector3> axes;

		std::vector<M::Vector3> faceAxesA = GetFaceAxes(a);
		std::vector<M::Vector3> faceAxesB = GetFaceAxes(b);
		std::vector<M::Vector3> edgeAxes = GetEdgeAxes(a, b);

		axes.insert(axes.end(), faceAxesA.begin(), faceAxesA.end());
		axes.insert(axes.end(), faceAxesB.begin(), faceAxesB.end());
		axes.insert(axes.end(), edgeAxes.begin(), edgeAxes.end());

		return axes;
	}

	std::vector<P::ContactPoint> GenerateContactPoints(
		const P::HitMesh& bodyA,
		const P::HitMesh& bodyB,
		const M::Vector3& normal,
		float depth) {

		std::vector<P::ContactPoint> contacts;

		if (!IsValidAxis(normal)) {
			return contacts;
		}

		M::Vector3 n = normal.Normalized();

		FaceQuery faceA =
			FindMostAlignedFace(bodyA, n);

		FaceQuery faceB =
			FindMostAlignedFace(bodyB, -n);

		if (faceA.index < 0 || faceB.index < 0) {
			return contacts;
		}

		ReferenceFace reference{};

		const P::HitMesh* incidentMesh = nullptr;
		int incidentFaceIndex = -1;

		if (faceA.dot >= faceB.dot) {

			reference.mesh = &bodyA;
			reference.faceIndex = faceA.index;
			reference.normal = faceA.normal;
			reference.center = faceA.center;
			reference.referenceIsA = true;

			incidentMesh = &bodyB;

			FaceQuery incidentFace =
				FindMostAntiParallelFace(
					bodyB,
					reference.normal);

			incidentFaceIndex = incidentFace.index;

		} else {

			reference.mesh = &bodyB;
			reference.faceIndex = faceB.index;
			reference.normal = faceB.normal;
			reference.center = faceB.center;
			reference.referenceIsA = false;

			incidentMesh = &bodyA;

			FaceQuery incidentFace =
				FindMostAntiParallelFace(
					bodyA,
					reference.normal);

			incidentFaceIndex = incidentFace.index;
		}

		if (reference.mesh == nullptr ||
			incidentMesh == nullptr ||
			reference.faceIndex < 0 ||
			incidentFaceIndex < 0) {
			return contacts;
		}

		const G::Face& referenceFace =
			reference.mesh->faces[reference.faceIndex];

		const G::Face& incidentFace =
			incidentMesh->faces[incidentFaceIndex];

		std::vector<M::Vector3> incidentPolygon =
			GetFaceVertices(*incidentMesh, incidentFace);

		if (incidentPolygon.empty()) {
			return contacts;
		}

		std::vector<M::Vector3> clipped =
			ClipIncidentFaceByReferenceFace(
				*reference.mesh,
				referenceFace,
				reference.normal,
				incidentPolygon);

		if (clipped.empty()) {
			return contacts;
		}

		constexpr float kContactSlop = 0.01f;

		float referencePlane = reference.center.Dot(reference.normal);

		for (const M::Vector3& point : clipped) {

			float distance = point.Dot(reference.normal) - referencePlane;

			if (distance <= kContactSlop + depth) {

				float pointPenetration =
					std::max(-distance, 0.0f);

				if (pointPenetration <= 0.0f) {
					continue;
				}

				M::Vector3 projectedPoint =
					point - reference.normal * distance;

				AddUniquePoint(
					contacts,
					projectedPoint,
					pointPenetration,
					0.001f);
			}
		}

		constexpr size_t kMaxContactCount = 4;

		if (contacts.size() > kMaxContactCount) {

			M::Vector3 average =
				CalculateAveragePoint(contacts);

			std::sort(
				contacts.begin(),
				contacts.end(),
				[average](const P::ContactPoint& lhs, const P::ContactPoint& rhs) {

				float dl = (lhs.position - average).Length();

				float dr = (rhs.position - average).Length();

				return dl > dr;
			});

			contacts.resize(kMaxContactCount);
		}

		if (contacts.empty()) {

			M::Vector3 supportA =
				bodyA.GetSupportPoint(n);

			M::Vector3 supportB =
				bodyB.GetSupportPoint(-n);

			AddUniquePoint(
				contacts,
				(supportA + supportB) * 0.5f,
				depth,
				0.001f);
		}

		return contacts;
	}

	P::SATResult TestSAT(
		const P::HitMesh& bodyA,
		const P::HitMesh& bodyB) {

		P::SATResult result{};

		std::vector<M::Vector3> axes = GetAxes(bodyA, bodyB);

		if (axes.empty()) {
			return result;
		}

		float minOverlap = FLT_MAX;
		M::Vector3 minAxis{};

		for (M::Vector3 axis : axes) {

			if (!IsValidAxis(axis)) {
				continue;
			}

			axis.Normalize();

			G::Projection projectionA =
				ProjectVertices(bodyA.worldVertices, axis);

			G::Projection projectionB =
				ProjectVertices(bodyB.worldVertices, axis);

			float overlap =
				Overlap(projectionA, projectionB);

			if (overlap <= 0.0f) {
				result.hit = false;
				return result;
			}

			if (overlap < minOverlap) {

				minOverlap = overlap;
				minAxis = axis;
			}
		}

		if (!IsValidAxis(minAxis)) {
			result.hit = false;
			return result;
		}

		M::Vector3 centerA = bodyA.GetCenter();
		M::Vector3 centerB = bodyB.GetCenter();

		M::Vector3 centerDirection = centerB - centerA;

		if (centerDirection.Dot(minAxis) < 0.0f) {
			minAxis = -minAxis;
		}

		result.hit = true;
		result.normal = minAxis.Normalized();

		result.contactPoints =
			GenerateContactPoints(
				bodyA,
				bodyB,
				result.normal,
				minOverlap);

		return result;
	}

}