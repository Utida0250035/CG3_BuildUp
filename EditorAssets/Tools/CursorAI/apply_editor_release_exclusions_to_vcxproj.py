#!/usr/bin/env python3
"""エディタ専用 ClCompile に Release ExcludedFromBuild を付与（冪等）。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
VCPROJ = ROOT / "Project" / "CG2_BuildUp.vcxproj"
ALLOWLIST_PATH = (
    ROOT / "EditorAssets" / "Tools" / "CursorAI" / "editor_sources_release_link_allowlist.txt"
)

RELEASE_LINE = (
    '      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Release|x64\'">'
    "true</ExcludedFromBuild>"
)
DEV_LINE = (
    '      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Development|x64\'">'
    "false</ExcludedFromBuild>"
)
EDITOR_PREFIX = "Codes/Source/System/Editor/"
SCENE_EDITOR_PREFIXES = (
    "Codes/Source/Scene/SceneEntityEditor",
    "Codes/Source/Scene/SceneParticleEditor",
)
DEV_BOOT = {
    "Codes/Source/Scene/CommandSceneDevelopmentBoot.cpp",
    "Codes/Source/Scene/CommandSceneDevelopmentBootApply.cpp",
}


def load_allowlist() -> set[str]:
    allowed: set[str] = set()
    for line in ALLOWLIST_PATH.read_text(encoding="utf-8").splitlines():
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        allowed.add(line.replace("/", "\\"))
    return allowed


def normalize_path(path: str) -> str:
    return path.replace("\\", "/")


def needs_exclude(path: str, allowed: set[str]) -> bool:
    normalized = normalize_path(path)
    allowed_norm = {normalize_path(p) for p in allowed}
    if normalized in allowed_norm:
        return False
    if normalized.startswith(EDITOR_PREFIX):
        return True
    if any(normalized.startswith(p) for p in SCENE_EDITOR_PREFIXES):
        return True
    if normalized in DEV_BOOT:
        return True
    return False


def exclusion_block(indent: str) -> str:
    return f"{RELEASE_LINE}\n{DEV_LINE}\n{indent}"


def patch(text: str, allowed: set[str]) -> tuple[str, int]:
    changed = 0

    def on_self_close(match: re.Match[str]) -> str:
        nonlocal changed
        indent, path = match.group(1), match.group(2)
        if not needs_exclude(path, allowed):
            return match.group(0)
        changed += 1
        return (
            f'{indent}<ClCompile Include="{path}">\n'
            f"{RELEASE_LINE}\n"
            f"{DEV_LINE}\n"
            f"{indent}</ClCompile>"
        )

    text = re.sub(
        r'(\s*)<ClCompile Include="([^"]+)"\s*/>',
        on_self_close,
        text,
    )

    def on_block(match: re.Match[str]) -> str:
        nonlocal changed
        indent, path, inner = match.group(1), match.group(2), match.group(3)
        if not needs_exclude(path, allowed):
            return match.group(0)
        if "Release|x64" in inner and "ExcludedFromBuild" in inner:
            return match.group(0)
        changed += 1
        return (
            f'{indent}<ClCompile Include="{path}">\n'
            f"{RELEASE_LINE}\n"
            f"{DEV_LINE}\n"
            f"{inner.rstrip()}\n"
            f"{indent}</ClCompile>"
        )

    text = re.sub(
        r'(\s*)<ClCompile Include="([^"]+)">\s*(.*?)\s*</ClCompile>',
        on_block,
        text,
        flags=re.DOTALL,
    )
    return text, changed


def main() -> int:
    if not VCPROJ.is_file():
        print("missing vcxproj", file=sys.stderr)
        return 1
    allowed = load_allowlist()
    original = VCPROJ.read_text(encoding="utf-8")
    updated, count = patch(original, allowed)
    if updated != original:
        VCPROJ.write_text(updated, encoding="utf-8")
    print(f"apply_editor_release_exclusions_to_vcxproj: updated {count} entries")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())