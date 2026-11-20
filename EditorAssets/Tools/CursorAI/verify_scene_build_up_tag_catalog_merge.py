#!/usr/bin/env python3
"""BuildUp のシーン JSON 読込後に tagCatalog マージを行うことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    load_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpSceneDocumentLoad.cpp"
    tag_cpp = ROOT / "Project" / "Codes" / "Source" / "Scene" / "SceneBuildUpSceneDocumentTagCatalog.cpp"

    if load_cpp.is_file():
        text = load_cpp.read_text(encoding="utf-8", errors="replace")
        if "ApplyAfterJsonLoad" not in text:
            errors.append("TryLoad must call ApplyAfterJsonLoad after TryLoadSceneDocument")
    else:
        errors.append("missing SceneBuildUpSceneDocumentLoad.cpp")

    if tag_cpp.is_file():
        text = tag_cpp.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "MergeTagCatalogFromRegistry",
            "SyncAllEntityDefinitionTags",
            "SyncTagCatalogFromInstances",
        ):
            if needle not in text:
                errors.append(f"tag catalog module must call {needle}")
    else:
        errors.append("missing SceneBuildUpSceneDocumentTagCatalog.cpp")

    if errors:
        print("verify_scene_build_up_tag_catalog_merge: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_scene_build_up_tag_catalog_merge: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())