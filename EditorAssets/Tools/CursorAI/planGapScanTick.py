#!/usr/bin/env python3
"""Select exactly one gap-scan scope (1 tick) by priority. Writes gap_scan_tick.json."""

from __future__ import annotations

import json
import re
import sys
from dataclasses import dataclass
from datetime import datetime, timezone, timedelta
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DOCS = ROOT / "docs"
SCENARIOS_MD = DOCS / "use_case_scenarios.md"
INDEX_JSON = DOCS / "implementation_gaps_index.json"
STATE_JSON = ROOT / "gap_scan_state.json"
TICK_JSON = ROOT / "gap_scan_tick.json"

JST = timezone(timedelta(hours=9))

USE_CASE_WEIGHT = {
    "UC-Play": 1000,
    "UC-Authoring": 950,
    "UC-Ship": 900,
    "UC-Debug": 850,
}

USE_CASE_LABEL = {
    "UC-Play": "Play",
    "UC-Authoring": "Authoring",
    "UC-Debug": "Debug",
    "UC-Ship": "Ship",
}

# scenarioId prefix / spec path -> suggested code search scope (no Codes edits from this script)
PREFIX_AREA: list[tuple[str, str]] = [
    ("DBG-Streaming", "Streaming"),
    ("DBG-Render", "Render"),
    ("DBG-FeatureScene", "Scene"),
    ("DBG-PlayArea", "Editor"),
    ("PLY-Audio", "Streaming"),
    ("PLY-Scene", "Scene"),
    ("PLY-Camera", "UX"),
    ("PLY-Visual", "Render"),
    ("PLY-Movie", "Runtime"),
    ("AUT-Editor", "Editor"),
    ("AUT-Guidance", "UX"),
    ("AUT-Component", "Editor"),
    ("AUT-Visual", "Render"),
    ("AUT-Asset", "Editor"),
    ("SHP-DevBuild", "Infra"),
    ("SHP-Release", "Infra"),
    ("SHP-Asset", "Resource"),
    ("SHP-Config", "Infra"),
    ("SHP-Smoke", "Scene"),
    ("DBG-Profiler", "Runtime"),
]

SPEC_HINTS: dict[str, tuple[str, list[str]]] = {
    "feature-memos/streaming_debug_imgui.md": (
        "Streaming",
        ["CollectStreamingMemoryDebug", "DrawAudioStreamingMemoryDebugWindow", "GameOrder"],
    ),
    "feature-memos/debug_test_play_area.md": (
        "Editor",
        ["debug", "test", "play", "area"],
    ),
    "feature-memos/cloud_build_vcxproj_check.md": (
        "Infra",
        ["verify_vcxproj_coverage", "vcxproj", "CG2_BuildUp"],
    ),
    "feature-memos/operator_guidance_bubble.md": (
        "UX",
        ["OperatorGuidance", "operator_guidance"],
    ),
}

SECTION_UC = re.compile(r"^## (UC-[A-Za-z]+)")
SCENARIO_LINE = re.compile(
    r"^- \*\*([A-Z]{3}-[A-Za-z0-9-]+)\*\* — (.+?)(?:（spec: `([^`]+)`）)?\s*$"
)


@dataclass
class Scenario:
    scenario_id: str
    use_case: str
    text: str
    spec: str | None

    def area_hint(self) -> str:
        for prefix, area in PREFIX_AREA:
            if self.scenario_id.startswith(prefix):
                return area
        if self.spec and self.spec in SPEC_HINTS:
            return SPEC_HINTS[self.spec][0]
        return "Runtime"

    def search_hints(self) -> list[str]:
        hints: list[str] = []
        if self.spec and self.spec in SPEC_HINTS:
            hints.extend(SPEC_HINTS[self.spec][1])
        token = self.scenario_id.split("-", 1)[-1].split("-")[0]
        if token and token not in hints:
            hints.append(token)
        return hints[:8]


def load_json(path: Path, default: dict) -> dict:
    if not path.is_file():
        return default
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def parse_scenarios(md_text: str) -> list[Scenario]:
    current_uc = ""
    out: list[Scenario] = []
    for line in md_text.splitlines():
        m_sec = SECTION_UC.match(line.strip())
        if m_sec:
            current_uc = m_sec.group(1)
            continue
        m = SCENARIO_LINE.match(line.strip())
        if m and current_uc:
            out.append(
                Scenario(
                    scenario_id=m.group(1),
                    use_case=current_uc,
                    text=m.group(2).strip(),
                    spec=m.group(3),
                )
            )
    return out


def scenario_ids_from_scans(index: dict) -> dict[str, int]:
    counts: dict[str, int] = {}
    for scan in index.get("scans", []):
        sid = scan.get("scenarioId") or ""
        if sid:
            counts[sid] = counts.get(sid, 0) + 1
    return counts


