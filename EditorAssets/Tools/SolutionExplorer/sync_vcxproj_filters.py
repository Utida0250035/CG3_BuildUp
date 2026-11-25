#!/usr/bin/env python3
"""方針 A: CG2_BuildUp.vcxproj.filters の未設定 Filter のみパスから同期する。"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from vcxproj_filters_common import (
    apply_policy_a_text,
    append_filter_includes_text,
    append_missing_vcxproj_items_to_filters_text,
    count_items_with_filter,
    count_missing_filter_tags,
    count_self_closing_items,
    default_filters_path,
    default_vcxproj_path,
    include_set_diff,
    missing_filter_include_paths_text,
    parse_filters_xml,
    plan_policy_a,
    read_filters_text,
    read_vcxproj_text,
    repair_undefined_filter_includes_text,
    repo_root_from_here,
    write_filters_text,
)


def main() -> int:
    parser = argparse.ArgumentParser(
        description="vcxproj.filters の Filter 未設定エントリだけをディスクパスに合わせる（方針 A）",
    )
    parser.add_argument(
        "--write",
        action="store_true",
        help="filters ファイルを更新する（省略時は dry-run）",
    )
    parser.add_argument(
        "--filters",
        type=Path,
        default=None,
        help="filters ファイル（既定: Project/CG2_BuildUp.vcxproj.filters）",
    )
    parser.add_argument(
        "--vcxproj",
        type=Path,
        default=None,
        help="vcxproj ファイル（既定: Project/CG2_BuildUp.vcxproj）",
    )
    args = parser.parse_args()

    root = repo_root_from_here(Path(__file__))
    filters_path = args.filters or default_filters_path(root)
    vcxproj_path = args.vcxproj or default_vcxproj_path(root)

    if not filters_path.is_file():
        print(f"sync_vcxproj_filters: ファイルがありません: {filters_path}", file=sys.stderr)
        return 1
    if not vcxproj_path.is_file():
        print(f"sync_vcxproj_filters: ファイルがありません: {vcxproj_path}", file=sys.stderr)
        return 1

    vcxproj_text = read_vcxproj_text(vcxproj_path)
    text = read_filters_text(filters_path)
    missing_in_filters, _extra = include_set_diff(vcxproj_text, text)
    missing_items_count = sum(len(v) for v in missing_in_filters.values())
    xml_root = parse_filters_xml(filters_path).getroot()
    skipped = count_items_with_filter(xml_root)
    before_missing = count_self_closing_items(text)
    planned_added, planned_paths = plan_policy_a(text)

    repair_paths = missing_filter_include_paths_text(text)

    if args.write:
        if missing_items_count:
            text, appended, path_from_items = append_missing_vcxproj_items_to_filters_text(
                text, missing_in_filters
            )
            new_paths_from_items = path_from_items
        else:
            appended = 0
            new_paths_from_items = set()
        text, added, new_paths = apply_policy_a_text(text)
        new_paths = new_paths | new_paths_from_items
        text = append_filter_includes_text(text, new_paths)
        text, repaired = repair_undefined_filter_includes_text(text)
        new_paths = new_paths | repaired
        write_filters_text(filters_path, text)
        after_missing = count_missing_filter_tags(parse_filters_xml(filters_path).getroot())
    else:
        added, new_paths = planned_added, planned_paths | repair_paths
        after_missing = before_missing - added
        appended = missing_items_count
        print("sync_vcxproj_filters: dry-run（--write で更新）")

    print(f"  対象 vcxproj: {vcxproj_path.relative_to(root)}")
    print(f"  対象 filters: {filters_path.relative_to(root)}")
    print(f"  vcxproj にのみ存在（filters へ追記）: {missing_items_count}")
    if missing_items_count and not args.write:
        for tag in ("ClCompile", "ClInclude"):
            items = sorted(missing_in_filters.get(tag, set()))
            if items:
                print(f"    {tag}: {len(items)} 件")
                for inc in items[:8]:
                    print(f"      - {inc}")
                if len(items) > 8:
                    print(f"      ... 他 {len(items) - 8} 件")
    if args.write:
        print(f"  追記した Include 行: {appended}")
    print(f"  既に Filter あり（変更なし）: {skipped}")
    print(f"  追加する <Filter> 子要素: {added}")
    print(f"  新規 Filter Include フォルダ: {len(new_paths)}")
    if new_paths:
        for path in sorted(new_paths):
            print(f"    + {path}")
    print(f"  未設定 Filter 残数（見込み）: {after_missing}")

    if args.write and after_missing > 0:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())