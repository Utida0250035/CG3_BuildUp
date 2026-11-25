"""CG2_BuildUp.vcxproj.filters 向けの共通処理（方針 A: 未設定の Filter のみ補う）。"""

from __future__ import annotations

import re
import uuid
import xml.etree.ElementTree as ET
from pathlib import Path

ITEM_TAGS = frozenset({"ClCompile", "ClInclude"})

SELF_CLOSING_ITEM = re.compile(
    r'^(\s*)<(ClCompile|ClInclude) Include="([^"]+)"\s*/>\s*$',
    re.MULTILINE,
)
FILTER_INCLUDE_PATTERN = re.compile(r'<Filter Include="([^"]+)"')
FILTER_CHILD_PATTERN = re.compile(r"<Filter>([^<]+)</Filter>")
INCLUDE_ATTR_PATTERN = re.compile(r'<(ClCompile|ClInclude)\s+Include="([^"]+)"')
COMPILE_GROUP_END = "  </ItemGroup>\n  <ItemGroup>\n    <ClInclude Include="
INCLUDE_GROUP_END = "  </ItemGroup>\n  <ItemGroup>\n    <Filter Include="


def repo_root_from_here(here: Path) -> Path:
    p = here.resolve()
    for _ in range(12):
        if (p / "Project" / "CG2_BuildUp.vcxproj.filters").is_file():
            return p
        if p.parent == p:
            break
        p = p.parent
    raise ValueError("Project/CG2_BuildUp.vcxproj.filters が見つかりません")


def default_filters_path(root: Path) -> Path:
    return root / "Project" / "CG2_BuildUp.vcxproj.filters"


def default_vcxproj_path(root: Path) -> Path:
    return root / "Project" / "CG2_BuildUp.vcxproj"


def local_tag(tag: str) -> str:
    if tag.startswith("{"):
        return tag.split("}", 1)[1]
    return tag


def filter_path_from_include(include: str) -> str:
    """Include パスの親ディレクトリを Filter 名にする（区切りは \\）。"""
    normalized = include.replace("/", "\\")
    parts = normalized.split("\\")
    if len(parts) <= 1:
        raise ValueError(f"Filter を導出できない Include: {include!r}")
    return "\\".join(parts[:-1])


def new_filter_guid() -> str:
    return "{" + str(uuid.uuid4()) + "}"


def read_filters_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def read_vcxproj_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def collect_includes_by_tag(text: str) -> dict[str, set[str]]:
    buckets: dict[str, set[str]] = {tag: set() for tag in ITEM_TAGS}
    for tag, include in INCLUDE_ATTR_PATTERN.findall(text):
        if tag in buckets:
            buckets[tag].add(include)
    return buckets


def include_set_diff(
    vcxproj_text: str,
    filters_text: str,
) -> tuple[dict[str, set[str]], dict[str, set[str]]]:
    """戻り値: (filters に無い, filters にだけある) をタグ別に返す。"""
    vp = collect_includes_by_tag(vcxproj_text)
    ft = collect_includes_by_tag(filters_text)
    missing_in_filters = {tag: vp[tag] - ft[tag] for tag in ITEM_TAGS}
    extra_in_filters = {tag: ft[tag] - vp[tag] for tag in ITEM_TAGS}
    return missing_in_filters, extra_in_filters


def format_item_with_filter(tag: str, include: str, indent: str = "    ") -> str:
    filter_path = filter_path_from_include(include)
    return (
        f'{indent}<{tag} Include="{include}">\n'
        f"{indent}  <Filter>{filter_path}</Filter>\n"
        f"{indent}</{tag}>"
    )


def append_missing_vcxproj_items_to_filters_text(
    text: str,
    missing: dict[str, set[str]],
) -> tuple[str, int, set[str]]:
    """vcxproj にあって filters に無い Include 行を追記する。"""
    added = 0
    new_filter_paths: set[str] = set()
    existing_filters = collect_existing_filter_includes_text(text)

    compile_list = sorted(missing.get("ClCompile", set()))
    if compile_list:
        block = "\n".join(format_item_with_filter("ClCompile", inc) for inc in compile_list) + "\n"
        idx = text.find(COMPILE_GROUP_END)
        if idx < 0:
            raise ValueError("ClCompile ItemGroup の終端が見つかりません")
        text = text[:idx] + block + text[idx:]
        added += len(compile_list)
        for inc in compile_list:
            path = filter_path_from_include(inc)
            if path not in existing_filters:
                new_filter_paths.add(path)

    include_list = sorted(missing.get("ClInclude", set()))
    if include_list:
        block = "\n".join(format_item_with_filter("ClInclude", inc) for inc in include_list) + "\n"
        idx = text.find(INCLUDE_GROUP_END)
        if idx < 0:
            raise ValueError("ClInclude ItemGroup の終端が見つかりません")
        text = text[:idx] + block + text[idx:]
        added += len(include_list)
        for inc in include_list:
            path = filter_path_from_include(inc)
            if path not in existing_filters:
                new_filter_paths.add(path)

    return text, added, new_filter_paths


