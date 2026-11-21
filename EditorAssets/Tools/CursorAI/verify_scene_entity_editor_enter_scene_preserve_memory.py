#!/usr/bin/env python3
"""SceneEntityEditor 再入場でメモリ上のシーン編集をディスク読込で潰さないか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
SCENE = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
POLICY = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Scene"
    / "SceneEntityEditorEnterSceneDocumentLoadPolicy.cpp"
)


def main() -> int:
    errors: list[str] = []

    if not POLICY.is_file():
        errors.append("missing SceneEntityEditorEnterSceneDocumentLoadPolicy.cpp")
    else:
        text = POLICY.read_text(encoding="utf-8", errors="replace")
        if "hasEnteredSceneEntityEditorBefore" not in text:
            errors.append("policy must skip disk load on re-enter")
        if "pendingBootJsonPath" not in text:
            errors.append("policy must honor pending boot path")

    if SCENE.is_file():
        sc = SCENE.read_text(encoding="utf-8", errors="replace")
        if "PlanEnterSceneDocumentLoad" not in sc:
            errors.append("EnterScene must use document load policy")
        enter = sc.split("void SceneEntityEditor::EnterScene()", 1)
        if len(enter) < 2:
            errors.append("missing EnterScene")
        else:
            body = enter[1].split("void SceneEntityEditor::", 1)[0]
            if "LoadFromJsonPath" in body and "loadPlan.loadFromDisk" not in body:
                errors.append("LoadFromJsonPath must be conditional on loadPlan")
            if "hasEnteredSceneEntityEditor_" not in body:
                errors.append("EnterScene must track hasEnteredSceneEntityEditor_")
    else:
        errors.append("missing SceneEntityEditor.cpp")

    if errors:
        print(
            "verify_scene_entity_editor_enter_scene_preserve_memory: FAILED",
            file=sys.stderr,
        )
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_enter_scene_preserve_memory: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())