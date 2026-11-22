#!/usr/bin/env python3
"""SceneBuildUp::Draw が EditorFrame ワークベンチに依存しないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"

FORBIDDEN = (
    "LayoutDedicatedSceneViewWindow",
    "DrawRenderDiagnosticsMainMenuBar",
    "EditorFrame::Instance().ResetPerFrame",
    "LayoutEditorWorkbench",
    "RenderSceneViews",
)


def main() -> int:
    errors: list[str] = []
    if not BUILDUP.is_file():
        errors.append("missing SceneBuildUp.cpp")
    else:
        text = BUILDUP.read_text(encoding="utf-8", errors="replace")
        draw = text.split("SceneBuildUp::Draw()", 1)
        if len(draw) > 1:
            body = draw[1].split("void SceneBuildUp::", 1)[0]
            if "DrawScene()" not in body:
                errors.append("Draw must call DrawScene for minimal runtime path")
            for token in FORBIDDEN:
                if token in body:
                    errors.append(f"Draw must not call {token}")
        else:
            errors.append("missing SceneBuildUp::Draw")

        scene_draw = text.split("SceneBuildUp::DrawScene()", 1)
        if len(scene_draw) > 1:
            body = scene_draw[1].split("void SceneBuildUp::", 1)[0]
            if "#if defined(DEVELOPMENT)" in body and "return;" in body.split(
                "Atrum::Engine", 1
            )[0]:
                errors.append("DrawScene must not early-return in DEVELOPMENT")

    if errors:
        print("verify_scene_build_up_draw_skips_editor_frame: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_draw_skips_editor_frame: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())