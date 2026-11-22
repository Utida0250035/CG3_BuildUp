#!/usr/bin/env python3
"""インスタンス削除時に子を祖父（残った祖先）へ付け替える経路を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
MOD = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorRemoveSelectedInstances.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not MOD.is_file():
        errors.append("missing EntityEditorRemoveSelectedInstances.cpp")
    else:
        text = MOD.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "ResolveParentIndexAfterRemoval",
            "originalScene.instances",
        ):
            if needle not in text:
                errors.append(f"remove module missing: {needle}")
        if "instance.parentIndex = -1" in text and "removal.contains(instance.parentIndex)" in text:
            errors.append("must not flatten children to root when parent removed")

    if errors:
        print("verify_entity_editor_remove_promote_child: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_remove_promote_child: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())