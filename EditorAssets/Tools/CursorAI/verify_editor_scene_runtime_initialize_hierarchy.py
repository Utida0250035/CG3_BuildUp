#!/usr/bin/env python3
"""EditorSceneRuntime が親子インスタンスを含めて Initialize する契約を検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
RUNTIME_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "Scene" / "EditorSceneRuntime.cpp"
POLICY_CPP = ROOT / "Project" / "Codes" / "Source" / "System" / "Editor" / "EditorSceneRuntimeEntityInitializePolicy.cpp"
POLICY_H = ROOT / "Project" / "Codes" / "Include" / "System" / "Editor" / "EditorSceneRuntimeEntityInitializePolicy.h"


def main() -> int:
    errors: list[str] = []

    if not POLICY_H.is_file() or not POLICY_CPP.is_file():
        errors.append("missing EditorSceneRuntimeEntityInitializePolicy")
    else:
        policy = POLICY_CPP.read_text(encoding="utf-8", errors="replace")
        if "ResolveDependencies" not in policy:
            errors.append("policy must ResolveDependencies on all instances")
        if "GetParent() == nullptr" not in policy:
            errors.append("policy must initialize scene roots")
        if "NeedsSceneEntityInitialize" not in policy and "GetState()" not in policy:
            errors.append("policy must skip already-Active entities")
        if "kMaxOrphanInitPasses" not in policy:
            errors.append("policy must multi-pass orphan/child Initialize")
        if "parentIndex" in policy:
            errors.append("policy must not gate Initialize on parentIndex in document")

    if not RUNTIME_CPP.is_file():
        errors.append("missing EditorSceneRuntime.cpp")
    else:
        runtime = RUNTIME_CPP.read_text(encoding="utf-8", errors="replace")
        if "InitializeSceneDocumentEntities" not in runtime:
            errors.append("BuildEntities must call InitializeSceneDocumentEntities")
        if "EditorSceneRuntimeEntityInitializePolicy.h" not in runtime:
            errors.append("EditorSceneRuntime must include initialize policy")
        build_section = runtime.split("BuildEntitiesFromSceneDocument", 1)
        if len(build_section) > 1:
            body = build_section[1][:800]
            if "->Initialize()" in body and "InitializeSceneDocumentEntities" not in body:
                errors.append("BuildEntities must delegate Initialize to policy")

    if errors:
        print("verify_editor_scene_runtime_initialize_hierarchy: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_editor_scene_runtime_initialize_hierarchy: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())