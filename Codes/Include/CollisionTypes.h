#pragma once

#include "Vector3.h"

#include <cstdint>
#include <limits>
#include <vector>

///----------------------------------------
/// 頂点インデックスで表現する辺
///----------------------------------------
struct Edge {

    uint32_t v0 = 0;
    uint32_t v1 = 0;

};

///----------------------------------------
/// 面
/// SATでは法線軸として使用する
///----------------------------------------
struct Face {

    // 面を構成する頂点インデックス
    std::vector<uint32_t> indices{};

    // ローカル空間法線
    Vector3 normal{};

};

///----------------------------------------
/// 射影結果
///----------------------------------------
struct Projection {

    float min = std::numeric_limits<float>::max();
    float max = std::numeric_limits<float>::lowest();

};

///----------------------------------------
/// SAT判定結果
///----------------------------------------
struct SATResult {

    // 衝突したか
    bool isHit = false;

    // 最小押し戻し方向(MTV)
    Vector3 normal{};

    // めり込み量
    float depth = 0.0f;

    // 接触点
    Vector3 contactPoint{};

    // 使用した面
    uint32_t faceIndex = UINT32_MAX;

};

///----------------------------------------
/// 接触点
///----------------------------------------
struct ContactPoint {

    Vector3 position{};
    Vector3 normal{};

    float penetration = 0.0f;

};

///----------------------------------------
/// 衝突情報
///----------------------------------------
struct CollisionInfo {

    ContactPoint contact;

    SATResult sat;

};

///----------------------------------------
/// AABB
/// BroadPhase用
///----------------------------------------
struct AABB {

    Vector3 min{};
    Vector3 max{};

};

///----------------------------------------
/// OBB
/// 将来用
///----------------------------------------
struct OBB {

    Vector3 center{};

    Vector3 axis[3] = {
        {1.0f,0.0f,0.0f},
        {0.0f,1.0f,0.0f},
        {0.0f,0.0f,1.0f}
    };

    Vector3 halfSize{};

};

///----------------------------------------
/// 球
///----------------------------------------
struct Sphere {

    Vector3 center{};

    float radius = 0.0f;

};