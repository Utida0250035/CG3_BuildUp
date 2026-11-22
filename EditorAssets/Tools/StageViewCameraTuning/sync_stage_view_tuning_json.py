#!/usr/bin/env python3
"""Sync stage_view_camera_tuning.json numeric fields from C++ StageViewCamera sources."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from stage_view_tuning_contract import (
    apply_contract_to_json,
    default_paths,
    load_contract_from_cpp,
    load_json,
    repo_root_from_here,
    write_json,
)


def main() -> int:
    parser = argparse.ArgumentParser(description="Sync tuning JSON from C++ constants")
    parser.add_argument(
        "--write",
        action="store_true",
        help="Write JSON when C++ is the source of truth (default: dry-run)",
    )
    args = parser.parse_args()

    root = repo_root_from_here(Path(__file__))
    paths = default_paths(root)

    try:
        contract = load_contract_from_cpp(paths)
        data = load_json(paths["json"])
    except (ValueError, OSError, KeyError) as exc:
        print(f"sync_stage_view_tuning_json: {exc}", file=sys.stderr)
        return 1

    apply_contract_to_json(data, contract)

    if args.write:
        write_json(paths["json"], data)
        print(f"sync_stage_view_tuning_json: wrote {paths['json']}")
    else:
        print("sync_stage_view_tuning_json: dry-run (pass --write to update JSON)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())