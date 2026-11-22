"""Shared mappings between C++ StageViewCamera sources and stage_view_camera_tuning.json."""

from __future__ import annotations

import json
import re
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class TuningContract:
    perspective_fov_y: float
    client_width: int
    client_height: int
    battle_distance_scale: float
    battle_eye_height_above_pivot_y: float
    battle_eye_side_offset_x: float
    battle_elevation_fit_factor: float
    focus_distance_scale: float
    focus_eye_height_scale: float
    focus_eye_side_scale: float
    boss_intro_half_height: float
    boss_intro_half_width: float
    boss_defeat_half_height: float
    boss_defeat_half_width: float
    boss_defeat_push_seconds: float
    ground_height: float
    barrage_spawn_top_y: float
    left_end_x: float
    right_end_x: float


def repo_root_from_here(here: Path) -> Path:
    return here.resolve().parents[3]


def default_paths(root: Path) -> dict[str, Path]:
    return {
        "json": root / "EditorAssets/Camera/stage_view_camera_tuning.json",
        "stage_header": root / "Project/Codes/Include/Camera/StageViewCamera.h",
        "stage_cpp": root / "Project/Codes/Source/Camera/StageViewCamera.cpp",
        "stage_bounds": root / "Project/Codes/Include/System/StageBounds.h",
        "window_header": root / "Project/Codes/Include/Engine/Platform/Window/Window.h",
    }


def _parse_float(name: str, text: str) -> float:
    pattern = rf"{re.escape(name)}\s*=\s*([-\d.]+)f?"
    match = re.search(pattern, text)
    if not match:
        raise ValueError(f"Could not parse {name}")
    return float(match.group(1))


def _parse_int(name: str, text: str) -> int:
    pattern = rf"{re.escape(name)}\s*=\s*(\d+)"
    match = re.search(pattern, text)
    if not match:
        raise ValueError(f"Could not parse {name}")
    return int(match.group(1))


def _parse_focus_subject(text: str, symbol: str) -> tuple[float, float]:
    pattern = rf"{symbol}\s*\{{\s*([-\d.]+)f?\s*,\s*([-\d.]+)f?\s*\}}"
    match = re.search(pattern, text)
    if not match:
        raise ValueError(f"Could not parse {symbol}")
    return float(match.group(1)), float(match.group(2))


def load_contract_from_cpp(paths: dict[str, Path]) -> TuningContract:
    stage_header = paths["stage_header"].read_text(encoding="utf-8")
    stage_cpp = paths["stage_cpp"].read_text(encoding="utf-8")
    bounds_text = paths["stage_bounds"].read_text(encoding="utf-8")
    window_text = paths["window_header"].read_text(encoding="utf-8")

    focus_match = re.search(
        r"ComputeViewPose\(focusWorld, subjectHalfHeight, subjectHalfWidth,\s*"
        r"([-\d.]+)f?,\s*"
        r"kStageBattleViewEyeHeightAbovePivotY\s*\*\s*([-\d.]+)f?,\s*"
        r"kStageBattleViewEyeSideOffsetX\s*\*\s*([-\d.]+)f?",
        stage_cpp,
    )
    if not focus_match:
        raise ValueError("Could not parse focus scales in StageViewCamera.cpp")

    elevation_match = re.search(
        r"elevationFitFactor\s*\*\s*0\.12f|"
        r"\*\s*([-\d.]+)f?\s*;\s*$",
        stage_cpp,
        re.MULTILINE,
    )
    elevation_fit = 0.12
    elev_line = re.search(
        r"std::max\(eyeHeightAbovePivotY, 0\.0f\) /\s*"
        r"std::max\(viewHalfHeight, 1\.0e-3f\) \* ([-\d.]+)f",
        stage_cpp,
    )
    if elev_line:
        elevation_fit = float(elev_line.group(1))

    intro_h, intro_w = _parse_focus_subject(stage_header, "kBossIntroFocusSubject")
    defeat_h, defeat_w = _parse_focus_subject(stage_header, "kBossDefeatFocusSubject")

    return TuningContract(
        perspective_fov_y=_parse_float("kPerspectiveFovY", stage_cpp),
        client_width=_parse_int("kDefaultClientWidth", window_text),
        client_height=_parse_int("kDefaultClientHeight", window_text),
        battle_distance_scale=_parse_float(
            "kStageBattleViewDistanceScale", stage_header
        ),
        battle_eye_height_above_pivot_y=_parse_float(
            "kStageBattleViewEyeHeightAbovePivotY", stage_header
        ),
        battle_eye_side_offset_x=_parse_float(
            "kStageBattleViewEyeSideOffsetX", stage_header
        ),
        battle_elevation_fit_factor=elevation_fit,
        focus_distance_scale=float(focus_match.group(1)),
        focus_eye_height_scale=float(focus_match.group(2)),
        focus_eye_side_scale=float(focus_match.group(3)),
        boss_intro_half_height=intro_h,
        boss_intro_half_width=intro_w,
        boss_defeat_half_height=defeat_h,
        boss_defeat_half_width=defeat_w,
        boss_defeat_push_seconds=_parse_float(
            "kBossDefeatCameraPushSeconds", stage_header
        ),
        ground_height=_parse_float("kGroundHeight", bounds_text),
        barrage_spawn_top_y=_parse_float("kCeilHeight", bounds_text) + 1.25,
        left_end_x=_parse_float("kLeftEndX", bounds_text),
        right_end_x=_parse_float("kRightEndX", bounds_text),
    )