def priority_score(
    sc: Scenario,
    scan_counts: dict[str, int],
    rotation: list[str],
    rotation_cursor: int,
) -> tuple[int, list[str]]:
    reasons: list[str] = []
    score = USE_CASE_WEIGHT.get(sc.use_case, 500)
    reasons.append(f"useCaseWeight={score}")

    times = scan_counts.get(sc.scenario_id, 0)
    if times == 0:
        score += 500
        reasons.append("neverScanned=+500")
    else:
        score += max(0, 120 - times * 40)
        reasons.append(f"scanCount={times}")

    try:
        uc_index = rotation.index(sc.use_case)
    except ValueError:
        uc_index = 0
    dist = (uc_index - rotation_cursor) % len(rotation)
    rot_bonus = max(0, 80 - dist * 20)
    score += rot_bonus
    reasons.append(f"rotationBonus=+{rot_bonus}")

    if sc.spec and "streaming" in sc.spec.lower():
        score += 150
        reasons.append("streamingSpec=+150")
    if sc.scenario_id.startswith("DBG-FeatureScene"):
        score += 120
        reasons.append("featureSceneScenario=+120")

    return score, reasons


def make_scan_id() -> str:
    now = datetime.now(JST)
    return now.strftime("SCAN-%Y%m%d-%H%M%S")


def main() -> int:
    state = load_json(
        STATE_JSON,
        {
            "schemaVersion": 1,
            "useCaseRotation": list(USE_CASE_WEIGHT.keys()),
            "rotationCursor": 0,
            "scenarioScanCounts": {},
            "lastCompletedScanId": "",
            "currentPlannedScanId": "",
            "totalTicksCompleted": 0,
        },
    )
    index = load_json(INDEX_JSON, {"schemaVersion": 2, "gaps": {}, "scopeToId": {}, "scans": []})

    if TICK_JSON.is_file():
        existing = load_json(TICK_JSON, {})
        if existing.get("status") == "in_progress" and existing.get("scanId"):
            print(json.dumps({"reused": True, "tick": existing}, ensure_ascii=False, indent=2))
            return 0

    if not SCENARIOS_MD.is_file():
        print("missing use_case_scenarios.md", file=sys.stderr)
        return 1

    scenarios = parse_scenarios(SCENARIOS_MD.read_text(encoding="utf-8"))
    if not scenarios:
        print("no scenarios parsed", file=sys.stderr)
        return 1

    rotation = state.get("useCaseRotation") or list(USE_CASE_WEIGHT.keys())
    rotation_cursor = int(state.get("rotationCursor", 0)) % max(1, len(rotation))
    scan_counts = scenario_ids_from_scans(index)
    state_counts = state.get("scenarioScanCounts") or {}
    for k, v in state_counts.items():
        scan_counts[k] = max(scan_counts.get(k, 0), int(v))

    ranked: list[tuple[int, Scenario, list[str]]] = []
    for sc in scenarios:
        score, reasons = priority_score(sc, scan_counts, rotation, rotation_cursor)
        ranked.append((score, sc, reasons))
    ranked.sort(key=lambda t: (-t[0], t[1].scenario_id))

    _, chosen, reason_parts = ranked[0]
    scan_id = make_scan_id()

    tick = {
        "schemaVersion": 1,
        "status": "in_progress",
        "scanId": scan_id,
        "keyword": "ギャップスキャンを実行して",
        "primaryUseCase": chosen.use_case,
        "useCaseLabel": USE_CASE_LABEL.get(chosen.use_case, chosen.use_case),
        "scenarioId": chosen.scenario_id,
        "scenarioText": chosen.text,
        "spec": chosen.spec,
        "areaHint": chosen.area_hint(),
        "searchHints": chosen.search_hints(),
        "priorityScore": ranked[0][0],
        "priorityReasons": reason_parts,
        "plannedAt": datetime.now(JST).isoformat(),
        "tickInstructions": [
            "cursorInstructions.md §6.6 に従い、上記 scenario 1 件のみを対象にコード／仕様を突合する。",
            "新規 GAP は implementation_gaps_index.json の scopeToId で重複禁止。",
            "完了後: recordGapScanTick.py を実行し、必要なら syncImplementationGapsMd.py。",
            "progress_report.md に scanId・useCase・newIds 件数を追記。",
            "本 tick はビルド不要。Git 作業ブランチ不要（ドキュメントのみ）。",
        ],
        "alternates": [
            {"scenarioId": s.scenario_id, "score": sc}
            for sc, s, _ in ranked[1:4]
        ],
    }

    TICK_JSON.write_text(json.dumps(tick, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    state["currentPlannedScanId"] = scan_id
    STATE_JSON.write_text(json.dumps(state, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    print(json.dumps({"reused": False, "tick": tick}, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
