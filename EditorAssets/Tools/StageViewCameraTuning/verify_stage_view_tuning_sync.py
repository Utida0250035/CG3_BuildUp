#!/usr/bin/env python3
"""Verify stage_view_camera_tuning.json matches StageViewCamera C++ constants."""

from __future__ import annotations

import sys
from pathlib import Path

from stage_view_tuning_contract import (
    compare_contracts,
    contract_from_json,
    default_paths,
    load_contract_from_cpp,
    load_json,
    repo_root_from_here,
)


def main() -> int:
    root = repo_root_from_here(Path(__file__))
    paths = default_paths(root)

    for key, path in paths.items():
        if not path.is_file():
            print(f"verify_stage_view_tuning_sync: missing {key}: {path}", file=sys.stderr)
            return 1

    try:
        cpp_contract = load_contract_from_cpp(paths)
        json_contract = contract_from_json(load_json(paths["json"]))
    except (ValueError, KeyError, TypeError) as exc:
        print(f"verify_stage_view_tuning_sync: parse error: {exc}", file=sys.stderr)
        return 1

    errors = compare_contracts(cpp_contract, json_contract)
    if errors:
        for line in errors:
            print(line, file=sys.stderr)
        print(
            "verify_stage_view_tuning_sync: drift detected "
            "(run sync_stage_view_tuning_json.py --write)",
            file=sys.stderr,
        )
        return 1

    print("verify_stage_view_tuning_sync: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())