def contract_from_json(data: dict) -> TuningContract:
    battle = data["stageBattle"]
    focus = data["focus"]
    intro = data["bossIntroFocusSubject"]
    defeat = data["bossDefeatFocusSubject"]
    bounds = data["stageBounds"]
    return TuningContract(
        perspective_fov_y=float(data["perspectiveFovY"]),
        client_width=int(data["clientWidth"]),
        client_height=int(data["clientHeight"]),
        battle_distance_scale=float(battle["distanceScale"]),
        battle_eye_height_above_pivot_y=float(battle["eyeHeightAbovePivotY"]),
        battle_eye_side_offset_x=float(battle["eyeSideOffsetX"]),
        battle_elevation_fit_factor=float(battle.get("elevationFitFactor", 0.12)),
        focus_distance_scale=float(focus["distanceScale"]),
        focus_eye_height_scale=float(focus["eyeHeightScaleFromBattle"]),
        focus_eye_side_scale=float(focus["eyeSideScaleFromBattle"]),
        boss_intro_half_height=float(intro["halfHeight"]),
        boss_intro_half_width=float(intro["halfWidth"]),
        boss_defeat_half_height=float(defeat["halfHeight"]),
        boss_defeat_half_width=float(defeat["halfWidth"]),
        boss_defeat_push_seconds=float(data["bossDefeatCameraPushSeconds"]),
        ground_height=float(bounds["groundHeight"]),
        barrage_spawn_top_y=float(bounds["barrageSpawnTopY"]),
        left_end_x=float(bounds["leftEndX"]),
        right_end_x=float(bounds["rightEndX"]),
    )


def apply_contract_to_json(data: dict, contract: TuningContract) -> None:
    data["perspectiveFovY"] = contract.perspective_fov_y
    data["clientWidth"] = contract.client_width
    data["clientHeight"] = contract.client_height
    data["stageBattle"]["distanceScale"] = contract.battle_distance_scale
    data["stageBattle"]["eyeHeightAbovePivotY"] = (
        contract.battle_eye_height_above_pivot_y
    )
    data["stageBattle"]["eyeSideOffsetX"] = contract.battle_eye_side_offset_x
    data["stageBattle"]["elevationFitFactor"] = contract.battle_elevation_fit_factor
    data["focus"]["distanceScale"] = contract.focus_distance_scale
    data["focus"]["eyeHeightScaleFromBattle"] = contract.focus_eye_height_scale
    data["focus"]["eyeSideScaleFromBattle"] = contract.focus_eye_side_scale
    data["bossIntroFocusSubject"]["halfHeight"] = contract.boss_intro_half_height
    data["bossIntroFocusSubject"]["halfWidth"] = contract.boss_intro_half_width
    data["bossDefeatFocusSubject"]["halfHeight"] = contract.boss_defeat_half_height
    data["bossDefeatFocusSubject"]["halfWidth"] = contract.boss_defeat_half_width
    data["bossDefeatCameraPushSeconds"] = contract.boss_defeat_push_seconds
    data["stageBounds"]["groundHeight"] = contract.ground_height
    data["stageBounds"]["barrageSpawnTopY"] = contract.barrage_spawn_top_y
    data["stageBounds"]["leftEndX"] = contract.left_end_x
    data["stageBounds"]["rightEndX"] = contract.right_end_x


def load_json(path: Path) -> dict:
    with path.open(encoding="utf-8") as handle:
        return json.load(handle)


def write_json(path: Path, data: dict) -> None:
    path.write_text(
        json.dumps(data, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )


def compare_contracts(
    cpp: TuningContract, js: TuningContract, epsilon: float = 1.0e-4
) -> list[str]:
    errors: list[str] = []
    for field in cpp.__dataclass_fields__:
        left = getattr(cpp, field)
        right = getattr(js, field)
        if isinstance(left, float):
            if abs(left - float(right)) > epsilon:
                errors.append(f"{field}: cpp={left} json={right}")
        elif left != right:
            errors.append(f"{field}: cpp={left} json={right}")
    return errors