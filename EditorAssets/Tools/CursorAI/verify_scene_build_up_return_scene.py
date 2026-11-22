#!/usr/bin/env python3
"""SceneBuildUp が returnScene null でも F1 復帰できることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes"


def main() -> int:
    errors: list[str] = []
    buildup_cpp = CODES / "Source" / "Scene" / "SceneBuildUp.cpp"
    target_cpp = CODES / "Source" / "Scene" / "SceneBuildUpReturnTarget.cpp"

    if not target_cpp.is_file():
        errors.append("missing SceneBuildUpReturnTarget.cpp")
    else:
        text = target_cpp.read_text(encoding="utf-8", errors="replace")
        if "SceneEntityEditor::Instance()" not in text and "DEVELOPMENT" in text:
            pass
        if "ResolveSceneBuildUpReturnTarget" not in text:
            errors.append("ReturnTarget must define ResolveSceneBuildUpReturnTarget")
        if "SceneTitle::Instance()" not in text:
            errors.append("ReturnTarget needs Release fallback Title")

    if buildup_cpp.is_file():
        text = buildup_cpp.read_text(encoding="utf-8", errors="replace")
        update = text.split("SceneBuildUp::Update()", 1)
        if len(update) > 1:
            body = update[1].split("void SceneBuildUp::", 1)[0]
            if "ResolveSceneBuildUpReturnTarget" not in body:
                errors.append("Update must resolve return target")
            if "returnScene_ != nullptr" in body:
                errors.append("Update must not gate F1 on returnScene_ != nullptr")
        else:
            errors.append("missing SceneBuildUp::Update")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_return_scene: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_return_scene: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())