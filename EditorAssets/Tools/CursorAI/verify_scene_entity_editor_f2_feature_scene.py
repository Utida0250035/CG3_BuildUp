#!/usr/bin/env python3
"""F2 が DebugPlayArea 機能検証 BuildUp に割り当てられているか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    hotkeys_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorDebugHotkeys.cpp"
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"

    if not hotkeys_cpp.is_file():
        errors.append("missing SceneEntityEditorDebugHotkeys.cpp")
        return report(errors)

    text = hotkeys_cpp.read_text(encoding="utf-8", errors="replace")
    if "Key::F2" not in text:
        errors.append("F2 handler missing")
    if "kDebugPlayAreaTemplateScene" not in text:
        errors.append("F2 must launch kDebugPlayAreaTemplateScene")
    if "QueueFadeToBuildUp" not in text:
        errors.append("F2 must use QueueFadeToBuildUp")
    if "Key::F2" in text and "Key::F4" in text:
        f2_idx = text.index("Key::F2")
        f4_idx = text.index("Key::F4")
        f2_block = text[f2_idx:f4_idx]
        if "SceneParticleEditor" in f2_block:
            errors.append("F2 must not open SceneParticleEditor")
        if "Key::F4" in f2_block:
            errors.append("F2 block must end before F4 handler")
    if "Key::F4" in text and "SceneParticleEditor" not in text:
        errors.append("F4 should open SceneParticleEditor")

    if entity_cpp.is_file():
        entity = entity_cpp.read_text(encoding="utf-8", errors="replace")
        if "SceneEntityEditorDebugHotkeys::Tick" not in entity:
            errors.append("SceneEntityEditor must call DebugHotkeys::Tick")

    return report(errors)


def report(errors: list[str]) -> int:
    if errors:
        print("verify_scene_entity_editor_f2_feature_scene: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1
    print("verify_scene_entity_editor_f2_feature_scene: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())