#pragma once

namespace Atrum {

class Entity;
class MovieCameraController;

/// 復帰フェード待ち中もカメラ終端姿勢と SkyDome を更新し続ける
void TickSceneMovieVisualsDuringPendingTransition(
    bool trackPlaybackStarted, MovieCameraController &movieCamera,
    Entity *skyDome);

} // namespace Atrum