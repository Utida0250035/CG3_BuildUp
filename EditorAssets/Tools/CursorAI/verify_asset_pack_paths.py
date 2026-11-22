#!/usr/bin/env python3
"""ランタイム参照パスとリポジトリ同梱アセット（Assets / EditorAssets）の整合を検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
PROJECT = ROOT / "Project"
GAME_PATHS_H = PROJECT / "Codes" / "Include" / "System" / "GameAssetsPaths.h"
EDITOR_PATHS_H = (
    PROJECT / "Codes" / "Include" / "System" / "Editor" / "EditorAssetsPaths.h"
)
MAIN_CPP = PROJECT / "Codes" / "Source" / "Application" / "main.cpp"
VCPROJ = PROJECT / "CG2_BuildUp.vcxproj"
MEMO = (
    ROOT
    / "EditorAssets"
    / "Tools"
    / "CursorAI"
    / "docs"
    / "feature-memos"
    / "asset_pack_verify.md"
)

STRING_LITERAL = re.compile(r'"((?:\./|\.\./)?[^"]+)"')

# PlaySe の ./Resources/... は任意フォールバック（Project/Resources 無し想定）
OPTIONAL_PREFIXES = ("./Resources/",)

EXTRA_REQUIRED = (
    "./Assets/Images/white4x4.png",
)


def repo_path_for_runtime(runtime_path: str) -> Path | None:
    norm = runtime_path.replace("\\", "/")
    if norm.startswith("./Assets/"):
        return PROJECT / "Assets" / norm.removeprefix("./Assets/")
    if norm.startswith("../EditorAssets/"):
        return ROOT / "EditorAssets" / norm.removeprefix("../EditorAssets/")
    if norm.startswith("Assets/"):
        return PROJECT / norm
    if norm.startswith("./Resources/"):
        return PROJECT / "Resources" / norm.removeprefix("./Resources/")
    return None


def collect_literals(*files: Path) -> set[str]:
    paths: set[str] = set()
    for file in files:
        if not file.is_file():
            continue
        text = file.read_text(encoding="utf-8", errors="replace")
        for match in STRING_LITERAL.finditer(text):
            raw = match.group(1)
            if not (
                raw.startswith("./Assets/")
                or raw.startswith("../EditorAssets/")
                or raw.startswith("Assets/")
            ):
                continue
            paths.add(raw.replace("\\", "/"))
    return paths


def is_optional(runtime_path: str) -> bool:
    return any(runtime_path.startswith(p) for p in OPTIONAL_PREFIXES)


def main() -> int:
    errors: list[str] = []

    literals = collect_literals(GAME_PATHS_H, EDITOR_PATHS_H, MAIN_CPP)
    literals.update(EXTRA_REQUIRED)

    for runtime_path in sorted(literals):
        if is_optional(runtime_path):
            continue
        resolved = repo_path_for_runtime(runtime_path)
        if resolved is None:
            errors.append(f"unmapped runtime path: {runtime_path}")
            continue
        if resolved.suffix:
            if not resolved.is_file():
                errors.append(f"missing file: {runtime_path} -> {resolved}")
        elif not resolved.is_dir():
            errors.append(f"missing directory: {runtime_path} -> {resolved}")

    if VCPROJ.is_file():
        vcx = VCPROJ.read_text(encoding="utf-8", errors="replace")
        if 'xcopy /E /I /Y "$(ProjectDir)Assets"' not in vcx:
            errors.append("vcxproj PostBuild must copy Project/Assets")
        if 'xcopy /E /I /Y "$(ProjectDir)..\\EditorAssets"' not in vcx:
            errors.append("vcxproj PostBuild must copy EditorAssets")
    else:
        errors.append("missing CG2_BuildUp.vcxproj")

    if not MEMO.is_file():
        errors.append("missing asset_pack_verify.md")

    if errors:
        print("verify_asset_pack_paths: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_asset_pack_paths: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())