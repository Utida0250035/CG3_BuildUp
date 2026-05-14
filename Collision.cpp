#include "Collision.h"
#include <algorithm>

// OBBとベジェ曲線の交差判定（再帰）
bool CheckCollision(const OBB& obb, const Vector2 p[3], float t1, float t2, int depth, float& hitT) {
    // 現在の区間(t1-t2)における3つの点を取得
    Vector2 points[3] = { CalcBezier2(p, t1), CalcBezier2(p, (t1 + t2) * 0.5f), CalcBezier2(p, t2) };

    // OBBローカル空間でのバウンディングボックス(AABB)を計算
    float minX = 1e9f, minY = 1e9f, maxX = -1e9f, maxY = -1e9f;
    for (int i = 0; i < 3; ++i) {
        Vector2 lp = obb.ToLocal(points[i]);
        minX = std::min(minX, lp.x); maxX = std::max(maxX, lp.x);
        minY = std::min(minY, lp.y); maxY = std::max(maxY, lp.y);
    }

    // OBBの範囲（-halfSize ～ +halfSize）と重なっているか
    if (minX > obb.halfSize.x || maxX < -obb.halfSize.x ||
        minY > obb.halfSize.y || maxY < -obb.halfSize.y) {
        return false;
    }

    // 十分な精度に達したらtを確定
    if (depth >= 8) {
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
        Vector2 localContact = obb.ToLocal(worldContact);

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

void ResolveRigidBodyObbBezierResponse(RigidBodyOBB& body, const Vector2 pBezier[3]) {

    float hitT = 0.0f;
    
    if (CheckCollision(body, pBezier, 0.0f, 1.0f, 0, hitT)) {

    // 衝突点と法線の取得
        Vector2 contactPoint = CalcBezier2(pBezier, hitT);
        Vector2 localContact = body.ToLocal(contactPoint);

        // 法線(normal)と押し出し量(overlap)を決定

        // OBBの面法線
        Vector2 normal;
        float dx = body.halfSize.x - std::abs(localContact.x);
        float dy = body.halfSize.y - std::abs(localContact.y);

        if (dx < dy) {

            normal = body.axis[0] * (localContact.x > 0 ? 1.0f : -1.0f);

        } else {

            normal = body.axis[1] * (localContact.y > 0 ? 1.0f : -1.0f);

        }

        // 物理定数の準備

        // 重心から衝突点へのベクトル
        Vector2 r = contactPoint - body.center;

        // 2Dクロス積(外積, r×n)
        float rCrossN = VectorCross(r, normal);

        // 衝突点での物体の速度 (並進速度 + 回転による速度)
        // 2Dの回転速度ベクトル: (-ω*r.y, ω*r.x)
        Vector2 velocityAtPoint = body.velocity + Vector2{ -body.angularVelocity * r.y, body.angularVelocity * r.x };

        // 3. インパルス j の計算
        float e = 0.4f; // 反発係数
        float vn = VectorDot(velocityAtPoint, normal);

        if (vn > 0.0f) {

            // 離れていく方向なら処理しない
            return;

        }

        // 剛体の衝突公式: j = -(1+e)v / (1/m + (r x n)^2 / I)
        float invMass = 1.0f / body.mass;
        float invInertia = 1.0f / body.inertiaMoment;
        float j = -(1.0f + e) * vn;
        j /= (invMass + (rCrossN * rCrossN) * invInertia);

        // 速度と角速度の更新
        Vector2 impulse = normal * j;
        body.velocity = body.velocity + impulse * invMass;

        // トルクによる角速度の変化: Δω = (r x impulse) / I
        float torque = r.x * impulse.y - r.y * impulse.x;
        body.angularVelocity += torque * invInertia;

        // 位置補正（めり込み解消）
        body.center += normal * (dx < dy ? dx : dy);

    }

}