def collect_existing_filter_includes_text(text: str) -> set[str]:
    return set(FILTER_INCLUDE_PATTERN.findall(text))


def collect_referenced_filter_paths_text(text: str) -> set[str]:
    return {p.strip() for p in FILTER_CHILD_PATTERN.findall(text) if p.strip()}


def missing_filter_include_paths_text(text: str) -> set[str]:
    return collect_referenced_filter_paths_text(text) - collect_existing_filter_includes_text(text)


def repair_undefined_filter_includes_text(text: str) -> tuple[str, set[str]]:
    """子 <Filter> が参照するが Filter Include が無いパスを末尾に追加する。"""
    missing = missing_filter_include_paths_text(text)
    if not missing:
        return text, set()
    return append_filter_includes_text(text, missing), missing


def count_self_closing_items(text: str) -> int:
    return len(SELF_CLOSING_ITEM.findall(text))


def plan_policy_a(text: str) -> tuple[int, set[str]]:
    """自己完結タグに付与する Filter と、新規 Filter Include の一覧を返す。"""
    existing = collect_existing_filter_includes_text(text)
    new_paths: set[str] = set()
    for _indent, _tag, include in SELF_CLOSING_ITEM.findall(text):
        path = filter_path_from_include(include)
        if path not in existing:
            new_paths.add(path)
    added = count_self_closing_items(text)
    return added, new_paths


def apply_policy_a_text(text: str) -> tuple[str, int, set[str]]:
    existing = collect_existing_filter_includes_text(text)
    new_paths: set[str] = set()
    added = 0

    def repl(match: re.Match[str]) -> str:
        nonlocal added
        indent, tag, include = match.group(1), match.group(2), match.group(3)
        filter_path = filter_path_from_include(include)
        added += 1
        if filter_path not in existing:
            new_paths.add(filter_path)
        return (
            f"{indent}<{tag} Include=\"{include}\">\n"
            f"{indent}  <Filter>{filter_path}</Filter>\n"
            f"{indent}</{tag}>"
        )

    return SELF_CLOSING_ITEM.sub(repl, text), added, new_paths


def append_filter_includes_text(text: str, new_paths: set[str]) -> str:
    if not new_paths:
        return text
    block_lines: list[str] = []
    for path in sorted(new_paths):
        guid = new_filter_guid()
        block_lines.append(f'    <Filter Include="{path}">')
        block_lines.append(f"      <UniqueIdentifier>{guid}</UniqueIdentifier>")
        block_lines.append("    </Filter>")
    block = "\n".join(block_lines) + "\n"
    marker = "  </ItemGroup>\n</Project>"
    idx = text.rfind(marker)
    if idx < 0:
        raise ValueError("Filter ItemGroup の終端が見つかりません")
    return text[:idx] + block + text[idx:]


def write_filters_text(path: Path, text: str) -> None:
    path.write_text(text, encoding="utf-8", newline="\n")


def parse_filters_xml(path: Path) -> ET.ElementTree:
    try:
        return ET.parse(path)
    except ET.ParseError as exc:
        raise ValueError(f"XML 解析失敗: {path}: {exc}") from exc


def iter_filterable_items(root: ET.Element) -> list[ET.Element]:
    out: list[ET.Element] = []
    for elem in root.iter():
        if local_tag(elem.tag) in ITEM_TAGS:
            out.append(elem)
    return out


def child_filter(elem: ET.Element) -> ET.Element | None:
    for child in elem:
        if local_tag(child.tag) == "Filter":
            return child
    return None


def collect_existing_filter_includes(root: ET.Element) -> set[str]:
    found: set[str] = set()
    for elem in root.iter():
        if local_tag(elem.tag) != "Filter":
            continue
        name = elem.attrib.get("Include")
        if name:
            found.add(name)
    return found


def count_missing_filter_tags(root: ET.Element) -> int:
    n = 0
    for elem in iter_filterable_items(root):
        if child_filter(elem) is None and elem.attrib.get("Include"):
            n += 1
    return n


def count_items_with_filter(root: ET.Element) -> int:
    n = 0
    for elem in iter_filterable_items(root):
        if child_filter(elem) is not None:
            n += 1
    return n