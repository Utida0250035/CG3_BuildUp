#!/usr/bin/env python3
"""未登録コンポーネント種別を Draw 既定にしないか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RESOLVE = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Entity"
    / "EntityEditorComponentSlotKindResolve.cpp"
)
DEF = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Entity" / "EntityEditorDefinition.cpp"
WARN = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "System"
    / "Editor"
    / "Scene"
    / "EditorSceneRuntimeBuildWarnings.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not RESOLVE.is_file():
        errors.append("missing EntityEditorComponentSlotKindResolve.cpp")
    else:
        text = RESOLVE.read_text(encoding="utf-8", errors="replace")
        if "return std::nullopt" not in text:
            errors.append("resolver must return nullopt for unknown types")
        if "return ComponentSlotKind::Draw" in text.split("TryDefaultKindForRegisteredType", 1)[
            -1
        ].split("}", 1)[0]:
            errors.append("resolver must not use Draw as final fallback")

    if DEF.is_file():
        fj = DEF.read_text(encoding="utf-8", errors="replace")
        from_json = fj.split("void from_json(const nlohmann::json &j, ComponentSlot &slot)", 1)
        if len(from_json) < 2:
            errors.append("missing ComponentSlot from_json")
        else:
            body = from_json[1].split("void from_json", 1)[0]
            if "slot.kind = ComponentSlotKind::Draw" in body:
                errors.append("from_json must not assign Draw for unknown types")
            if "TryDefaultKindForRegisteredType" not in body:
                errors.append("from_json must use TryDefaultKindForRegisteredType")
    else:
        errors.append("missing EntityEditorDefinition.cpp")

    if WARN.is_file():
        if "TryDefaultKindForRegisteredType" not in WARN.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("BuildWarnings must use TryDefaultKindForRegisteredType")
    else:
        errors.append("missing EditorSceneRuntimeBuildWarnings.cpp")

    if errors:
        print("verify_entity_editor_unknown_component_slot_kind: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_entity_editor_unknown_component_slot_kind: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())