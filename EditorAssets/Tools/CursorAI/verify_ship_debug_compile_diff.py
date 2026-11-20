#!/usr/bin/env python3
"""vcxproj の 3 構成マクロが出荷契約と一致するか検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
VCPROJ = ROOT / "Project" / "CG2_BuildUp.vcxproj"
CONTRACT_CPP = (
    ROOT / "Project" / "Codes" / "Source" / "System" / "Build" / "BuildConfigurationContract.cpp"
)
MEMO = (
    ROOT
    / "EditorAssets"
    / "Tools"
    / "CursorAI"
    / "docs"
    / "feature-memos"
    / "ship_build_configurations.md"
)

CONFIG_SPECS: dict[str, dict[str, set[str]]] = {
    "Release|x64": {
        "must": {"NDEBUG", "_WINDOWS"},
        "must_not": {"DEVELOPMENT", "USE_IMGUI", "_DEBUG", "MSVC_DEBUG_CONFIGURATION"},
    },
    "Development|x64": {
        "must": {"NDEBUG", "DEVELOPMENT", "USE_IMGUI", "_WINDOWS"},
        "must_not": {"_DEBUG", "MSVC_DEBUG_CONFIGURATION"},
    },
    "Debug|x64": {
        "must": {"_DEBUG", "DEVELOPMENT", "USE_IMGUI", "MSVC_DEBUG_CONFIGURATION", "_WINDOWS"},
        "must_not": set(),
    },
}


def parse_preprocessor_definitions(vcxproj_text: str, config_platform: str) -> set[str]:
    condition = f"'$(Configuration)|$(Platform)'=='{config_platform}'"
    pattern = (
        rf"<ItemDefinitionGroup Condition=\"{re.escape(condition)}\">"
        r".*?<PreprocessorDefinitions>([^<]+)</PreprocessorDefinitions>"
    )
    match = re.search(pattern, vcxproj_text, flags=re.DOTALL)
    if not match:
        return set()
    raw = match.group(1)
    tokens: set[str] = set()
    for part in raw.replace("%(PreprocessorDefinitions)", "").split(";"):
        token = part.strip()
        if token and not token.startswith("%"):
            tokens.add(token.split("=")[0])
    return tokens


def main() -> int:
    errors: list[str] = []
    if not VCPROJ.is_file():
        errors.append("missing CG2_BuildUp.vcxproj")
        text = ""
    else:
        text = VCPROJ.read_text(encoding="utf-8", errors="replace")
        for config_platform, spec in CONFIG_SPECS.items():
            defs = parse_preprocessor_definitions(text, config_platform)
            if not defs:
                errors.append(f"{config_platform}: PreprocessorDefinitions not found")
                continue
            for token in spec["must"]:
                if token not in defs:
                    errors.append(f"{config_platform}: expected macro {token}")
            for token in spec["must_not"]:
                if token in defs:
                    errors.append(f"{config_platform}: must not define {token}")

    if not CONTRACT_CPP.is_file():
        errors.append("missing BuildConfigurationContract.cpp")
    elif "IsShipCandidateBuild" not in CONTRACT_CPP.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("BuildConfigurationContract.cpp incomplete")

    if not MEMO.is_file():
        errors.append("missing ship_build_configurations.md")
    elif "出荷候補" not in MEMO.read_text(encoding="utf-8", errors="replace"):
        errors.append("ship_build_configurations.md must document ship candidate")

    if errors:
        print("verify_ship_debug_compile_diff: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_ship_debug_compile_diff: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())