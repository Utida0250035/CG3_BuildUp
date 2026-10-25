#pragma once

#include "Math/Vector3.h"

namespace Atrum {

class Camera;

/// 透視戦闘カメラの eye / 注視点
struct StageViewCameraPose {
  Math::Vector3 eye{};
  Math::Vector3 pivot{};
};

/// ComputeFocusViewPose 用の被写体半サイズ（高さ・幅）
struct StageViewFocusSubjectSize {
  float halfHeight = 1.0f;
  float halfWidth = 1.0f;
};

inline constexpr StageViewFocusSubjectSize kBossIntroFocusSubject{
    1.35f, 1.35f};
inline constexpr StageViewFocusSubjectSize kBossDefeatFocusSubject{1.05f,
                                                                     1.05f};
inline constexpr float kBossDefeatCameraPushSeconds = 2.2f;

/// 戦闘カメラ：真横ではなく pivot より少し上・手前寄りから見下ろす
inline constexpr float kStageBattleViewEyeHeightAbovePivotY = 3.6f;
inline constexpr float kStageBattleViewEyeSideOffsetX = -2.25f;
inline constexpr float kStageBattleViewDistanceScale = 1.15f;

/// 戦闘エリア全体が画面に収まるカメラ姿勢
StageViewCameraPose ComputeStageBattleViewPose();

/// 指定ワールド座標を画面中心に近づけるカメラ姿勢
StageViewCameraPose ComputeFocusViewPose(const Math::Vector3 &focusWorld,
                                         float subjectHalfHeight,
                                         float subjectHalfWidth);

void ApplyStageViewCameraPose(Camera *camera, const StageViewCameraPose &pose);

StageViewCameraPose LerpStageViewCameraPose(const StageViewCameraPose &from,
                                            const StageViewCameraPose &to,
                                            float t);

} // namespace Atrum
