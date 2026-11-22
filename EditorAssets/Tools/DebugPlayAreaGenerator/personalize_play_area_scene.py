#!/usr/bin/env python3
"""テンプレ JSON をシーン名に合わせて instance／tag まで差し替える。"""

from __future__ import annotations

from typing import Any


def sanitize_file_stem(name: str) -> str:
    stem = []
    for ch in name:
        if ch.isalnum() or ch in "-_":
            stem.append(ch)
        else:
            stem.append("_")
    text = "".join(stem)
    return text if text else "DebugPlayArea"


def personalize_play_area_scene(data: dict[str, Any], scene_name: str) -> None:
    data["sceneName"] = scene_name
    safe = sanitize_file_stem(scene_name)

    catalog = data.get("tagCatalog")
    if isinstance(catalog, list) and scene_name not in catalog:
        catalog.append(scene_name)

    entities = data.get("entities")
    if not isinstance(entities, list):
        return

    renames = {
        "DebugProp": f"{safe}_Prop",
        "DebugFloorMarker": f"{safe}_Floor",
    }

    for entity in entities:
        if not isinstance(entity, dict):
            continue
        inst = entity.get("instanceName")
        if inst not in renames:
            continue
        new_name = renames[inst]
        entity["instanceName"] = new_name
        definition = entity.setdefault("definition", {})
        if isinstance(definition, dict):
            definition["entityName"] = new_name