#!/usr/bin/env python3
"""JsonTest デバッグパネル TU に Release ExcludedFromBuild を付与（冪等）。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
VCPROJ = ROOT / "Project" / "CG2_BuildUp.vcxproj"
JSON_TEST_CPP = r"Codes\Source\System\Json\JsonTest.cpp"

RELEASE_LINE = (
    '      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Release|x64\'">'
    "true</ExcludedFromBuild>"
)
DEV_LINE = (
    '      <ExcludedFromBuild Condition="\'$(Configuration)|$(Platform)\'==\'Development|x64\'">'
    "false</ExcludedFromBuild>"
)


def patch(text: str) -> tuple[str, bool]:
    pattern = rf'(\s*)<ClCompile Include="{re.escape(JSON_TEST_CPP)}"\s*/>'
    match = re.search(pattern, text)
    if not match:
        if JSON_TEST_CPP.replace("\\", "/") in text and "Release|x64" in text:
            # already expanded block
            block_pat = (
                rf'<ClCompile Include="{re.escape(JSON_TEST_CPP)}">.*?</ClCompile>'
            )
            block = re.search(block_pat, text, flags=re.DOTALL)
            if block and "Release|x64" in block.group(0) and "ExcludedFromBuild" in block.group(0):
                return text, False
        return text, False
    indent = match.group(1)
    replacement = (
        f'{indent}<ClCompile Include="{JSON_TEST_CPP}">\n'
        f"{RELEASE_LINE}\n"
        f"{DEV_LINE}\n"
        f"{indent}</ClCompile>"
    )
    return text[: match.start()] + replacement + text[match.end() :], True


def main() -> int:
    if not VCPROJ.is_file():
        print("missing vcxproj", file=sys.stderr)
        return 1
    original = VCPROJ.read_text(encoding="utf-8")
    updated, changed = patch(original)
    if changed:
        VCPROJ.write_text(updated, encoding="utf-8")
        print("apply_json_test_release_exclusion_to_vcxproj: updated JsonTest.cpp")
    else:
        print("apply_json_test_release_exclusion_to_vcxproj: no change needed")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())