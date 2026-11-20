#!/usr/bin/env python3
"""BuildUp が親子階層の描画契約を EditorSceneRuntime と揃えることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
DRAW_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpRuntimeEntityDraw.cpp"
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"


def main() -> int:
    errors: list[str] = []

    if not DRAW_CPP.is_file():
        errors.append("missing SceneBuildUpRuntimeEntityDraw.cpp")
    elif "DrawSceneDocumentEntities" not in DRAW_CPP.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("entity draw must delegate to DrawSceneDocumentEntities")

    if BUILDUP.is_file():
        text = BUILDUP.read_text(encoding="utf-8", errors="replace")
        block = text.split("DrawRuntimeEntities()", 1)
        if len(block) < 2:
            errors.append("missing DrawRuntimeEntities")
        else:
            body = block[1].split("void SceneBuildUp::", 1)[0]
            if "SceneBuildUpRuntimeEntityDraw::Draw" not in body:
                errors.append("DrawRuntimeEntities must use SceneBuildUpRuntimeEntityDraw")
            if "GetParent()" in body and "DrawSceneDocumentEntities" not in body:
                errors.append("DrawRuntimeEntities must not hand-roll root-only draw")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_child_instance_draw: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_child_instance_draw: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())