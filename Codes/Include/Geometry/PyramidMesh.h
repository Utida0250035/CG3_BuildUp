#pragma once

#include "Geometry/GeometryTypes.h"
#include "Math/Vector3.h"
#include <vector>

namespace Atrum::Geometry {

	struct PyramidVertex
	{
		Math::Vector3 position;
		Math::Vector3 normal;
	};

	struct PyramidMesh
	{
		//--------------------------------------------
		// SAT用頂点（重複なし）
		//--------------------------------------------

		std::vector<Math::Vector3> collisionVertices =
		{
			{-1.0f,-1.0f,0.0f},     //0
			{ 1.0f,-1.0f,0.0f},     //1
			{ 1.0f, 1.0f,0.0f},     //2
			{-0.5f,0.5f,0.0f},      //3
			{ 0.0f,0.0f,2.0f}      //4
		};

		//--------------------------------------------
		// SAT用辺
		//--------------------------------------------

		std::vector<Edge> edges =
		{
			{0,1},
			{1,2},
			{2,3},
			{3,0},

			{0,4},
			{1,4},
			{2,4},
			{3,4}
		};

		//--------------------------------------------
		// SAT用面
		//--------------------------------------------

		std::vector<Face> faces;

		//--------------------------------------------
		// 描画用
		//--------------------------------------------

		std::vector<PyramidVertex> renderVertices;

		PyramidMesh()
		{
			BuildFaces();
			BuildRenderVertices();
		}

	private:

		void AddFace(
			uint32_t i0,
			uint32_t i1,
			uint32_t i2)
		{
			Face face;

			face.indices =
			{
				i0,
				i1,
				i2
			};

			faces.push_back(face);
		}

		void BuildFaces()
		{
			faces.clear();

			AddFace(0, 2, 1);
			AddFace(0, 3, 2);

			AddFace(0, 1, 4);
			AddFace(1, 2, 4);
			AddFace(2, 3, 4);
			AddFace(3, 0, 4);
		}

		void BuildRenderVertices()
		{
			renderVertices.clear();

			for (const Face& face : faces)
			{
				if (face.indices.size() < 3)
				{
					continue;
				}

				uint32_t i0 = face.indices[0];
				uint32_t i1 = face.indices[1];
				uint32_t i2 = face.indices[2];

				Math::Vector3 v0 = collisionVertices[i0];
				Math::Vector3 v1 = collisionVertices[i1];
				Math::Vector3 v2 = collisionVertices[i2];

				Math::Vector3 normal = (v1 - v0).Cross(v2 - v0);

				if (normal.Length() <= 0.000001f) {
					normal = { 0.0f, 1.0f, 0.0f };
				} else {
					normal.Normalize();
				}

				renderVertices.push_back({ collisionVertices[i0], normal });
				renderVertices.push_back({ collisionVertices[i1], normal });
				renderVertices.push_back({ collisionVertices[i2], normal });
			}
		}

	};

}