#!/usr/bin/env python3
"""Mark the current gap_scan_tick as completed and advance rotation state."""

from __future__ import annotations

import argparse
import json
from datetime import datetime, timezone, timedelta
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DOCS = ROOT / "docs"
INDEX_JSON = DOCS / "implementation_gaps_index.json"
STATE_JSON = ROOT / "gap_scan_state.json"
TICK_JSON = ROOT / "gap_scan_tick.json"

JST = timezone(timedelta(hours=9))


def load_json(path: Path) -> dict:
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def main() -> int:
    parser = argparse.ArgumentParser(description="Record gap scan tick completion")
    parser.add_argument("--new-ids", default="", help="Comma-separated new gap IDs")
    parser.add_argument("--updated-ids", default="", help="Comma-separated updated gap IDs")
    parser.add_argument("--closed-ids", default="", help="Comma-separated closed gap IDs")
    parser.add_argument("--abort", action="store_true", help="Abort tick without advancing rotation")
    args = parser.parse_args()

    if not TICK_JSON.is_file():
        raise SystemExit("gap_scan_tick.json missing; run planGapScanTick.py first")

    tick = load_json(TICK_JSON)
    if tick.get("status") != "in_progress":
        raise SystemExit(f"tick status is not in_progress: {tick.get('status')}")

    index = load_json(INDEX_JSON) if INDEX_JSON.is_file() else {"schemaVersion": 2, "scans": []}
    state = load_json(STATE_JSON)

    new_ids = [x.strip() for x in args.new_ids.split(",") if x.strip()]
    updated_ids = [x.strip() for x in args.updated_ids.split(",") if x.strip()]
    closed_ids = [x.strip() for x in args.closed_ids.split(",") if x.strip()]

    if args.abort:
        tick["status"] = "aborted"
        tick["completedAt"] = datetime.now(JST).isoformat()
        TICK_JSON.write_text(json.dumps(tick, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        state["currentPlannedScanId"] = ""
        STATE_JSON.write_text(json.dumps(state, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        return 0

    scan_record = {
        "scanId": tick["scanId"],
        "at": datetime.now(JST).isoformat(),
        "mode": "incremental",
        "primaryUseCase": tick.get("primaryUseCase"),
        "scenarioId": tick.get("scenarioId"),
        "areaHint": tick.get("areaHint"),
        "priorityScore": tick.get("priorityScore"),
        "newIds": new_ids,
        "updatedIds": updated_ids,
        "closedIds": closed_ids,
    }
    index.setdefault("scans", []).append(scan_record)
    INDEX_JSON.write_text(json.dumps(index, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    scenario_id = tick.get("scenarioId") or ""
    counts = state.setdefault("scenarioScanCounts", {})
    counts[scenario_id] = int(counts.get(scenario_id, 0)) + 1

    rotation = state.get("useCaseRotation") or ["UC-Play", "UC-Authoring", "UC-Debug", "UC-Ship"]
    uc = tick.get("primaryUseCase")
    if uc in rotation:
        state["rotationCursor"] = (rotation.index(uc) + 1) % len(rotation)

    state["lastCompletedScanId"] = tick["scanId"]
    state["currentPlannedScanId"] = ""
    state["totalTicksCompleted"] = int(state.get("totalTicksCompleted", 0)) + 1
    STATE_JSON.write_text(json.dumps(state, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    tick["status"] = "completed"
    tick["completedAt"] = scan_record["at"]
    tick["recorded"] = scan_record
    TICK_JSON.write_text(json.dumps(tick, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(scan_record, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
