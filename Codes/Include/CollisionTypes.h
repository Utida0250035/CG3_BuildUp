#pragma once

#include <cfloat>
#include <vector>

#include "Vector3.h"

struct Edge {
    int start = 0;
    int end = 0;
};

struct Face {
    std::vector<uint32_t> indices;
};

struct Projection {
    float min = 0.0f;
    float max = 0.0f;
};

struct SATResult {
    bool hit = false;

    Vector3 normal{};
    float depth = FLT_MAX;

    // 旧1点接触。互換用に残す。
    Vector3 contactPoint{};

    // 新規：複数接触点
    std::vector<Vector3> contactPoints;
};

struct ContactPoint {
    Vector3 position{};
    float penetration = 0.0f;
};

struct CollisionInfo {
    bool hit = false;
    Vector3 normal{};
    float depth = 0.0f;
    std::vector<Vector3> contactPoints;
};

struct AABB {
    Vector3 min{};
    Vector3 max{};
};

struct OBB {
    Vector3 center{};
    Vector3 axis[3]{};
    Vector3 halfSize{};
};

struct Sphere {
    Vector3 center{};
    float radius = 1.0f;
};