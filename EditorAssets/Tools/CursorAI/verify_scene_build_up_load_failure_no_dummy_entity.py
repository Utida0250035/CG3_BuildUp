#!/usr/bin/env python3
"""SceneBuildUp 読込失敗時に Entity_0 を生成しないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
POLICY = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpLoadFailureEntities.cpp"
NOTICE = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpSceneLoadFailure.cpp"


def main() -> int:
    errors: list[str] = []

    if not POLICY.is_file():
        errors.append("missing SceneBuildUpLoadFailureEntities.cpp")
    elif "loadSucceeded" not in POLICY.read_text(encoding="utf-8", errors="replace"):
        errors.append("load-failure policy must gate on loadSucceeded")

    if BUILDUP.is_file():
        text = BUILDUP.read_text(encoding="utf-8", errors="replace")
        enter = text.split("SceneBuildUp::EnterScene()", 1)
        if len(enter) > 1:
            body = enter[1].split("void SceneBuildUp::", 1)[0]
            if "MakeDefaultEntityDefinition" in body:
                errors.append("EnterScene must not use MakeDefaultEntityDefinition")
            if "Entity_0" in body:
                errors.append("EnterScene must not hardcode Entity_0")
            if "BuildEntitiesAfterDocumentLoad" not in body:
                errors.append("EnterScene must use BuildEntitiesAfterDocumentLoad")
        else:
            errors.append("missing EnterScene")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if NOTICE.is_file():
        if "デフォルト 1 体は生成しません" not in NOTICE.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("failure notice must state no default entity")
    else:
        errors.append("missing SceneBuildUpSceneLoadFailure.cpp")

    if errors:
        print("verify_scene_build_up_load_failure_no_dummy_entity: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_load_failure_no_dummy_entity: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())