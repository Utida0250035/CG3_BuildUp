#!/usr/bin/env python3
"""EditorScenePlayRequest がパス以外に起動意図メタデータを保持することを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
META_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorScenePlayRequestMetadata.h"
STAGING_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorScenePlayRequestStaging.cpp"
BUILDUP_ROUTE = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorScenePlayBuildUpRoute.cpp"
IN_EDITOR = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditorInEditorPlayQueue.cpp"
BUILDUP_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUp.cpp"
ENTITY_CPP = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneEntityEditor.cpp"


def main() -> int:
    errors: list[str] = []

    if not META_H.is_file():
        errors.append("missing EditorScenePlayRequestMetadata.h")
    else:
        meta = META_H.read_text(encoding="utf-8", errors="replace")
        for kind in ("BuildUpLaunch", "InEditorRuntimePreview", "RepositoryStartup"):
            if kind not in meta:
                errors.append(f"staging kind {kind} missing")

    if not STAGING_CPP.is_file():
        errors.append("missing EditorScenePlayRequestStaging.cpp")
    else:
        staging = STAGING_CPP.read_text(encoding="utf-8", errors="replace")
        if "stagingKind_" not in staging:
            errors.append("PlayRequest must store staging kind")
        if "StagingKind" not in staging:
            errors.append("StagingKind accessor missing")

    for path, kind in (
        (BUILDUP_ROUTE, "BuildUpLaunch"),
        (IN_EDITOR, "InEditorRuntimePreview"),
    ):
        if not path.is_file():
            errors.append(f"missing {path.name}")
        elif kind not in path.read_text(encoding="utf-8", errors="replace"):
            errors.append(f"{path.name} must stage {kind}")

    for path, label in ((BUILDUP_CPP, "SceneBuildUp"), (ENTITY_CPP, "SceneEntityEditor")):
        if path.is_file():
            if "ClearStagedEditorPlayScenePath" not in path.read_text(
                encoding="utf-8", errors="replace"
            ):
                errors.append(f"{label} must clear staged play request on exit")
        else:
            errors.append(f"missing {path.name}")

    if errors:
        print("verify_editor_scene_play_request_metadata: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_play_request_metadata: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())