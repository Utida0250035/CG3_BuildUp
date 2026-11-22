#!/usr/bin/env python3
"""Smoke-check movie camera track JSON (mirrors MovieCameraTrackKeyframeParsePolicy)."""

from __future__ import annotations

import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[3]
CAMERA_ASSETS = ROOT / "EditorAssets" / "Camera"

EASE_KINDS = frozenset({"FocusPush", "PullBackReveal", "LegacyInOutQuad"})

REQUIRED_SAMPLE = CAMERA_ASSETS / "sample_movie_track.json"


def parse_vec3(value: Any) -> tuple[float, float, float] | None:
    if not isinstance(value, list) or len(value) != 3:
        return None
    try:
        return (float(value[0]), float(value[1]), float(value[2]))
    except (TypeError, ValueError):
        return None


def try_parse_keyframe(entry: Any) -> dict[str, Any] | None:
    if not isinstance(entry, dict):
        return None
    if "eye" not in entry or "pivot" not in entry:
        return None
    eye = parse_vec3(entry.get("eye"))
    pivot = parse_vec3(entry.get("pivot"))
    if eye is None or pivot is None:
        return None

    time_seconds = 0.0
    if "time" in entry:
        try:
            time_seconds = float(entry["time"])
        except (TypeError, ValueError):
            return None

    ease = "PullBackReveal"
    ease_raw = entry.get("easeToNext")
    if ease_raw is not None:
        if not isinstance(ease_raw, str) or ease_raw not in EASE_KINDS:
            return None
        ease = ease_raw

    return {
        "time": time_seconds,
        "eye": eye,
        "pivot": pivot,
        "easeToNext": ease,
    }


def load_valid_keyframes_from_path(path: Path) -> tuple[list[dict[str, Any]], list[str]]:
    errors: list[str] = []
    try:
        root = json.loads(path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as exc:
        errors.append(f"{path}: cannot read JSON ({exc})")
        return [], errors

    frames = root.get("keyframes")
    if not isinstance(frames, list):
        errors.append(f"{path}: missing or invalid 'keyframes' array")
        return [], errors

    valid: list[dict[str, Any]] = []
    for index, entry in enumerate(frames):
        parsed = try_parse_keyframe(entry)
        if parsed is None:
            continue
        valid.append(parsed)

    return valid, errors


def load_track_ok(path: Path) -> bool:
    valid, errors = load_valid_keyframes_from_path(path)
    if errors:
        return False
    return len(valid) >= 2


def discover_movie_track_json_files() -> list[Path]:
    if not CAMERA_ASSETS.is_dir():
        return []
    found: list[Path] = []
    for path in sorted(CAMERA_ASSETS.glob("*.json")):
        try:
            root = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, json.JSONDecodeError):
            continue
        if isinstance(root.get("keyframes"), list):
            found.append(path)
    return found


def smoke_skip_invalid_keyframe() -> list[str]:
    """1 件不正でも有効 2 件以上なら OK（C++ LoadMovieCameraTrackFromJson と同要件）。"""
    errors: list[str] = []
    payload = {
        "keyframes": [
            {"time": 0.0, "eye": [0, 1, -1], "pivot": [0, 0, 0]},
            {"time": 0.5, "eye": "not-a-vector", "pivot": [0, 0, 0]},
            {"time": 1.0, "eye": [1, 2, -2], "pivot": [0, 0, 0]},
        ]
    }
    valid: list[dict[str, Any]] = []
    for entry in payload["keyframes"]:
        parsed = try_parse_keyframe(entry)
        if parsed is not None:
            valid.append(parsed)
    if len(valid) < 2:
        errors.append("inline smoke: expected >=2 valid keyframes after skip")
    return errors


def main() -> int:
    errors: list[str] = []
    errors.extend(smoke_skip_invalid_keyframe())

    if not REQUIRED_SAMPLE.is_file():
        errors.append(f"missing required sample: {REQUIRED_SAMPLE.relative_to(ROOT)}")
    elif not load_track_ok(REQUIRED_SAMPLE):
        valid, _ = load_valid_keyframes_from_path(REQUIRED_SAMPLE)
        errors.append(
            f"{REQUIRED_SAMPLE.name}: need >=2 valid keyframes (got {len(valid)})"
        )

    for path in discover_movie_track_json_files():
        if path == REQUIRED_SAMPLE:
            continue
        if not load_track_ok(path):
            valid, parse_errors = load_valid_keyframes_from_path(path)
            errors.extend(parse_errors)
            errors.append(
                f"{path.relative_to(ROOT)}: need >=2 valid keyframes (got {len(valid)})"
            )

    if errors:
        print("verify_movie_camera_track_json: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_movie_camera_track_json: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())