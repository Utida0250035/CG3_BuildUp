#pragma once

#include "Vector3.h"
#include <vector>

struct PyramidVertex {
	Vector3 position;
	Vector3 normal;
};

struct PyramidMesh {
	std::vector<PyramidVertex> vertices{};

	PyramidMesh() {

		auto AddTriangle = [&](const Vector3& v0,
			const Vector3& v1,
			const Vector3& v2) {

			Vector3 normal =
				VectorNormalize(VectorCross(v1 - v0, v2 - v0));

			vertices.push_back({ v0, normal });
			vertices.push_back({ v1, normal });
			vertices.push_back({ v2, normal });
		};

		// 底面
		AddTriangle(
			{ -1.0f,-1.0f,0.0f },
			{ 1.0f, 1.0f,0.0f },
			{ 1.0f,-1.0f,0.0f }
		);

		AddTriangle(
			{ -1.0f,-1.0f,0.0f },
			{ -0.5f,0.5f,0.0f },
			{ 1.0f, 1.0f, 0.0f }
		);

		// 側面
		AddTriangle(
			{ -1.0f,-1.0f,0.0f },
			{ 1.0f,-1.0f,0.0f },
			{ 0.0f,0.0f,2.0f }
		);

		AddTriangle(
			{ 1.0f,-1.0f,0.0f },
			{ 1.0f, 1.0f,0.0f },
			{ 0.0f,0.0f,2.0f }
		);

		AddTriangle(
			{ 1.0f, 1.0f,0.0f },
			{ -0.5f,0.5f,0.0f },
			{ 0.0f,0.0f,2.0f }
		);

		AddTriangle(
			{ -0.5f,0.5f,0.0f },
			{ -1.0f,-1.0f,0.0f },
			{ 0.0f,0.0f,2.0f }
		);

	}

};