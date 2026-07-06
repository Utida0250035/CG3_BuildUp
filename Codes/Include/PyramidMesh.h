#pragma once

#include "CollisionTypes.h"
#include "Vector3.h"

#include <vector>

struct PyramidVertex
{
    Vector3 position;
    Vector3 normal;
};

struct PyramidMesh
{
    //--------------------------------------------
    // SAT用頂点（重複なし）
    //--------------------------------------------

    std::vector<Vector3> collisionVertices =
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

    //--------------------------------------------
    // 面生成
    //--------------------------------------------

    void AddFace(
        uint32_t i0,
        uint32_t i1,
        uint32_t i2)
    {
        Vector3 v0 = collisionVertices[i0];
        Vector3 v1 = collisionVertices[i1];
        Vector3 v2 = collisionVertices[i2];

        Vector3 normal =
            VectorNormalize(
                VectorCross(
                v1 - v0,
                v2 - v0));

        Face face;

        face.indices =
        {
            i0,
            i1,
            i2
        };

        face.normal = normal;

        faces.push_back(face);
    }

    //--------------------------------------------
    // SAT面生成
    //--------------------------------------------

    void BuildFaces()
    {
        faces.clear();

        // 底面
        AddFace(0, 2, 1);
        AddFace(0, 3, 2);

        // 側面
        AddFace(0, 1, 4);
        AddFace(1, 2, 4);
        AddFace(2, 3, 4);
        AddFace(3, 0, 4);
    }

    //--------------------------------------------
    // 描画頂点生成
    //--------------------------------------------

    void BuildRenderVertices()
    {
        renderVertices.clear();

        for (const Face& face : faces)
        {
            renderVertices.push_back(
                {
                    collisionVertices[face.indices[0]],
                    face.normal
                });

            renderVertices.push_back(
                {
                    collisionVertices[face.indices[1]],
                    face.normal
                });

            renderVertices.push_back(
                {
                    collisionVertices[face.indices[2]],
                    face.normal
                });
        }
    }

};