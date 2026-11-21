#!/usr/bin/env python3
"""EditorAssetsPaths に DebugPlayArea 用パス定数が揃っているか検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PATHS_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorAssetsPaths.h"
ROOTS_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorEditableSceneJsonRoots.h"
SCAFFOLD_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorDebugPlayAreaScaffold.cpp"
BOOT_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "CommandSceneDevelopmentBoot.cpp"
MEMO = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "docs" / "feature-memos" / "debug_test_play_area.md"
CODES = ROOT / "Project" / "Codes"

REQUIRED_CONSTANTS = (
    "kDebugPlayAreaDirectory",
    "kDebugPlayAreaTemplateScene",
    "kDebugPlayAreaFeatureTestScene",
    "kDebugPlayAreaCombatBoundsSandboxScene",
)


def main() -> int:
    errors: list[str] = []

    if not PATHS_H.is_file():
        errors.append("missing EditorAssetsPaths.h")
    else:
        text = PATHS_H.read_text(encoding="utf-8", errors="replace")
        for name in REQUIRED_CONSTANTS:
            if name not in text:
                errors.append(f"EditorAssetsPaths.h missing {name}")
        if "DebugPlayArea/" not in text and "DebugPlayArea\\" not in text:
            errors.append("EditorAssetsPaths.h must define DebugPlayArea paths")

    if ROOTS_H.is_file():
        roots = ROOTS_H.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaDirectory" not in roots:
            errors.append("EditorEditableSceneJsonRoots must include kDebugPlayAreaDirectory")
    else:
        errors.append("missing EditorEditableSceneJsonRoots.h")

    if SCAFFOLD_CPP.is_file():
        scaffold = SCAFFOLD_CPP.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaTemplateScene" not in scaffold:
            errors.append("scaffold must use kDebugPlayAreaTemplateScene")
        if "kDebugPlayAreaDirectory" not in scaffold:
            errors.append("scaffold must use kDebugPlayAreaDirectory")

    if BOOT_CPP.is_file():
        boot = BOOT_CPP.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaDirectory" not in boot:
            errors.append("Development boot must use kDebugPlayAreaDirectory")

    # 直書きパスは EditorAssetsPaths.h の正本のみ
    literal = re.compile(r"\.\./EditorAssets/DebugPlayArea")
    for cpp in CODES.rglob("*.cpp"):
        rel = cpp.relative_to(ROOT)
        text = cpp.read_text(encoding="utf-8", errors="replace")
        if literal.search(text):
            errors.append(f"literal DebugPlayArea path in {rel} (use EditorAssetsPaths)")
    for h in CODES.rglob("*.h"):
        if h.name == "EditorAssetsPaths.h":
            continue
        rel = h.relative_to(ROOT)
        text = h.read_text(encoding="utf-8", errors="replace")
        if literal.search(text):
            errors.append(f"literal DebugPlayArea path in {rel}")

    if MEMO.is_file():
        memo = MEMO.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaDirectory" not in memo:
            errors.append("debug_test_play_area.md should cite EditorAssetsPaths constants")
    else:
        errors.append("missing debug_test_play_area.md")

    if errors:
        print("verify_editor_assets_paths_debug_play_area: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_assets_paths_debug_play_area: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())