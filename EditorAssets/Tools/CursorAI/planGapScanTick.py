#!/usr/bin/env python3
"""Select exactly one gap-scan scope (1 tick) from ultimateGoal.md.

「完成形の説明」の `・` 行をファイル順に一巡し、その後だけ「エンジンとしての前提」を見る。
"""

from __future__ import annotations

import hashlib
import json
import re
import sys
from dataclasses import dataclass
from datetime import datetime, timezone, timedelta
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DOCS = ROOT / "docs"
GOAL_MD = DOCS / "ultimateGoal.md"
INDEX_JSON = DOCS / "implementation_gaps_index.json"
STATE_JSON = ROOT / "gap_scan_state.json"
TICK_JSON = ROOT / "gap_scan_tick.json"

JST = timezone(timedelta(hours=9))

GOAL_SECTION = "完成形の説明"
BASE_SECTION = "エンジンとしての前提"

BR_TAG = re.compile(r"</?br\s*/?>", re.IGNORECASE)

USE_CASE_LABEL = {
    "UC-Play": "Play",
    "UC-Authoring": "Authoring",
    "UC-Debug": "Debug",
    "UC-Ship": "Ship",
}

# 先に一致した用途を summary ラベルに使う。選定順には使わない。
USE_CASE_KEYWORDS: list[tuple[str, tuple[str, ...]]] = [
    ("UC-Debug", ("デバッグ", "メモリ", "Gui", "GUI", "グラフ", "失敗")),
    ("UC-Ship", ("配布", "同梱", "圧縮", "テンプレートexe", "exe化", "出力先", "垂直同期", "全画面")),
    ("UC-Authoring", ("エディタ", "インスペクタ", "配置", "保存", "アセット", "json", "プレハブ")),
]

# 先に一致した Area を検索の足がかりにする。
AREA_KEYWORDS: list[tuple[str, tuple[str, ...]]] = [
    ("Streaming", ("ストリーミング",)),
    ("Render", ("ライト", "Fog", "フォグ", "グラフィ", "描画", "マテリアル", "解像度", "垂直同期", "スプライト")),
    ("Scene", ("シーン",)),
    ("Resource", ("png", "obj", "gltf", "dds", "バイナリ", "圧縮", "リソース", "音源", "wav")),
    ("Editor", ("エディタ", "インスペクタ", "Gui", "GUI")),
    ("Infra", ("exe", "同梱", "vcxproj")),
    ("UX", ("ヒント", "ナビ", "HUD", "吹き出し", "カメラ")),
]


@dataclass
class GoalLine:
    source: str
    order: int
    big: str
    mid: str
    text: str

    @property
    def scenario_id(self) -> str:
        prefix = "UG-G" if self.source == "goal" else "UG-B"
        raw = f"{self.big}\n{self.mid}\n{self.text}".encode("utf-8")
        return f"{prefix}-{hashlib.sha1(raw).hexdigest()[:8]}"

    def use_case(self) -> str:
        blob = f"{self.big} {self.mid} {self.text}"
        for uc, words in USE_CASE_KEYWORDS:
            if any(word in blob for word in words):
                return uc
        return "UC-Play"

    def area_hint(self) -> str:
        blob = f"{self.big} {self.mid} {self.text}"
        for area, words in AREA_KEYWORDS:
            if any(word in blob for word in words):
                return area
        return "Runtime"

    def search_hints(self) -> list[str]:
        hints: list[str] = []
        for token in re.findall(r"[A-Za-z][A-Za-z0-9_]{2,}", f"{self.big} {self.mid} {self.text}"):
            if token not in hints:
                hints.append(token)
        for extra in (self.big, self.mid):
            if extra and extra not in hints:
                hints.append(extra)
        return hints[:8]


def load_json(path: Path, default: dict) -> dict:
    if not path.is_file():
        return default
    with path.open(encoding="utf-8") as f:
        return json.load(f)


def clean_line(raw: str) -> str:
    return BR_TAG.sub("", raw).strip()


