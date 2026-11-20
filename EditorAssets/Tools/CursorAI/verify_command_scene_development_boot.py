#!/usr/bin/env python3
"""Development 起動が ResolveDevelopmentBootTarget 経由であることを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
CODES = ROOT / "Project" / "Codes"


def main() -> int:
    errors: list[str] = []
    command_cpp = CODES / "Source" / "Scene" / "CommandScene.cpp"
    boot_cpp = CODES / "Source" / "Scene" / "CommandSceneDevelopmentBoot.cpp"
    apply_cpp = CODES / "Source" / "Scene" / "CommandSceneDevelopmentBootApply.cpp"

    if command_cpp.is_file():
        text = command_cpp.read_text(encoding="utf-8", errors="replace")
        ctor = text.split("CommandScene::CommandScene()", 1)
        if len(ctor) > 1:
            chunk = ctor[1].split("sceneNow_->EnterScene", 1)[0]
            if "CreateInitialSceneForDevelopmentBoot" not in chunk:
                errors.append("CommandScene constructor must use BootApply")
            if "SceneEntityEditor::Instance()" in chunk:
                errors.append(
                    "CommandScene must not hardcode SceneEntityEditor::Instance"
                )
    else:
        errors.append("missing CommandScene.cpp")

    if boot_cpp.is_file():
        text = boot_cpp.read_text(encoding="utf-8", errors="replace")
        if "kDebugPlayAreaTemplateScene" not in text:
            errors.append("boot resolver needs DebugPlayArea template fallback")
        if "DevelopmentBootKind::SceneBuildUp" not in text:
            errors.append("boot resolver must assign SceneBuildUp kind")

    if not apply_cpp.is_file():
        errors.append("missing CommandSceneDevelopmentBootApply.cpp")

    if errors:
        print("verify_command_scene_development_boot: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_command_scene_development_boot: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())