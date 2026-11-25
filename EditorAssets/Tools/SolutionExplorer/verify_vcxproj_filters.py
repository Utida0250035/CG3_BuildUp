#!/usr/bin/env python3
"""CG2_BuildUp.vcxproj.filters の Filter 割り当てと vcxproj との Include 一致を検証する。"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from vcxproj_filters_common import (
    child_filter,
    collect_existing_filter_includes,
    default_filters_path,
    default_vcxproj_path,
    filter_path_from_include,
    include_set_diff,
    iter_filterable_items,
    local_tag,
    parse_filters_xml,
    read_filters_text,
    read_vcxproj_text,
    repo_root_from_here,
)


def main() -> int:
    parser = argparse.ArgumentParser(description="vcxproj.filters の Filter 整合を検証")
    parser.add_argument("--filters", type=Path, default=None)
    parser.add_argument("--vcxproj", type=Path, default=None)
    args = parser.parse_args()

    root = repo_root_from_here(Path(__file__))
    filters_path = args.filters or default_filters_path(root)
    vcxproj_path = args.vcxproj or default_vcxproj_path(root)

    vcxproj_text = read_vcxproj_text(vcxproj_path)
    filters_text = read_filters_text(filters_path)
    missing_in_filters, extra_in_filters = include_set_diff(vcxproj_text, filters_text)

    tree = parse_filters_xml(filters_path)
    xml_root = tree.getroot()

    known = collect_existing_filter_includes(xml_root)
    missing_child: list[str] = []
    orphan_filter_ref: list[str] = []
    mismatch: list[str] = []

    for elem in iter_filterable_items(xml_root):
        include = elem.attrib.get("Include", "")
        tag = local_tag(elem.tag)
        filter_ = child_filter(elem)
        if filter_ is None or not (filter_.text or "").strip():
            missing_child.append(f"{tag}: {include}")
            continue
        path = filter_.text.strip()
        if path not in known:
            orphan_filter_ref.append(f"{tag}: {include} -> {path}")
        expected = filter_path_from_include(include)
        if path != expected:
            mismatch.append(f"{tag}: {include}\n    現在={path}\n    導出={expected}")

    parity_ok = not any(missing_in_filters.values()) and not any(
        extra_in_filters.values()
    )
    filters_ok = not missing_child and not orphan_filter_ref
    ok = parity_ok and filters_ok

    print(f"verify_vcxproj_filters: {filters_path.relative_to(root)}")
    print(f"  vcxproj: {vcxproj_path.relative_to(root)}")
    print(f"  Filter Include 定義数: {len(known)}")
    print(f"  <Filter> 未設定: {len(missing_child)}")
    print(f"  未定義 Filter 参照: {len(orphan_filter_ref)}")
    print(f"  パス不一致（方針 A では既存は許容）: {len(mismatch)}")
    print(
        "  Include 一致: "
        f"vcxproj のみ {sum(len(v) for v in missing_in_filters.values())} / "
        f"filters のみ {sum(len(v) for v in extra_in_filters.values())}"
    )

    for label, items in (
        ("<Filter> 未設定", missing_child[:20]),
        ("未定義 Filter 参照", orphan_filter_ref[:20]),
    ):
        if items:
            print(f"  --- {label}（先頭 {len(items)} 件）---")
            for line in items:
                print(f"    {line}")

    for tag, title in (
        ("ClCompile", "vcxproj にのみ（filters 欠落）"),
        ("ClInclude", "vcxproj にのみ（filters 欠落）"),
    ):
        items = sorted(missing_in_filters.get(tag, set()))
        if items:
            print(f"  --- {title}: {len(items)} 件（先頭 {min(20, len(items))}）---")
            for inc in items[:20]:
                print(f"    {inc}")

    for tag, title in (
        ("ClCompile", "filters にのみ（vcxproj 欠落）"),
        ("ClInclude", "filters にのみ（vcxproj 欠落）"),
    ):
        items = sorted(extra_in_filters.get(tag, set()))
        if items:
            print(f"  --- {title}: {len(items)} 件（先頭 {min(20, len(items))}）---")
            for inc in items[:20]:
                print(f"    {inc}")

    if not ok:
        return 1
    print("verify_vcxproj_filters: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
