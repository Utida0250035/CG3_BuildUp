#!/usr/bin/env python3
"""Scaffold a debug test play area scene JSON from template."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from personalize_play_area_scene import personalize_play_area_scene, sanitize_file_stem

ROOT = Path(__file__).resolve().parents[2]
TEMPLATE = ROOT / "DebugPlayArea" / "template_test_play_area.scene.json"
OUT_DIR = ROOT / "DebugPlayArea"


def main() -> int:
    parser = argparse.ArgumentParser(description="Create debug play area scene")
    parser.add_argument("name", help="Scene name (e.g. MyCombatTest)")
    parser.add_argument(
        "--out",
        type=Path,
        help="Output path (default: EditorAssets/DebugPlayArea/<name>.scene.json)",
    )
    args = parser.parse_args()

    if not TEMPLATE.is_file():
        print(f"Missing template: {TEMPLATE}", file=sys.stderr)
        return 1

    safe_name = sanitize_file_stem(args.name)
    out_path = args.out or (OUT_DIR / f"{safe_name}.scene.json")

    with TEMPLATE.open(encoding="utf-8") as handle:
        data = json.load(handle)

    personalize_play_area_scene(data, args.name)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    with out_path.open("w", encoding="utf-8") as handle:
        json.dump(data, handle, indent=2, ensure_ascii=False)
        handle.write("\n")

    print(f"Wrote {out_path}")
    print(
        "ランタイムプレビュー: プロジェクト一覧で JSON を選択 →「シーンを実行」(BuildUp)。"
        "生成直後は「作成して BuildUp」。エディタ内のみなら「作成してエディタ内プレビュー」。"
        "F2=FeatureTest 正本、F6=combat_bounds_sandbox。"
        "起動シーン化は「起動シーンに設定」。"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