def parse_goal_lines(md_text: str) -> list[GoalLine]:
    section = ""
    big = ""
    mid = ""
    out: list[GoalLine] = []
    for raw in md_text.splitlines():
        line = clean_line(raw)
        if line.startswith("## "):
            title = line[3:].strip()
            if title == GOAL_SECTION:
                section = "goal"
            elif title == BASE_SECTION:
                section = "baseline"
            else:
                section = ""
            big = ""
            mid = ""
            continue
        if not section:
            continue
        if line.startswith("[") and line.endswith("]") and len(line) >= 2:
            big = line[1:-1].strip()
            mid = ""
            continue
        if line.startswith("〇"):
            mid = line[1:].strip()
            continue
        if not line.startswith("・"):
            continue
        text = line[1:].strip()
        if not text or text == "（未記入）":
            continue
        out.append(
            GoalLine(
                source=section,
                order=len(out) + 1,
                big=big,
                mid=mid,
                text=text,
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


def choose_line(lines: list[GoalLine], counts: dict[str, int]) -> tuple[GoalLine, list[str]]:
    """完成形をファイル順に一巡してから、前提をファイル順に一巡する。"""
    goals = [line for line in lines if line.source == "goal"]
    bases = [line for line in lines if line.source == "baseline"]
    if not goals:
        raise RuntimeError("完成形の説明に・行が無い")

    def count_of(line: GoalLine) -> int:
        return counts.get(line.scenario_id, 0)

    pending_goal = [line for line in goals if count_of(line) == 0]
    if pending_goal:
        return pending_goal[0], ["source=goal", "pass=初回", "fileOrder"]

    pending_base = [line for line in bases if count_of(line) == 0]
    if pending_base:
        return pending_base[0], ["source=baseline", "goalPass=完了", "fileOrder"]

    min_goal = min(count_of(line) for line in goals)
    if any(count_of(line) != min_goal for line in goals):
        pending = [line for line in goals if count_of(line) == min_goal]
        return pending[0], ["source=goal", f"scanCount={min_goal}", "fileOrder"]

    behind_base = [line for line in bases if count_of(line) < min_goal]
    if behind_base:
        return behind_base[0], ["source=baseline", f"catchUpTo={min_goal}", "fileOrder"]

    pending = [line for line in goals if count_of(line) == min_goal]
    return pending[0], ["source=goal", f"nextPass={min_goal + 1}", "fileOrder"]


def make_scan_id() -> str:
    now = datetime.now(JST)
    return now.strftime("SCAN-%Y%m%d-%H%M%S")


def main() -> int:
    state = load_json(
        STATE_JSON,
        {
            "schemaVersion": 1,
            "useCaseRotation": ["UC-Play", "UC-Authoring", "UC-Debug", "UC-Ship"],
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

    if not GOAL_MD.is_file():
        print("missing ultimateGoal.md", file=sys.stderr)
        return 1

    lines = parse_goal_lines(GOAL_MD.read_text(encoding="utf-8"))
    if not any(line.source == "goal" for line in lines):
        print("no goal lines parsed", file=sys.stderr)
        return 1

    scan_counts = scenario_ids_from_scans(index)
    state_counts = state.get("scenarioScanCounts") or {}
    for key, value in state_counts.items():
        scan_counts[key] = max(scan_counts.get(key, 0), int(value))

    chosen, reason_parts = choose_line(lines, scan_counts)
    same_source = [line for line in lines if line.source == chosen.source and line.scenario_id != chosen.scenario_id]
    scan_id = make_scan_id()
    use_case = chosen.use_case()

    tick = {
        "schemaVersion": 2,
        "status": "in_progress",
        "scanId": scan_id,
        "keyword": "ギャップスキャンを実行して",
        "source": chosen.source,
        "sourceLabel": "完成形の説明" if chosen.source == "goal" else "エンジンとしての前提",
        "order": chosen.order,
        "headingBig": chosen.big,
        "headingMid": chosen.mid,
        "primaryUseCase": use_case,
        "useCaseLabel": USE_CASE_LABEL.get(use_case, use_case),
        "scenarioId": chosen.scenario_id,
        "scenarioText": chosen.text,
        "spec": "docs/ultimateGoal.md",
        "areaHint": chosen.area_hint(),
        "searchHints": chosen.search_hints(),
        "priorityScore": 1000 if chosen.source == "goal" else 100,
        "priorityReasons": reason_parts,
        "plannedAt": datetime.now(JST).isoformat(),
        "tickInstructions": [
            "cursorInstructions.md §6.6 に従い、scenarioText の・行 1 件だけをコードと突合する。",
            "[] と 〇 は対象にしない。source が baseline のときも、その 1 行だけを見る。",
            "新規 GAP は implementation_gaps_index.json の scopeToId で重複禁止。",
            "完了後: recordGapScanTick.py を実行し、必要なら syncImplementationGapsMd.py。",
            "progress_report.md に scanId・source・scenarioId・newIds 件数を追記。",
            "本 tick はビルド不要。Git 作業ブランチ不要（ドキュメントのみ）。",
        ],
        "alternates": [
            {"scenarioId": line.scenario_id, "source": line.source, "order": line.order}
            for line in same_source[:3]
        ],
    }

    TICK_JSON.write_text(json.dumps(tick, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    state["selection"] = "ultimateGoal"
    state["currentPlannedScanId"] = scan_id
    STATE_JSON.write_text(json.dumps(state, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")

    print(json.dumps({"reused": False, "tick": tick}, ensure_ascii=False, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
