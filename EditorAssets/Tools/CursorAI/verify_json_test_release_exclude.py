#!/usr/bin/env python3
"""JsonTest.cpp が Release ビルドから除外されているか検査。"""

from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
VCPROJ = ROOT / "Project" / "CG2_BuildUp.vcxproj"
JSON_TEST_CPP = "Codes/Source/System/Json/JsonTest.cpp"
RELEASE_MARK = "Release|x64"


def main() -> int:
    errors: list[str] = []
    if not VCPROJ.is_file():
        errors.append("missing CG2_BuildUp.vcxproj")
    else:
        text = VCPROJ.read_text(encoding="utf-8", errors="replace")
        pattern = re.compile(
            r'<ClCompile Include="([^"]+)"([^/]*)(?:/>|>(.*?)</ClCompile>)',
            re.DOTALL,
        )
        found = False
        for match in pattern.finditer(text):
            path = match.group(1).replace("\\", "/")
            if path != JSON_TEST_CPP:
                continue
            found = True
            block = match.group(0)
            if RELEASE_MARK not in block or "ExcludedFromBuild" not in block:
                errors.append("JsonTest.cpp missing Release ExcludedFromBuild")
        if not found:
            errors.append("JsonTest.cpp not listed in vcxproj")

    src = ROOT / "Project" / "Codes" / "Source" / "System" / "Json" / "JsonTest.cpp"
    if src.is_file():
        body = src.read_text(encoding="utf-8", errors="replace")
        if "USE_IMGUI" not in body and "DEVELOPMENT" not in body:
            errors.append("JsonTest.cpp expected DEVELOPMENT/USE_IMGUI guards")
    else:
        errors.append("missing JsonTest.cpp")

    if errors:
        print("verify_json_test_release_exclude: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_json_test_release_exclude: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())