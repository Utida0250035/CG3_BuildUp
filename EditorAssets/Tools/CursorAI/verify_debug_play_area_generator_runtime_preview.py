#!/usr/bin/env python3
"""DebugPlayAreaGenerator のランタイムプレビュー手順がエディタ導線の正本になっているか検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
README = ROOT / "EditorAssets" / "Tools" / "DebugPlayAreaGenerator" / "README.md"
GUIDANCE_H = (
    ROOT
    / "Project"
    / "Codes"
    / "Include"
    / "System"
    / "Editor"
    / "DebugPlayAreaRuntimePreviewGuidance.h"
)
SCAFFOLD_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorDebugPlayAreaScaffold.cpp"
SCAFFOLD_PY = ROOT / "EditorAssets" / "Tools" / "DebugPlayAreaGenerator" / "scaffold_play_area.py"


def extract_ksteps_ja(text: str) -> str | None:
    match = re.search(r'kStepsJa\[\]\s*=\s*"([^"]+)"', text, re.DOTALL)
    if not match:
        return None
    return match.group(1).replace("\n", "")


def main() -> int:
    errors: list[str] = []

    if not README.is_file():
        errors.append("missing DebugPlayAreaGenerator README.md")
    else:
        readme = README.read_text(encoding="utf-8", errors="replace")
        if "SceneBuildUpRegistry::Launch" in readme:
            errors.append("README must not document raw Launch API as primary path")
        for marker in ("シーンを実行", "作成して BuildUp", "DebugPlayAreaRuntimePreviewGuidance"):
            if marker not in readme:
                errors.append(f"README missing {marker!r}")
        if "C++ の `Launch` 直書きは不要" not in readme:
            errors.append("README should state Launch direct-call is unnecessary")

    if not GUIDANCE_H.is_file():
        errors.append("missing DebugPlayAreaRuntimePreviewGuidance.h")
    else:
        guidance = GUIDANCE_H.read_text(encoding="utf-8", errors="replace")
        steps = extract_ksteps_ja(guidance)
        if not steps:
            errors.append("kStepsJa not found in guidance header")
        if "kPanelHintJa" not in guidance:
            errors.append("kPanelHintJa missing")

    if SCAFFOLD_CPP.is_file():
        cpp = SCAFFOLD_CPP.read_text(encoding="utf-8", errors="replace")
        if "DebugPlayAreaRuntimePreviewGuidance::kStepsJa" not in cpp:
            errors.append("project panel must show runtime preview guidance")
    else:
        errors.append("missing EditorDebugPlayAreaScaffold.cpp")

    if SCAFFOLD_PY.is_file():
        py = SCAFFOLD_PY.read_text(encoding="utf-8", errors="replace")
        if "SceneBuildUpRegistry::Launch" in py:
            errors.append("scaffold_play_area.py must not mention Launch API")
    else:
        errors.append("missing scaffold_play_area.py")

    if errors:
        print("verify_debug_play_area_generator_runtime_preview: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_debug_play_area_generator_runtime_preview: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())