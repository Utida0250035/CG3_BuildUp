#!/usr/bin/env python3
"""出荷候補 Release|x64 のリンクが PDB を生成しない設定か検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
VCPROJ = ROOT / "Project" / "CG2_BuildUp.vcxproj"
MEMO = ROOT / "EditorAssets" / "Tools" / "CursorAI" / "docs" / "feature-memos" / "ship_build_configurations.md"

RELEASE_GROUP = re.compile(
    r"<ItemDefinitionGroup Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=='Release\|x64'\">"
    r"(.*?)</ItemDefinitionGroup>",
    re.DOTALL,
)


def main() -> int:
    errors: list[str] = []

    if not VCPROJ.is_file():
        errors.append("missing CG2_BuildUp.vcxproj")
    else:
        text = VCPROJ.read_text(encoding="utf-8", errors="replace")
        match = RELEASE_GROUP.search(text)
        if not match:
            errors.append("missing Release|x64 ItemDefinitionGroup")
        else:
            block = match.group(1)
            gen = re.search(
                r"<GenerateDebugInformation>(true|false)</GenerateDebugInformation>",
                block,
            )
            if not gen:
                errors.append("Release|x64 Link: missing GenerateDebugInformation")
            elif gen.group(1).lower() == "true":
                errors.append("Release|x64 Link: GenerateDebugInformation must be false for ship")

    if MEMO.is_file():
        memo = MEMO.read_text(encoding="utf-8", errors="replace")
        if "GenerateDebugInformation" not in memo:
            errors.append("ship_build_configurations.md must document GenerateDebugInformation")
    else:
        errors.append("missing ship_build_configurations.md")

    if errors:
        print("verify_release_link_generate_debug_info: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_release_link_generate_debug_info: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())