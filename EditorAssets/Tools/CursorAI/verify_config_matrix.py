#!/usr/bin/env python3
"""コンパイル定義と Release 除外の期待マトリクスを一括検査。"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
TOOLS = ROOT / "EditorAssets" / "Tools" / "CursorAI"
MEMO = TOOLS / "docs" / "feature-memos" / "cloud_build_vcxproj_check.md"

MATRIX: tuple[tuple[str, Path], ...] = (
    ("macro contract (Debug / Development / Release)", TOOLS / "verify_ship_debug_compile_diff.py"),
    ("editor TU Release ExcludedFromBuild", TOOLS / "verify_editor_sources_release_exclude.py"),
    ("ForDebug TU Release ExcludedFromBuild", TOOLS / "verify_for_debug_sources_release_exclude.py"),
    ("Release link GenerateDebugInformation=false", TOOLS / "verify_release_link_generate_debug_info.py"),
    ("JsonTest debug panel TU Release exclude", TOOLS / "verify_json_test_release_exclude.py"),
)


def run_check(label: str, script: Path) -> list[str]:
    if not script.is_file():
        return [f"{label}: missing {script.name}"]
    proc = subprocess.run(
        [sys.executable, str(script)],
        cwd=ROOT,
        capture_output=True,
        text=True,
    )
    if proc.returncode == 0:
        return []
    errors = []
    if proc.stdout.strip():
        errors.append(f"{label}: {proc.stdout.strip()}")
    if proc.stderr.strip():
        for line in proc.stderr.strip().splitlines():
            errors.append(f"{label}: {line}")
    if not errors:
        errors.append(f"{label}: failed with exit {proc.returncode}")
    return errors


def main() -> int:
    errors: list[str] = []

    if not MEMO.is_file():
        errors.append("missing cloud_build_vcxproj_check.md")
    else:
        memo = MEMO.read_text(encoding="utf-8", errors="replace")
        if "verify_config_matrix.py" not in memo:
            errors.append("cloud_build_vcxproj_check.md must document verify_config_matrix.py")
        if "verify_ship_debug_compile_diff" not in memo:
            errors.append("cloud_build_vcxproj_check.md must list macro contract check")

    for label, script in MATRIX:
        errors.extend(run_check(label, script))

    if errors:
        print("verify_config_matrix: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_config_matrix: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())