#!/usr/bin/env python3
"""FitCameraPerspectiveToViewport が tuning JSON の FOV を使うか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
FRAME = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorFrame.cpp"
PARAMS = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "EditorFrameFitCameraPerspectiveParams.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not PARAMS.is_file():
        errors.append("missing EditorFrameFitCameraPerspectiveParams.cpp")
    else:
        text = PARAMS.read_text(encoding="utf-8", errors="replace")
        if "StageViewCameraTuning" not in text or "PerspectiveFovY" not in text:
            errors.append("params must read PerspectiveFovY from tuning")

    if FRAME.is_file():
        body = FRAME.read_text(encoding="utf-8", errors="replace")
        fn = body.split("void FitCameraPerspectiveToViewport", 1)
        if len(fn) < 2:
            errors.append("missing FitCameraPerspectiveToViewport")
        else:
            block = fn[1].split("} // namespace", 1)[0]
            if "PerspectiveFov(0.5f" in block:
                errors.append("must not hardcode FOV 0.5 in FitCameraPerspectiveToViewport")
            if "ResolveViewportPerspectiveFovY" not in block:
                errors.append("FitCameraPerspectiveToViewport must use ResolveViewportPerspectiveFovY")
    else:
        errors.append("missing EditorFrame.cpp")

    if errors:
        print("verify_editor_frame_fit_camera_perspective_fov: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_frame_fit_camera_perspective_fov: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())