#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

namespace Atrum {
class Camera;
}

namespace Atrum::Editor {

/// ImGui 上の矩形（フレームバッファ座標・左上原点）
struct SubViewportRect {
  int x = 0;
  int y = 0;
  int width = 0;
  int height = 0;
  bool active = false;
};

/// シーンビュー種別（エディット／ゲーム初期／実行）
enum class SceneViewTab {
  Edit,
  Game,
  Runtime,
};

/// ImGui::Image 用の論理サイズ
struct SceneViewDisplaySize {
  float width = 0.0f;
  float height = 0.0f;
};

/// フレーム単位のエディタ描画状態（シーンビュー範囲など）
class EditorFrame {
public:
  static EditorFrame &Instance();

  void ResetPerFrame();

  void SetSceneViewRect(const SceneViewTab tab, const SubViewportRect &rect,
                        const bool hovered);
  const SubViewportRect &SceneViewRect(const SceneViewTab tab) const;

  void SetSceneViewDisplaySize(const SceneViewTab tab,
                               const SceneViewDisplaySize &size);
  const SceneViewDisplaySize &SceneViewDisplaySizeOf(
      const SceneViewTab tab) const;

  void SetSceneViewTab(const SceneViewTab tab);
  SceneViewTab SceneViewTabSelection() const { return sceneViewTab_; }

  bool EditViewHovered() const { return editViewHovered_; }

private:
  SubViewportRect editView_{};
  SubViewportRect gameView_{};
  SubViewportRect runtimeView_{};
  SceneViewDisplaySize editDisplay_{};
  SceneViewDisplaySize gameDisplay_{};
  SceneViewDisplaySize runtimeDisplay_{};
  bool editViewHovered_ = false;
  SceneViewTab sceneViewTab_ = SceneViewTab::Edit;
};

/// 現在の ImGui 領域でシーン描画矩形を計測（バックバッファピクセル）
bool MeasureSceneViewContentRegion(SubViewportRect &outRect,
                                   bool *outHovered = nullptr,
                                   const char *hitId = "##scene_view_hit");

void FitCameraPerspectiveToViewport(::Atrum::Camera *camera, int width,
                                    int height);

} // namespace Atrum::Editor

#endif
