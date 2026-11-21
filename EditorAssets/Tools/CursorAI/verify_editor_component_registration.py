#!/usr/bin/env python3
"""EntityEditor のコンポーネント登録三箇所（Definition / Ui プリセット / Runtime 追加）の同期を検査する。"""

from __future__ import annotations

import re
import sys
from pathlib import Path


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def extract_definition_known(text: str) -> set[str]:
    match = re.search(
        r"bool IsKnownEditorRuntimeComponentTypeName\(const std::string &typeName\) \{([^}]+)\}",
        text,
        re.DOTALL,
    )
    if not match:
        return set()
    body = match.group(1)
    return set(re.findall(r'typeName == "(Cmp[^"]+)"', body))


def slice_between(text: str, start_marker: str, end_marker: str) -> str:
    start = text.find(start_marker)
    if start < 0:
        return ""
    end = text.find(end_marker, start + len(start_marker))
    if end < 0:
        return ""
    return text[start:end]


def extract_ui_presets(text: str) -> set[str]:
    block = slice_between(
        text,
        "constexpr KnownComponentEntry kKnownComponents[] = {",
        "};",
    )
    if not block:
        return set()
    return set(re.findall(r'\{"(Cmp[^"]+)"', block))


def extract_runtime_addable(text: str) -> set[str]:
    block = slice_between(
        text,
        "bool TryAddEmptyComponent(Entity &entity, const ComponentSlot &slot)",
        "void ApplySlotExecutionOrder",
    )
    if not block:
        return set()
    return set(re.findall(r'slot\.typeName == "(Cmp[^"]+)"', block))


def extract_default_params_types(text: str) -> set[str]:
    match = re.search(
        r"nlohmann::json DefaultParamsForType\(const std::string &typeName\) \{([\s\S]*?)^\}",
        text,
        re.MULTILINE,
    )
    if not match:
        return set()
    return set(re.findall(r'if \(typeName == "(Cmp[^"]+)"\)', match.group(1)))


def extract_draw_param_types(text: str) -> set[str]:
    match = re.search(
        r"bool DrawComponentParams\(ComponentSlot &slot, const UiState &ui\) \{([\s\S]*?)^\}",
        text,
        re.MULTILINE,
    )
    if not match:
        return set()
    return set(re.findall(r'if \(slot\.typeName == "(Cmp[^"]+)"\)', match.group(1)))


def main() -> int:
    root = repo_root()
    def_path = root / "Project/Codes/Source/System/Editor/Entity/EntityEditorDefinition.cpp"
    ui_path = root / "Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp"
    runtime_path = root / "Project/Codes/Source/System/Editor/Scene/EditorSceneRuntime.cpp"

    for path in (def_path, ui_path, runtime_path):
        if not path.is_file():
            print(f"verify_editor_component_registration: missing {path}", file=sys.stderr)
            return 1

    def_text = read_text(def_path)
    ui_text = read_text(ui_path)
    runtime_text = read_text(runtime_path)

    definition = extract_definition_known(def_text)
    ui_presets = extract_ui_presets(ui_text)
    runtime_add = extract_runtime_addable(runtime_text)
    default_params = extract_default_params_types(def_text)
    draw_params = extract_draw_param_types(ui_text)

    errors: list[str] = []
    warnings: list[str] = []

    if not definition:
        errors.append("IsKnownEditorRuntimeComponentTypeName: no Cmp* types parsed")
    if not ui_presets:
        errors.append("kKnownComponents: no Cmp* presets parsed")
    if not runtime_add:
        errors.append("TryAddEmptyComponent: no Cmp* types parsed")

    def_ui = definition.symmetric_difference(ui_presets)
    if def_ui:
        errors.append(
            "Definition vs Ui preset mismatch: "
            + ", ".join(sorted(def_ui))
        )

    def_runtime = definition.symmetric_difference(runtime_add)
    if def_runtime:
        errors.append(
            "Definition vs Runtime add mismatch: "
            + ", ".join(sorted(def_runtime))
        )

    ui_runtime = ui_presets.symmetric_difference(runtime_add)
    if ui_runtime:
        errors.append(
            "Ui preset vs Runtime add mismatch: "
            + ", ".join(sorted(ui_runtime))
        )

    for cmp_name in sorted(ui_presets):
        if cmp_name not in default_params:
            warnings.append(f"DefaultParamsForType missing: {cmp_name}")
        if cmp_name not in draw_params:
            warnings.append(f"DrawComponentParams missing: {cmp_name}")

    print(
        f"editor component registration: {len(definition)} definition, "
        f"{len(ui_presets)} ui presets, {len(runtime_add)} runtime add"
    )
    for line in warnings:
        print(f"warning: {line}")
    for line in errors:
        print(line, file=sys.stderr)

    if errors:
        return 1

    print("verify_editor_component_registration: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())