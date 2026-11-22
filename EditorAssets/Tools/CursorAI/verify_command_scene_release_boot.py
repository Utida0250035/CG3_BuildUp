#!/usr/bin/env python3
"""Release 起動が未設定時 Title へ落ちることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes"


def main() -> int:
    errors: list[str] = []
    command_cpp = CODES / "Source" / "Scene" / "CommandScene.cpp"
    apply_cpp = CODES / "Source" / "Scene" / "CommandSceneReleaseBootApply.cpp"

    if command_cpp.is_file():
        text = command_cpp.read_text(encoding="utf-8", errors="replace")
        ctor = text.split("CommandScene::CommandScene()", 1)
        if len(ctor) > 1:
            chunk = ctor[1].split("sceneNow_->EnterScene", 1)[0]
            release = chunk.split("#else", 1)
            if len(release) > 1:
                body = release[1].split("#endif", 1)[0]
                if "CreateInitialSceneForReleaseBoot" not in body:
                    errors.append(
                        "CommandScene non-DEVELOPMENT must use ReleaseBootApply"
                    )
                if "kDebugPlayAreaTemplateScene" in body:
                    errors.append(
                        "CommandScene must not fall back to DebugPlayArea template"
                    )
    else:
        errors.append("missing CommandScene.cpp")

    if apply_cpp.is_file():
        text = apply_cpp.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaTemplateScene" in text:
            errors.append("Release boot must not use DebugPlayArea template")
        if "SceneTitle::Instance()" not in text:
            errors.append("Release boot must use SceneTitle fallback")
    else:
        errors.append("missing CommandSceneReleaseBootApply.cpp")

    if errors:
        print("verify_command_scene_release_boot: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_command_scene_release_boot: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())