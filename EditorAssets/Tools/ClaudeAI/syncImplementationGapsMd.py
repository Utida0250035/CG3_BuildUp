#!/usr/bin/env python3
"""Refresh implementation_gaps.md meta, scan history, and gap list from index JSON."""

from __future__ import annotations

import json
import re
from datetime import datetime, timezone, timedelta
from pathlib import Path

ROOT = Path(__file__).resolve().parent
DOCS = ROOT / "docs"
INDEX_JSON = DOCS / "implementation_gaps_index.json"
GAPS_MD = DOCS / "implementation_gaps.md"

JST = timezone(timedelta(hours=9))
USE_CASE_LABEL = {
    "UC-Play": "Play",
    "UC-Authoring": "Authoring",
    "UC-Debug": "Debug",
    "UC-Ship": "Ship",
}


def label_for_summary(uc: str) -> str:
    return USE_CASE_LABEL.get(uc, uc.replace("UC-", ""))


def gap_block(gap_id: str, gap: dict) -> str:
    uc = gap.get("primaryUseCase", "")
    label = label_for_summary(uc)
    lines = [
        f"### {gap_id}",
        "",
        f"- status: {gap.get('status', 'open')}",
        f"- tier: {gap.get('tier', 'P3')}",
        f"- score: {gap.get('score', 0)}",
        f"- primaryUseCase: {uc}",
    ]
    sec = gap.get("secondaryUseCases") or []
    if sec:
        lines.append(f"- secondaryUseCases: {', '.join(sec)}")
    lines.extend(
        [
            f"- scenarioId: {gap.get('scenarioId', '')}",
            f"- frequency: {gap.get('frequency', 'unknown')}",
            f"- frequencyBasis: {gap.get('frequencyBasis', '')}",
            f"- scope: {gap.get('scopeKey', '')}",
            f"- primary: `{gap.get('primary', '')}`",
            f"- summary: 【{label}】{gap.get('summaryShort', gap.get('summary', ''))}",
            f"- scenarioText: {gap.get('scenarioText', '')}",
            "- evidence:",
        ]
    )
    for ev in gap.get("evidence") or ["（未記載）"]:
        lines.append(f"  - {ev}")
    if gap.get("suggestedUse"):
        lines.append(f"- suggestedUse: {gap['suggestedUse']}")
    if gap.get("roadmapLink"):
        lines.append(f"- roadmapLink: {gap['roadmapLink']}")
    lines.append(f"- firstSeenAt: {gap.get('firstSeenAt', '')}")
    lines.append(f"- lastSeenAt: {gap.get('lastSeenAt', '')}")
    lines.append("")
    return "\n".join(lines)


def main() -> int:
    if not INDEX_JSON.is_file():
        raise SystemExit("implementation_gaps_index.json missing")

    index = json.loads(INDEX_JSON.read_text(encoding="utf-8"))
    gaps = index.get("gaps") or {}
    pendings = index.get("pendings") or {}

    open_gaps = [
        (gid, g)
        for gid, g in gaps.items()
        if g.get("status") in ("open", "investigating")
    ]
    open_gaps.sort(key=lambda t: (-int(t[1].get("score", 0)), t[0]))

    archive = [
        (gid, g)
        for gid, g in gaps.items()
        if g.get("status") not in ("open", "investigating", None)
    ]

    scans = index.get("scans") or []
    last_scan = scans[-1]["at"] if scans else "（未実施）"

    pending_lines = ["なし"] if not pendings else []
    for pid, p in pendings.items():
        pending_lines.append(
            f"- {pid} | at: {p.get('at', '')} | hint: {p.get('hint', '')} | "
            f"確認したい用途候補: {p.get('useCaseCandidates', '')}"
        )

    gap_sections: list[str] = []
    if not open_gaps:
        gap_sections.append("（0 件）")
    else:
        by_uc: dict[str, list[tuple[str, dict]]] = {}
        for gid, g in open_gaps:
            uc = g.get("primaryUseCase", "UC-Play")
            by_uc.setdefault(uc, []).append((gid, g))
        for uc in sorted(by_uc.keys()):
            gap_sections.append(f"#### {uc}")
            gap_sections.append("")
            for gid, g in by_uc[uc]:
                gap_sections.append(gap_block(gid, g))

    archive_lines = ["なし"] if not archive else []
    for gid, g in archive:
        archive_lines.append(
            f"- {gid} | {g.get('status')} | {g.get('lastSeenAt', '')} | "
            f"{g.get('summaryShort', g.get('summary', ''))[:80]}"
        )

    scan_hist = ["なし"] if not scans else []
    for s in scans[-15:]:
        scan_hist.append(
            f"- scanId: {s.get('scanId')} | at: {s.get('at')} | "
            f"scenario: {s.get('scenarioId')} | new: {len(s.get('newIds') or [])} | "
            f"updated: {len(s.get('updatedIds') or [])}"
        )

    if not GAPS_MD.is_file():
        raise SystemExit("implementation_gaps.md missing")

    text = GAPS_MD.read_text(encoding="utf-8")
    now = datetime.now(JST).strftime("%Y-%m-%dT%H:%M:%S%z")

    text = re.sub(
        r"- \*\*lastIncrementalScanAt:\*\* .*",
        f"- **lastIncrementalScanAt:** {last_scan}",
        text,
        count=1,
    )

    # Manual section replacement for reliability
    sections = {
        "用途不明（確認待ち）": "\n".join(pending_lines),
        "ギャップ一覧": "\n".join(gap_sections),
        "Archive": "\n".join(archive_lines),
        "スキャン履歴（直近）": "\n".join(scan_hist),
    }
    for title, body in sections.items():
        pat = rf"(## {re.escape(title)}\n\n)(.*?)(\n\n---|\n\n## )"
        m = re.search(pat, text, flags=re.DOTALL)
        if m:
            text = text[: m.start(2)] + body + text[m.end(2) :]

    GAPS_MD.write_text(text, encoding="utf-8")
    print(f"synced {len(open_gaps)} open gaps, {len(scans)} scans")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
