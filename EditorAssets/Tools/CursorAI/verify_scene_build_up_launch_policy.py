#!/usr/bin/env python3
"""BuildUp Launch が QueueFadeToBuildUp 経由に寄せられているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    policy_h = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneBuildUpLaunchPolicy.h"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"

    if not policy_h.is_file():
        errors.append("missing SceneBuildUpLaunchPolicy.h")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        if "QueueFadeToBuildUp" not in text:
            errors.append("SceneEntityEditor: expected QueueFadeToBuildUp")
        if "Registry::Instance().Launch(scenePath" in text:
            errors.append(
                "SceneEntityEditor: raw Launch+Set should use QueueFadeToBuildUp"
            )

    if errors:
        print("verify_scene_build_up_launch_policy: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_launch_policy: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())