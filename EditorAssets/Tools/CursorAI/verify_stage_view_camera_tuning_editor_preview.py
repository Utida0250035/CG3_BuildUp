#!/usr/bin/env python3
"""StageViewCameraTuning のエディタ内プレビュー／ホットリロード配線を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []

    tuning_cpp = ROOT / "Project/Codes/Source/System/Editor/Camera/StageViewCameraTuning.cpp"
    panel_cpp = ROOT / "Project/Codes/Source/System/Editor/Camera/StageViewCameraTuningPanel.cpp"
    tick_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "Camera"
        / "EditorStageViewCameraTuningHotReloadTick.cpp"
    )
    scene_cpp = ROOT / "Project/Codes/Source/Scene/Scene.cpp"
    workbench = ROOT / "Project/Codes/Source/System/Editor/EditorEntityWorkbench.cpp"
    odyssey = (
        ROOT
        / "EditorAssets"
        / "Tools"
        / "CursorAI"
        / "docs"
        / "feature-memos"
        / "odyssey_style_stage_camera.md"
    )
    vcxproj = ROOT / "Project/CG2_BuildUp.vcxproj"

    for path in (tuning_cpp, panel_cpp, tick_cpp, scene_cpp, workbench):
        if not path.is_file():
            errors.append(f"missing {path.name}")

    if tuning_cpp.is_file():
        text = tuning_cpp.read_text(encoding="utf-8", errors="replace")
        if "PollHotReload" not in text:
            errors.append("StageViewCameraTuning must implement PollHotReload")

    if panel_cpp.is_file():
        panel = panel_cpp.read_text(encoding="utf-8", errors="replace")
        if "JSON preview on Game view" not in panel:
            errors.append("panel must offer Game view JSON preview toggle")
        if "Auto hot-reload" not in panel:
            errors.append("panel must offer auto hot-reload toggle")

    if tick_cpp.is_file() and workbench.is_file():
        tick = tick_cpp.read_text(encoding="utf-8", errors="replace")
        wb = workbench.read_text(encoding="utf-8", errors="replace")
        if "TickStageViewCameraTuningHotReload" not in tick:
            errors.append("hot reload tick missing")
        if "TickStageViewCameraTuningHotReload" not in wb:
            errors.append("workbench must call TickStageViewCameraTuningHotReload")

    if scene_cpp.is_file():
        scene = scene_cpp.read_text(encoding="utf-8", errors="replace")
        if "IsGameViewPreviewEnabled" not in scene:
            errors.append("Scene must honor game view preview flag")

    if odyssey.is_file():
        odyssey_text = odyssey.read_text(encoding="utf-8", errors="replace")
        if "未実装（次）" in odyssey_text:
            errors.append("odyssey_style_stage_camera.md still marks preview as unimplemented")
        if "StageViewCameraTuningPanel" not in odyssey_text and "Stage camera tuning" not in odyssey_text:
            if "stage_view_camera_tuning" not in odyssey_text.lower():
                errors.append("odyssey doc should reference editor preview")
    else:
        errors.append("missing odyssey_style_stage_camera.md")

    if vcxproj.is_file():
        if "EditorStageViewCameraTuningHotReloadTick.cpp" not in vcxproj.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("vcxproj missing hot reload tick TU")

    if errors:
        print("verify_stage_view_camera_tuning_editor_preview: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_stage_view_camera_tuning_editor_preview: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())