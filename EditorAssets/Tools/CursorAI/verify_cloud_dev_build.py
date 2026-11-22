#!/usr/bin/env python3
"""Cloud Linux 向け Development プリフライト手順が定義されているか検査。"""

from __future__ import annotations

import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
TOOLS = ROOT / "EditorAssets" / "Tools" / "CursorAI"
PREFLIGHT = TOOLS / "runCloudDevelopmentPreflight.sh"
MEMO = TOOLS / "docs" / "feature-memos" / "cloud_dev_build.md"
CLOUD_CHECK = TOOLS / "docs" / "feature-memos" / "cloud_build_vcxproj_check.md"


def main() -> int:
    errors: list[str] = []

    if not PREFLIGHT.is_file():
        errors.append("missing runCloudDevelopmentPreflight.sh")
    else:
        text = PREFLIGHT.read_text(encoding="utf-8", errors="replace")
        for needle in (
            "verify_vcxproj_coverage.py",
            "verify_config_matrix.py",
            "verify_build_development_ci_parity.py",
        ):
            if needle not in text:
                errors.append(f"preflight script must invoke {needle}")

    if not MEMO.is_file():
        errors.append("missing cloud_dev_build.md")
    else:
        body = MEMO.read_text(encoding="utf-8", errors="replace")
        if "runCloudDevelopmentPreflight.sh" not in body:
            errors.append("cloud_dev_build.md must document preflight script")
        if "buildDevelopment.cmd" not in body:
            errors.append("cloud_dev_build.md must mention Windows buildDevelopment.cmd")

    if not CLOUD_CHECK.is_file():
        errors.append("missing cloud_build_vcxproj_check.md")
    elif "runCloudDevelopmentPreflight.sh" not in CLOUD_CHECK.read_text(
        encoding="utf-8", errors="replace"
    ):
        errors.append("cloud_build_vcxproj_check.md must link Cloud Linux preflight")

    if PREFLIGHT.is_file():
        proc = subprocess.run(
            ["bash", str(PREFLIGHT)],
            cwd=ROOT,
            capture_output=True,
            text=True,
        )
        if proc.returncode != 0:
            if proc.stderr.strip():
                errors.append(f"preflight execution failed: {proc.stderr.strip()[:500]}")
            else:
                errors.append(f"preflight execution failed with exit {proc.returncode}")

    if errors:
        print("verify_cloud_dev_build: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_cloud_dev_build: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())