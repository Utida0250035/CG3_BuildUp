#pragma once
#include "Matrix3D.h"
#include "PyramidMesh.h"
#include "Vector3.h"
#include <vector>

inline Vector3 CalculateCentroid(const std::vector<Vector3>& vertices) {
    Vector3 sum = { 0.0f, 0.0f, 0.0f };
    for (const auto& v : vertices) {
        sum.x += v.x;
        sum.y += v.y;
        sum.z += v.z;
    }
    float count = static_cast<float>(vertices.size());
    return { sum.x / count, sum.y / count, sum.z / count };
}

// 物理演算用ボディ構造体
struct HitMesh {
    // 形状データ（凸包の頂点群）
    // ※元のモデルの頂点から「凸包アルゴリズム(QuickHull等)」で生成したものを持つ
    std::vector<Vector3> localVertices{};

    // 現在の状態
    Vector3 position{};    // ワールド位置
    Vector3 rotation{};    // クォータニオン回転
    Matrix4x4 worldMatrix{}; // ワールド行列 (位置・回転から算出)

    // 速度と角速度（物理応答用）
    Vector3 velocity{};
    Vector3 angularVelocity{};

    // ヘルパー：ワールド空間での頂点取得（サポート関数用）
    Vector3 GetWorldVertex(int index) const {
        return VectorTransform(localVertices[index], worldMatrix);
    }

    void Update(const float deltaTime) {

        position += velocity * deltaTime;
        rotation += angularVelocity * deltaTime;

        UpdateMatrix();

    }

    // 行列の更新
    void UpdateMatrix() {
        worldMatrix = MakeScaleMatrix({ 1.0f, 1.0f, 1.0f }) * MakeRotateMatrix(rotation) * MakeTranslateMatrix(position);
    }

    // ある方向 d に対して最も遠い頂点を探す（サポート関数）
    Vector3 GetSupportPoint(Vector3 direction) const {
        // ローカル空間での方向を計算（行列の逆変換を使用）
        Matrix4x4 invWorld = MatrixInverse(worldMatrix);
        Vector3 localDir = VectorTransform(direction, invWorld);

        float maxDot = -FLT_MAX;
        int bestIndex = 0;

        for (size_t i = 0; i < localVertices.size(); ++i) {
            float dot = VectorDot(localVertices[i], localDir);
            if (dot > maxDot) {
                maxDot = dot;
                bestIndex = (int)i;
            }
        }

        // ワールド空間の頂点を返す
        return GetWorldVertex(bestIndex);
    }

    void CenterMesh() {
        Vector3 centroid = CalculateCentroid(localVertices);

        for (auto& v : localVertices) {
            v.x -= centroid.x;
            v.y -= centroid.y;
            v.z -= centroid.z;
        }
    }

};

inline HitMesh CreateAsymmetricPyramid() {
    PyramidMesh pyramid{};
    HitMesh hm{};

    for (const auto& vertex : pyramid.vertices) {

        hm.localVertices.push_back(vertex.position);

    }

    // 初期状態の設定
    hm.position = {};
    hm.rotation = {};
    hm.velocity = {};
    hm.angularVelocity = {};
    hm.UpdateMatrix();

    return hm;
}

// 既存のMeshデータ（パース済み）から HitMesh を初期化する
inline HitMesh CreateHitMeshFromObj(const std::vector<Vector3>& rawVertices) {
    HitMesh hm{};

    // ここで凸包アルゴリズムまたは間引き処理を行う
    // 今日中に終わらせるなら、とりあえず頂点をコピーするだけでもOK
    // ただし、頂点数が多い場合は「数点おきに間引く」などの工夫を推奨
    for (size_t i = 0; i < rawVertices.size(); i += 4) { // 簡易的な間引き例
        hm.localVertices.push_back(rawVertices[i]);
    }

    // 初期状態の設定
    hm.position = Vector3(0.0f, 0.0f, 0.0f);
    hm.rotation = {};
    hm.velocity = {};
    hm.angularVelocity = {};
    hm.UpdateMatrix();

    return hm;
}