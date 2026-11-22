#!/usr/bin/env python3
"""BuildUp が camera==nullptr のとき無言 return しないことを検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
BUILDUP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
FEEDBACK = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Scene"
    / "SceneBuildUpMissingPrimaryCameraFeedback.cpp"
)

SILENT_RETURN = re.compile(
    r"if\s*\(\s*camera\s*==\s*nullptr\s*\)\s*\{\s*return\s*;\s*\}",
    re.MULTILINE,
)


def main() -> int:
    errors: list[str] = []

    if not FEEDBACK.is_file():
        errors.append("missing SceneBuildUpMissingPrimaryCameraFeedback.cpp")
    elif "HandleSceneBuildUpDrawWithoutPrimaryCamera" not in FEEDBACK.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("feedback module must handle missing primary camera")

    if BUILDUP.is_file():
        text = BUILDUP.read_text(encoding="utf-8", errors="replace")
        if SILENT_RETURN.search(text):
            errors.append("must not silently return on camera == nullptr")
        if "HandleSceneBuildUpDrawWithoutPrimaryCamera" not in text:
            errors.append("SceneBuildUp must call missing-camera feedback")
        if "DrawSceneBuildUpMissingPrimaryCameraNoticeIfPending" not in text:
            errors.append("Draw must show missing-camera notice when pending")
    else:
        errors.append("missing SceneBuildUp.cpp")

    if errors:
        print("verify_scene_build_up_camera_null_feedback: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_camera_null_feedback: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())