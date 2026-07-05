
#include <vector>
#include <algorithm>
#include "HitMesh.h"

struct Triangle {
    Vector3 v0, v1, v2;
    Vector3 normal; // 事前に法線を計算しておくこと
};

void ResolveCollision(HitMesh& body, const std::vector<Triangle>& terrain) {
    Vector3 bestNormal{};
    float maxDepth = -1.0f;
    Vector3 contactPoint{};

    // 1. 衝突判定 (全三角形に対してループ)
    for (const auto& tri : terrain) {
        Vector3 N = VectorNormalize(tri.normal);
        Vector3 P0 = tri.v0;

        for (const auto& v : body.localVertices) {
            Vector3 worldV = VectorTransform(v, body.worldMatrix);
            float dist = VectorDot(N, worldV - P0);

            if (dist < 0.0f && -dist > maxDepth) {
                maxDepth = -dist;
                bestNormal = N;
                contactPoint = worldV; // 衝突点
            }
        }
    }

    // 衝突がある場合
    if (maxDepth > 0.0001f) {
        // A. 位置の押し戻し（貫通分を解消）
        body.position = body.position + bestNormal * maxDepth;

        // B. 速度の反射 (反発係数 e を考慮)
        float e = 0.3f; // 反発係数（跳ね返りの強さ）
        float vn = VectorDot(body.velocity, bestNormal);
        if (vn < 0.0f) { // すでに離れる方向なら無視
            Vector3 impulse = bestNormal * -(1.0f + e) * vn;
            body.velocity += impulse;
        }

        // C. 回転の付加 (接触点から中心へのベクトルを使って角速度を変化)
        Vector3 r = contactPoint - body.position;
        Vector3 torque = VectorCross(r, bestNormal); // 簡略化トルク
        body.angularVelocity += torque;

    }

}