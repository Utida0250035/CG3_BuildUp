#include "Collision.h"
#include <algorithm>
#include <numbers>

// MyOBBとベジェ曲線の交差判定（再帰）
bool CheckCollision(const MyOBB& obb, const Vector2 p[3], float t1, float t2, int depth, float& hitT) {

	// 区間の開始点
	Vector2 pStart = CalcBezier2(p, t1);

	// 区間の中間点
	Vector2 pMid = CalcBezier2(p, (t1 + t2) * 0.5f);

	// 区間の終点
	Vector2 pEnd = CalcBezier2(p, t2);

	// AABBの計算に制御点(p[1])のその区間での影響を含める（簡易的にはpMidで代用せず、p[1]を考慮）
	float minX = std::min({ obb.ToLocal(pStart).x, obb.ToLocal(pEnd).x, obb.ToLocal(pMid).x });
	float maxX = std::max({ obb.ToLocal(pStart).x, obb.ToLocal(pEnd).x, obb.ToLocal(pMid).x });
	float minY = std::min({ obb.ToLocal(pStart).y, obb.ToLocal(pEnd).y, obb.ToLocal(pMid).y });
	float maxY = std::max({ obb.ToLocal(pStart).y, obb.ToLocal(pEnd).y, obb.ToLocal(pMid).y });

	// 明確にMyOBB範囲外なら即座に抜ける
	if (minX > obb.halfSize.x + 32.0f || maxX < -obb.halfSize.x - 32.0f ||
		minY > obb.halfSize.y + 32.0f || maxY < -obb.halfSize.y - 32.0f) {
		return false;
	}

	if (depth >= 8) {
		// 十分な精度に達したらtを確定

		hitT = (t1 + t2) * 0.5f;
		return true;
	}

	float mid = (t1 + t2) * 0.5f;

	// 重心(obb.center)に近い方の区間を優先して探索する
	float d1 = VectorLengthSquare((CalcBezier2(p, (t1 + mid) * 0.5f) - obb.center));
	float d2 = VectorLengthSquare((CalcBezier2(p, (mid + t2) * 0.5f) - obb.center));

	if (d1 < d2) {

		if (CheckCollision(obb, p, t1, mid, depth + 1, hitT)) {
			return true;
		}

		if (CheckCollision(obb, p, mid, t2, depth + 1, hitT)) {
			return true;
		}

	} else {

		if (CheckCollision(obb, p, mid, t2, depth + 1, hitT)) {
			return true;
		}

		if (CheckCollision(obb, p, t1, mid, depth + 1, hitT)) {
			return true;
		}

	}

	return false;

}

// MyOBBとベジェ曲線の交差判定（再帰）詳細版
bool CheckCollisionDetailed(const MyOBB& obb, const Vector2& obbVertexPos, const Vector2 pBezier[3], float& t1, float& t2, int depthCount, const int depth) {

	// 区間の開始点
	Vector2 pStart = CalcBezier2(pBezier, t1);

	// 区間の中間点
	Vector2 pMid = CalcBezier2(pBezier, (t1 + t2) * 0.5f);

	// 区間の終点
	Vector2 pEnd = CalcBezier2(pBezier, t2);

	// AABBの計算に制御点(p[1])のその区間での影響を含める（簡易的にはpMidで代用せず、p[1]を考慮）
	float minX = std::min({ obb.ToLocal(pStart).x, obb.ToLocal(pEnd).x, obb.ToLocal(pMid).x });
	float maxX = std::max({ obb.ToLocal(pStart).x, obb.ToLocal(pEnd).x, obb.ToLocal(pMid).x });
	float minY = std::min({ obb.ToLocal(pStart).y, obb.ToLocal(pEnd).y, obb.ToLocal(pMid).y });
	float maxY = std::max({ obb.ToLocal(pStart).y, obb.ToLocal(pEnd).y, obb.ToLocal(pMid).y });

	// 明確にAABB範囲外なら即座に抜ける
	if (minX > obb.halfSize.x + 32.0f || maxX < -obb.halfSize.x - 32.0f ||
		minY > obb.halfSize.y + 32.0f || maxY < -obb.halfSize.y - 32.0f) {
		return false;
	}

	if (depthCount >= depth) {
		// 十分な精度に達したらtを確定
		return true;
	}

	float mid = (t1 + t2) * 0.5f;

	// 重心(obb.center)に近い方の区間を優先して探索する
	float d1 = VectorLengthSquare((CalcBezier2(pBezier, (t1 + mid) * 0.5f) - obbVertexPos));
	float d2 = VectorLengthSquare((CalcBezier2(pBezier, (mid + t2) * 0.5f) - obbVertexPos));

	float t1Temp = 0.0f;
	float t2Temp = 0.0f;

	// 矩形に近い区間を優先して判定する
	if (d1 < d2) {

		t2Temp = t2;

		t2 = mid;

		if (CheckCollisionDetailed(obb, obbVertexPos, pBezier, t1, t2, depthCount + 1, depth)) {
			return true;
		}

		t2 = t2Temp;


		t1Temp = t1;

		t1 = mid;

		if (CheckCollisionDetailed(obb, obbVertexPos, pBezier, t1, t2, depthCount + 1, depth)) {
			return true;
		}

		t1 = t1Temp;

	} else {

		t1Temp = t1;

		t1 = mid;

		if (CheckCollisionDetailed(obb, obbVertexPos, pBezier, t1, t2, depthCount + 1, depth)) {
			return true;
		}

		t1 = t1Temp;


		t2Temp = t2;

		t2 = mid;

		if (CheckCollisionDetailed(obb, obbVertexPos, pBezier, t1, t2, depthCount + 1, depth)) {
			return true;
		}

		t2 = t2Temp;

	}

	return false;

}

