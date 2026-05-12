#include "Collision.h"
#include <algorithm>

// OBBとベジェ曲線の交差判定（再帰）
bool CheckCollision(const OBB& obb, const Vector2 p[3], float t1, float t2, int depth, float& hitT) {
    // 現在の区間(t1-t2)における3つの点を取得
    Vector2 points[3] = { CalcBezier2(p, t1), CalcBezier2(p, (t1 + t2) * 0.5f), CalcBezier2(p, t2) };

    // OBBローカル空間でのバウンディングボックス(AABB)を計算
    float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f;
    for (int i = 0; i < 3; ++i) {
        Vector2 lp = ToLocal(points[i], obb);
        minX = std::min(minX, lp.x); maxX = std::max(maxX, lp.x);
        minY = std::min(minY, lp.y); maxY = std::max(maxY, lp.y);
    }

    // OBBの範囲（-halfSize ～ +halfSize）と重なっているか
    if (minX > obb.halfSize.x || maxX < -obb.halfSize.x ||
        minY > obb.halfSize.y || maxY < -obb.halfSize.y) {
        return false;
    }

    // 十分な精度に達したらtを確定
    if (depth > 6) {
        hitT = (t1 + t2) * 0.5f;
        return true;
    }

    // 分割して再帰探索
    float mid = (t1 + t2) * 0.5f;
    if (CheckCollision(obb, p, t1, mid, depth + 1, hitT)) {
     
        return true;

    }

    return CheckCollision(obb, p, mid, t2, depth + 1, hitT);

}

void ResolveObbBezierResponse(OBB& obb, Vector2& velocity, const Vector2 p[3]) {

    float hitT = 0.0f;

    // 1. 衝突検知
    if (CheckCollision(obb, p, 0.0f, 1.0f, 0, hitT)) {

        // 2. 衝突点（ワールド座標）とローカル座標の取得
        Vector2 worldContact = CalcBezier2(p, hitT);
        Vector2 localContact = ToLocal(worldContact, obb);

        // 3. 応答法線（OBBのどの面で当たったか）の決定
        // めり込みが最も浅い方向に押し出す
        float dx = obb.halfSize.x - std::abs(localContact.x);
        float dy = obb.halfSize.y - std::abs(localContact.y);

        Vector2 normal;
        float overlap;

        if (dx < dy) {
            normal = obb.axis[0] * (localContact.x > 0 ? 1.0f : -1.0f);
            overlap = dx;
        } else {
            normal = obb.axis[1] * (localContact.y > 0 ? 1.0f : -1.0f);
            overlap = dy;
        }

        // 4. 位置補正 (めり込み解消)
        obb.center = obb.center + normal * (overlap + 0.01f);

        // 5. 速度の反射 (物理応答)
        float restitution = 0.4f; // 跳ね返り係数
        float vn = VectorDot(velocity, normal);

        if (vn < 0) { // 法線方向（壁）に向かって進んでいる場合のみ
            velocity = velocity - normal * (vn * (1.0f + restitution));
        }

    }

}