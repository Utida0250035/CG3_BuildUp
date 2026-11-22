#!/usr/bin/env python3
"""BuildEntities が instanceName をランタイム Entity に反映するか検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"
POLICY_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorSceneRuntimeInstanceNamePolicy.cpp"


def main() -> int:
    errors: list[str] = []

    if not POLICY_CPP.is_file():
        errors.append("missing EditorSceneRuntimeInstanceNamePolicy.cpp")
    else:
        policy = POLICY_CPP.read_text(encoding="utf-8", errors="replace")
        if "instanceName" not in policy:
            errors.append("policy must read instance.instanceName")
        if "SetHash" not in policy:
            errors.append("policy must call Entity::SetHash")

    if not RUNTIME_CPP.is_file():
        errors.append("missing EditorSceneRuntime.cpp")
    else:
        text = RUNTIME_CPP.read_text(encoding="utf-8", errors="replace")
        if "ApplySceneInstanceName" not in text:
            errors.append("BuildEntities must call ApplySceneInstanceName")
        if "EditorSceneRuntimeInstanceNamePolicy.h" not in text:
            errors.append("EditorSceneRuntime must include instance name policy")
        build = text.split("BuildEntitiesFromSceneDocument", 1)
        if len(build) > 1:
            body = build[1][:500]
            if "BuildEntityFromDefinition(instance.definition)" in body and "ApplySceneInstanceName" not in body:
                errors.append("must not build from definition only")

    if errors:
        print("verify_editor_scene_runtime_instance_name: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_instance_name: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())