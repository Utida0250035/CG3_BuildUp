#pragma once

#include "Math/Matrix4x4.h"
#include "Math/Vector3.h"
#include <array>
#include <cstdint>
#include <functional>

namespace Atrum::Renderer {

/// ワールド始点・終点から、カメラ向きの太さを持つ線分用クアッド（4 頂点）を組み立てる
/// @return 有効な線分なら true（長さ・幅が極小のとき false）
bool BuildCameraFacingLineQuad(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const float width, const Math::Vector3 &cameraWorldPosition,
    std::array<Math::Vector3, 4> &outPositions, Math::Vector3 &outNormal);

/// XZ 床面上の線分（幅は床面内のみ。カメラ角で太さが変わらない）
bool BuildPlanarLineQuadOnFloor(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const float width, std::array<Math::Vector3, 4> &outPositions,
    Math::Vector3 &outNormal);

/// カメラ前方かつ投影可能な区間だけに線分を切り詰める
bool ClipWorldLineToVisibleProjection(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const Math::Matrix4x4 &viewProjection, Math::Vector3 &clippedStart,
    Math::Vector3 &clippedEnd);

/// 投影可能な連続区間ごとにコールバック（途中の非可視部を描かない）
void ForEachVisibleLineSubsegment(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const Math::Matrix4x4 &viewProjection,
    const std::function<void(const Math::Vector3 &, const Math::Vector3 &)>
        &emitSegment);

/// ワールド点付近における 1 ピクセルあたりの世界幅（フォールバック線幅用）
float EstimateMetersPerPixelAtWorldPoint(
    const Math::Vector3 &worldPoint, const Math::Vector3 &cameraWorldPosition,
    const Math::Matrix4x4 &perspectiveMatrix, const int viewportHeight);

/// 画面上のピクセル幅を保ち XZ 床へ再投影した線分クアッド
bool BuildScreenStableFloorLineQuad(
    const Math::Vector3 &worldStart, const Math::Vector3 &worldEnd,
    const float lineWidthPixels, const int viewportWidth,
    const int viewportHeight, const Math::Vector3 &cameraWorldPosition,
    const Math::Matrix4x4 &viewMatrix,
    const Math::Matrix4x4 &perspectiveMatrix, const float floorY,
    std::array<Math::Vector3, 4> &outPositions, Math::Vector3 &outNormal);

} // namespace Atrum::Renderer
