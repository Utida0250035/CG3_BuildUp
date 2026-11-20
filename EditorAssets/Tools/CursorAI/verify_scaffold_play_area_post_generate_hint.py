#!/usr/bin/env python3
"""scaffold 完了案内が Launch だけでなくエディタ導線を含むことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]

REQUIRED_MARKERS = (
    "エディタ内プレビュー",
    "シーンを実行",
    "BuildUp",
    "起動シーン",
)


def main() -> int:
    errors: list[str] = []
    py_path = ROOT / "EditorAssets" / "Tools" / "DebugPlayAreaGenerator" / "scaffold_play_area.py"
    cpp_path = (
        ROOT
        / "Project"
        / "Codes"
        / "Include"
        / "System"
        / "Editor"
        / "DebugPlayAreaScaffoldGuidance.h"
    )
    scaffold_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "System"
        / "Editor"
        / "EditorDebugPlayAreaScaffold.cpp"
    )

    for path in (py_path, cpp_path):
        if not path.is_file():
            errors.append(f"missing {path.name}")
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        if path.name == "scaffold_play_area.py":
            if "SceneBuildUpRegistry::Launch" in text:
                errors.append("python scaffold must not only mention Launch API")
        for marker in REQUIRED_MARKERS:
            if marker not in text:
                errors.append(f"{path.name}: missing marker {marker!r}")

    if scaffold_cpp.is_file():
        text = scaffold_cpp.read_text(encoding="utf-8", errors="replace")
        if "QueueEditorScenePlayViaBuildUp" not in text:
            errors.append("editor scaffold should offer BuildUp queue shortcut")
        if "QueueInEditorPlayRuntimeTabOnly" not in text:
            errors.append("editor scaffold should offer in-editor preview shortcut")

    if errors:
        print("verify_scaffold_play_area_post_generate_hint: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scaffold_play_area_post_generate_hint: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())