bool ResolveObbBezierResponse(MyOBB& obb, Vector2& velocity, const Vector2 p[3]) {

	float hitT = 0.0f;

	// 1. 衝突検知
	if (CheckCollision(obb, p, 0.0f, 1.0f, 0, hitT)) {

		// 2. 衝突点（ワールド座標）とローカル座標の取得
		Vector2 worldContact = CalcBezier2(p, hitT);
		Vector2 localContact = obb.ToLocal(worldContact);

		// 3. 応答法線（MyOBBのどの面で当たったか）の決定
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
		obb.center = obb.center + normal * (overlap + 1.25f);

		// 5. 速度の反射 (物理応答)
		float restitution = 0.4f; // 跳ね返り係数
		float vn = VectorDot(velocity, normal);

		if (vn < 0) { // 法線方向（壁）に向かって進んでいる場合のみ
			velocity = velocity - normal * (vn * (1.0f + restitution));
		}

		return true;

	}

	return false;

}

bool ResolveRigidBodyObbBezierResponse(RigidBodyOBB& body, const Vector2 pBezier[3]) {

	float hitT = 0.0f;

	if (CheckCollision(body, pBezier, 0.0f, 1.0f, 0, hitT)) {

		// 衝突点と法線の取得
		Vector2 contactPoint = CalcBezier2(pBezier, hitT);
		Vector2 localContact = body.ToLocal(contactPoint);

		// 法線(normal)と押し出し量(overlap)を決定

		// MyOBBの面法線
		Vector2 normal;
		float dx = body.halfSize.x - std::abs(localContact.x);
		float dy = body.halfSize.y - std::abs(localContact.y);

		float overlap = 0.0f;

		if (dx < dy) {

			normal = body.axis[0] * (localContact.x > 0 ? 1.0f : -1.0f);

			overlap = dx;

		} else {

			normal = body.axis[1] * (localContact.y > 0 ? 1.0f : -1.0f);

			overlap = dy;

		}

		if (overlap > body.size.x) {

			if (overlap > body.size.y) {

				return false;

			}

		}

		// 衝突した面からMyOBBを外に押し出す方向に法線の向きを固定する

		Vector2 towardCenter = body.center - contactPoint;

		if (VectorDot(normal, towardCenter) < 0) {

			normal *= -1.0f;

		}

		if (overlap > 0.0f) {

			// 位置補正（めり込み解消）
			body.center += normal * (overlap + 0.5f);

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
		float e = 0.8f; // 反発係数
		float vn = VectorDot(velocityAtPoint, normal);

		if (vn > 0.0f) {

			// 離れていく方向なら処理しない
			return false;

		}


		if (VectorLengthSquare(body.velocity) <= 1.0f) {

			e = 0.0f;
			body.velocity = Vector2{};

		}

		body.velocity *= 0.99f;
		body.angularVelocity *= 0.99f;

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

		return true;

	}

	return false;

}

bool ResolveRigidBodyObbBezierResponseDetailed(RigidBodyOBB& body, const Vector2 pBezier[3], float restitution, float boundPercent, float allowRange, int depth) {

	bool isCollision = false;

	Vector2 corners[4]{};

	body.GetWorldCorners(corners);

	float t1 = 0.0f;
	float t2 = 1.0f;

	bool isHitLeftEnd = (ProcessPointToOBBEdges(body, CalcBezier2(pBezier, 0.0f), restitution, boundPercent, allowRange));

	bool isHitRightEnd = ProcessPointToOBBEdges(body, CalcBezier2(pBezier, 1.0f), restitution, boundPercent, allowRange);

	isCollision = isHitLeftEnd || isHitRightEnd;

	for (auto& corner : corners) {

		// t1, t3をリセット
		t1 = 0.0f;
		t2 = 1.0f;

		if (CheckCollisionDetailed(body, corner, pBezier, t1, t2, 0, depth)) {

			ProcessPointToSegmentInBezier(body, corner, pBezier, t1, t2, restitution, boundPercent, allowRange);

			if (!isHitLeftEnd) {

				ProcessPointInBezierToOBBEdges(body, pBezier, t1, restitution, boundPercent, allowRange);

			}

			if (!isHitRightEnd) {

				ProcessPointInBezierToOBBEdges(body, pBezier, t2, restitution, boundPercent, allowRange);

			}

			isCollision = true;

		}

	}

	return isCollision;

}

void ProcessPointToSegment(RigidBodyOBB& body, const Vector2& point, const Vector2& segmentStart, const Vector2& segmentEnd) {
	Vector2 segment = segmentEnd - segmentStart;
	Vector2 relative = point - segmentStart;

	// 線分上の最近接点への射影係数 t
	float t = std::clamp(VectorDot(relative, segment) / VectorLengthSquare(segment), 0.0f, 1.0f);
	Vector2 closestPoint = segmentStart + segment * t;
	Vector2 diff = point - closestPoint;
	float dist = VectorLength(diff);

	// めり込み判定（小さな厚みを考慮）
	const float thickness = 3.0f;
	if (dist < thickness && dist > 0.0f) {
		Vector2 normal = diff / dist; // 点を押し出す方向

		// 法線の向きを重心方向で補正（上面なら上へ）
		if (VectorDot(normal, (body.center - closestPoint)) < 0) {
			normal = normal * -1.0f;
		}

		float overlap = thickness - dist;
		ApplyImpulse(body, point, normal, overlap, 0.8f, 0.8f, 0.05f);
	}
}

void ProcessPointToSegmentInBezier(RigidBodyOBB& body, const Vector2& point, const Vector2 pBezier[3], float t1, float t2, float restitution, float boundPercent, float allowRange) {

	Vector2 segmentStart = CalcBezier2(pBezier, t1);
	Vector2 segmentEnd = CalcBezier2(pBezier, t2);

	Vector2 segment = segmentEnd - segmentStart;
	Vector2 relative = point - segmentStart;

	// 線分上の最近接点への射影係数 t
	float t = std::clamp(VectorDot(relative, segment) / VectorLengthSquare(segment), 0.0f, 1.0f);
	Vector2 closestPoint = segmentStart + segment * t;
	Vector2 difference = point - closestPoint;
	float distance = VectorLength(difference);

	// めり込み判定（小さな厚みを考慮）
	const float thickness = 3.0f;

	float localT = std::clamp(VectorDot(relative, segment) / VectorLengthSquare(segment), 0.0f, 1.0f);

	// 線分上の位置から、ベジェ全体の t を逆算
	float actualT = t1 + (t2 - t1) * localT;

	if (distance < thickness && distance > 0.0f) {

		Vector2 normal;
		// 線分から作るのではなく、曲線の微分から法線を作る
		Vector2 tangent = CalcBezier2Tangent(pBezier, actualT);
		normal = VectorNormalize(Vector2{ -tangent.y, tangent.x });

		// 重心方向を向くように補正
		if (VectorDot(normal, body.center - point) < 0) {
			normal = normal * -1.0f;
		}

		float overlap = thickness - distance;
		ApplyImpulse(body, point, normal, overlap, restitution, boundPercent, allowRange);

	}

}

bool ProcessPointToOBBEdges(RigidBodyOBB& body, const Vector2& pWorld, const float restitution, const float boundPercent, const float allowRange) {
	// ローカル座標への変換
	Vector2 local = body.ToLocal(pWorld);

	// 侵入判定 (IsPointInOBB)
	float dx = body.halfSize.x - std::abs(local.x);
	float dy = body.halfSize.y - std::abs(local.y);

	if (dx >= -allowRange && dy >= -allowRange) {
		// 侵入している

		Vector2 normal;
		float overlap;

		// 3. 左右と上下、どちらの辺に近いか（めり込みが浅い方へ押し出す）
		if (dx < dy) {
			// 左右の辺
			float side = (local.x > 0 ? 1.0f : -1.0f);
			normal = body.axis[0] * side;
			overlap = dx;
		} else {
			// 上下の辺
			float side = (local.y > 0 ? 1.0f : -1.0f);
			normal = body.axis[1] * side;
			overlap = dy;
		}

		// 衝突応答の適用
		// 点 P が止まっている地面側だとすると、押し返されるのは body（矩形）側
		// 法線は「点から矩形の外へ向かう向き」にする必要がある
		Vector2 towardCenter = body.center - pWorld;
		if (VectorDot(normal, towardCenter) < 0) {
			normal = normal * -1.0f;
		}

		// 第三引数は「衝突が起きたワールド座標」として pWorld を渡す
		ApplyImpulse(body, pWorld, normal, overlap, restitution, boundPercent, allowRange);

		return true;

	}

	return false;

}

void ProcessPointInBezierToOBBEdges(RigidBodyOBB& body, const Vector2 pBezier[3], float t, float restitution, float boundPercent, float allowRange) {

	// 指定された t 地点のワールド座標と、その地点の「滑らかな法線」を取得
	Vector2 pWorld = CalcBezier2(pBezier, t);
	Vector2 tangent = CalcBezier2Tangent(pBezier, t);
	Vector2 smoothNormal = VectorNormalize(Vector2{ -tangent.y, tangent.x });

	// ローカル座標への変換
	Vector2 local = body.ToLocal(pWorld);

	// 侵入判定 (AABB内判定と同じ原理)
	float dx = body.halfSize.x - std::abs(local.x);
	float dy = body.halfSize.y - std::abs(local.y);

	Vector2 localNormal = { VectorDot(smoothNormal, body.axis[0]), VectorDot(smoothNormal, body.axis[1]) };

	if (dx >= -allowRange && dy >= -allowRange) {
		// 侵入している
		Vector2 obbNormal;
		float overlap;

		// 矩形のどの辺から押し出すべきか判定（最も浅い方向 + 法線の向きを加味）
		if (dx * std::abs(localNormal.x) < dy * std::abs(localNormal.y)) {
			float side = (local.x > 0 ? 1.0f : -1.0f);
			obbNormal = body.axis[0] * side;
			overlap = dx;
		} else {
			float side = (local.y > 0 ? 1.0f : -1.0f);
			obbNormal = body.axis[1] * side;
			overlap = dy;
		}

		// 押し出し方向を「曲線の法線」に寄せる
		// obbNormal（矩形の辺の向き）だけで押し返すと、継ぎ目でカクつくため、
		// 曲線の smoothNormal と向きを合わせる
		Vector2 finalNormal = smoothNormal;

		// smoothNormal が矩形の内側を向いている場合は反転させる
		if (VectorDot(finalNormal, body.center - pWorld) < 0) {
			finalNormal = finalNormal * -1.0f;
		}

		// 衝突応答を適用
		// hitPoint は曲線の点(pWorld)、押し出す方向は finalNormal
		ApplyImpulse(body, pWorld, finalNormal, overlap, restitution, boundPercent, allowRange);

	}

}

void ApplyImpulse(RigidBodyOBB& body, const Vector2& hitPoint, const Vector2& normal, const float overlap, const float restitution, const float boundPercent, const float allowRange) {

	// 位置補正 (疑似的な射影法)

	// めり込んでいる分だけ、即座に押し出す
	Vector2 correction = normal * (std::max(overlap + allowRange, 0.0f) * boundPercent);
	body.center += correction;

	// 衝突点での相対速度の計算

	// 重心から衝突点へのベクトル
	Vector2 r = hitPoint - body.center;

	// 衝突点の速度 = 平行移動速度 + (角速度 * 腕の長さの垂直ベクトル)
	Vector2 contactPointVelocity = body.velocity + Vector2{ -body.angularVelocity * r.y, body.angularVelocity * r.x };

	// 法線方向の相対速度 (接近しているか離れているか)
	float vn = VectorDot(contactPointVelocity, normal);

	// 既に離れる方向に動いている（vn > 0）なら、速度変化（衝撃）は適用しない
	if (vn > 0.0f) {
		return;
	}

	// 反発係数を用いた衝撃量の計算

	// 慣性モーメントを考慮した質量（スカラー）
	// 衝突点にどれだけ「力が伝わりにくいか」を計算する
	// cp = (r × n)^2 / I (2D外積の自乗 / 慣性モーメント)
	float rCrossN = VectorCross(r, normal);
	float impulseSum = (1.0f / body.mass) + (rCrossN * rCrossN) / body.inertiaMoment;

	// 衝撃量 j の算出
	float j = -(1.0f + restitution) * vn;
	j /= impulseSum;

	// 速度と角速度の更新

	Vector2 impulse = normal * j;

	// 平行移動速度の更新: v = v + J/m
	body.velocity += impulse * (1.0f / body.mass);

	// 角速度の更新: ω = ω + (r × J) / I
	body.angularVelocity += VectorCross(r, impulse) / body.inertiaMoment;

}