#!/usr/bin/env python3
"""SetupPreviewCamera がプレビュー CmpCamera を反映することを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    setup_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorPreviewCameraSetup.cpp"
    fallback_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "Scene"
        / "SceneEntityEditorPreviewStageViewCameraFallback.cpp"
    )
    entity_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"
    entity_h = ROOT / "Project" / "Codes" / "Include" / "Scene" / "SceneEntityEditor.h"

    if not setup_cpp.is_file():
        errors.append("missing SceneEntityEditorPreviewCameraSetup.cpp")

    if entity_h.is_file():
        if "GetSupplementalCameraEntities" not in entity_h.read_text(
            encoding="utf-8", errors="replace"
        ):
            errors.append("SceneEntityEditor.h: expected GetSupplementalCameraEntities")

    if entity_cpp.is_file():
        text = entity_cpp.read_text(encoding="utf-8", errors="replace")
        if "ApplyPrimaryFromPreviewEntities" not in text:
            errors.append("SetupPreviewCamera must use ApplyPrimaryFromPreviewEntities")
        if "ApplyPreviewStageViewCameraFallback" not in text:
            errors.append("SetupPreviewCamera must fallback to StageView camera")
        setup_fn = text.split("void SceneEntityEditor::SetupPreviewCamera", 1)
        if len(setup_fn) > 1:
            body = setup_fn[1].split("void SceneEntityEditor::", 1)[0]
            if "1.6f, -6.0f" in body:
                errors.append("SetupPreviewCamera: hardcoded free camera translate")

    if fallback_cpp.is_file():
        fb = fallback_cpp.read_text(encoding="utf-8", errors="replace")
        if "ResolveStageViewFramingBounds" not in fb:
            errors.append("fallback must resolve stage framing from document")
        if "SetupStageViewCamera" not in fb:
            errors.append("fallback must call SetupStageViewCamera")
    else:
        errors.append("missing SceneEntityEditorPreviewStageViewCameraFallback.cpp")

    if errors:
        print("verify_scene_entity_editor_preview_camera: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_entity_editor_preview_camera: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())