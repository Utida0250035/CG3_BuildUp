#!/usr/bin/env python3
"""Preview StageViewCamera poses from stage_view_camera_tuning.json."""

from __future__ import annotations

import argparse
import json
import math
import sys
from pathlib import Path

DEFAULT_JSON = (
    Path(__file__).resolve().parents[2] / "Camera" / "stage_view_camera_tuning.json"
)


def compute_view_pose(
    pivot: tuple[float, float, float],
    view_half_height: float,
    view_half_width: float,
    distance_scale: float,
    eye_height_above_pivot_y: float,
    eye_side_offset_x: float,
    fov_y: float,
    aspect: float,
    elevation_fit_factor: float,
) -> dict[str, tuple[float, float, float]]:
    half_tan = math.tan(fov_y * 0.5)
    distance_for_height = view_half_height / half_tan
    distance_for_width = view_half_width / (half_tan * aspect)
    eye_back = max(distance_for_height, distance_for_width) * distance_scale
    elevation_fit_scale = 1.0 + max(eye_height_above_pivot_y, 0.0) / max(
        view_half_height, 1.0e-3
    ) * elevation_fit_factor
    adjusted_back = eye_back * elevation_fit_scale
    px, py, pz = pivot
    eye = (px + eye_side_offset_x, py + eye_height_above_pivot_y, pz - adjusted_back)
    return {"eye": eye, "pivot": pivot}


def load_tuning(path: Path) -> dict:
    with path.open(encoding="utf-8") as handle:
        return json.load(handle)


def battle_pose(tuning: dict) -> dict:
    b = tuning["stageBounds"]
    bottom_y = b["groundHeight"]
    top_y = b["barrageSpawnTopY"]
    center_x = (b["leftEndX"] + b["rightEndX"]) * 0.5
    center_y = (bottom_y + top_y) * 0.5
    view_height = (top_y - bottom_y) * 0.5
    view_width = (b["rightEndX"] - b["leftEndX"]) * 0.5
    sb = tuning["stageBattle"]
    aspect = tuning["clientWidth"] / tuning["clientHeight"]
    return compute_view_pose(
        (center_x, center_y, 0.0),
        view_height,
        view_width,
        sb["distanceScale"],
        sb["eyeHeightAbovePivotY"],
        sb["eyeSideOffsetX"],
        tuning["perspectiveFovY"],
        aspect,
        sb.get("elevationFitFactor", 0.12),
    )


def focus_pose(
    tuning: dict,
    focus: tuple[float, float, float],
    half_h: float,
    half_w: float,
) -> dict:
    sb = tuning["stageBattle"]
    fc = tuning["focus"]
    aspect = tuning["clientWidth"] / tuning["clientHeight"]
    return compute_view_pose(
        focus,
        half_h,
        half_w,
        fc["distanceScale"],
        sb["eyeHeightAbovePivotY"] * fc["eyeHeightScaleFromBattle"],
        sb["eyeSideOffsetX"] * fc["eyeSideScaleFromBattle"],
        tuning["perspectiveFovY"],
        aspect,
        sb.get("elevationFitFactor", 0.12),
    )


def fmt_vec(v: tuple[float, float, float]) -> str:
    return f"({v[0]:.4f}, {v[1]:.4f}, {v[2]:.4f})"


def main() -> int:
    parser = argparse.ArgumentParser(description="Preview StageViewCamera poses")
    parser.add_argument("--json", type=Path, default=DEFAULT_JSON)
    parser.add_argument("--focus", type=float, nargs=3, metavar=("X", "Y", "Z"))
    parser.add_argument(
        "--focus-half",
        type=float,
        nargs=2,
        default=(1.0, 1.0),
        metavar=("H", "W"),
    )
    args = parser.parse_args()

    if not args.json.is_file():
        print(f"Missing tuning json: {args.json}", file=sys.stderr)
        return 1

    tuning = load_tuning(args.json)
    battle = battle_pose(tuning)
    print("=== stage battle ===")
    print(f"  pivot: {fmt_vec(battle['pivot'])}")
    print(f"  eye:   {fmt_vec(battle['eye'])}")

    if args.focus is not None:
        focus = focus_pose(
            tuning, tuple(args.focus), args.focus_half[0], args.focus_half[1]
        )
        print("=== focus ===")
        print(f"  pivot: {fmt_vec(focus['pivot'])}")
        print(f"  eye:   {fmt_vec(focus['eye'])}")
    else:
        boss = tuning["bossIntroFocusSubject"]
        focus = focus_pose(
            tuning,
            (0.0, 0.0, 0.0),
            boss["halfHeight"],
            boss["halfWidth"],
        )
        print("=== boss intro focus (origin) ===")
        print(f"  pivot: {fmt_vec(focus['pivot'])}")
        print(f"  eye:   {fmt_vec(focus['eye'])}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
