# -*- coding: utf-8 -*-
"""implementation_gaps_index.json から roadmap.md へギャップ追補節を挿入する（一回限り運用可）。"""
from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parent
INDEX_PATH = ROOT / "docs" / "implementation_gaps_index.json"
ROADMAP_PATH = ROOT / "docs" / "feature-memos" / "roadmap.md"

MAP_ID = {
    "Audio": "Audio",
    "Scene": "Scene",
    "Editor": "Editor",
    "Render": "Render",
    "Camera": "Camera",
    "UX": "UX",
    "Debug": "Debug",
    "Infra": "Repository",
    "Ship": "Repository",
    "Streaming": "Loader",
    "Resource": "Loader",
    "Runtime": "System",
    "Entity": "System",
    "Engine": "Debug",
}

P1_SECTIONS = {
    "Audio": "Audio",
    "Debug": "Debug",
    "Camera": "Camera",
    "Scene": "Scene",
    "Render": "Render",
    "Repository": "Repository",
    "Editor": "Editor",
    "UX": "UX",
}

P23_BACKLOG = {
    "Audio": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] シーン遷移時の BGM／SE 継続・クロスフェード・遷移 SE（CommandScene / ChangeSceneFade 系。在庫 45件）\n"
        "- [ ] AudioManager ストリーミング／デコーダ／ボイスコールバック境界（在庫内の AudioManager 中心）\n"
        "  - 正本: `../implementation_gaps.md` / `implementation_gaps_index.json`（Area=Audio）"
    ),
    "Scene": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] SceneBuildUp 検証経路・レジストリ・cameraRig 接続の細部（在庫 65件）\n"
        "- [ ] CommandScene 遷移要求・Development 起動シーン不一致\n"
        "  - 正本: `../implementation_gaps.md`（Area=Scene）"
    ),
    "Render": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] SceneGame 描画／ライティング反映の残件（在庫 6件）\n"
        "  - 正本: `../implementation_gaps.md`（Area=Render）"
    ),
    "Debug": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] フレーム時間オーバーレイ・ストリーミングデバッグメニュー導線（在庫 2件）\n"
        "  - 正本: `../implementation_gaps.md`（Area=Debug）"
    ),
    "Repository": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] Release 構成・同梱パス・vcxproj 除外の細部（在庫 25件）\n"
        "  - 正本: `../implementation_gaps.md`（Area=Infra / Ship）"
    ),
    "Editor": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] EntityEditorUi／インスペクタ・プレハブ・タグカタログ導線（在庫 92件・Authoring 中心）\n"
        "- [ ] SceneEntityEditor プレビューと本番 SceneBuildUp 経路の差分\n"
        "  - 正本: `../implementation_gaps.md`（Area=Editor）"
    ),
    "UX": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] OperatorGuidance の Authoring 細部（在庫 5件）\n"
        "  - 正本: `../implementation_gaps.md`（Area=UX）"
    ),
    "System": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] Entity／Runtime まわり残件（在庫 3件）\n"
        "  - 正本: `../implementation_gaps.md`"
    ),
    "Camera": (
        "### ギャップスキャン追補（P2/P3・在庫）\n"
        "- [ ] tuning.json と StageViewCamera 定数の自動同期（在庫 1件）\n"
        "  - 正本: `../implementation_gaps.md`"
    ),
}

GAP_HEADER = (
    "**ギャップ在庫:** `../implementation_gaps.md`"
    "（2026-11-11 時点・未解消 289。P0/P1 は各節「ギャップスキャン追補」、P2/P3 は在庫行＋正本参照）"
)

GAP_INDEX = [
    "## GapScan（索引）",
    "",
    "- 機械正本: `../implementation_gaps_index.json`",
    "- 人間向け一覧: `../implementation_gaps.md`（tier 降順・UseCase 束ね）",
    "- 半自律 tick: `cursorInstructions.md` §6.6（「ギャップスキャンを実行して」）",
    "- **取り込み方針:** 本ファイルは P0/P1 を実装タスク化。P2/P3 は節末在庫＋正本で管理（重複 ID は `scopeKey` マージ済み）",
    "",
]

MARKER_P01 = "### ギャップスキャン追補（P0/P1）"
MARKER_P23 = "### ギャップスキャン追補（P2/P3・在庫）"


def gap_section_key(gap_id: str) -> str:
    return MAP_ID.get(gap_id.split("-")[1], gap_id.split("-")[1])


def summary_text(gap: dict) -> str:
    s = gap.get("summaryShort") or gap.get("summary", "")
    for tag in ("【Play】", "【Authoring】", "【Debug】", "【Ship】"):
        s = s.replace(tag, "")
    return s.strip()


def p01_block(gkey: str, gaps: dict) -> str:
    items: list[tuple[int, str]] = []
    for gap_id, gap in gaps.items():
        if gap.get("status") not in ("open", "investigating"):
            continue
        if gap.get("tier") not in ("P0", "P1"):
            continue
        if gap_section_key(gap_id) != gkey:
            continue
        uc = gap.get("primaryUseCase", "").replace("UC-", "")
        line = f"- [ ] {summary_text(gap)}（gap: `{gap_id}`・{uc}・{gap.get('tier')}）"
        items.append((gap.get("score", 0), line))
    if not items:
        return ""
    items.sort(reverse=True)
    return MARKER_P01 + "\n" + "\n".join(line for _, line in items)


def inject_block(section_name: str, gaps: dict) -> str:
    parts: list[str] = []
    gkey = P1_SECTIONS.get(section_name)
    if gkey:
        block = p01_block(gkey, gaps)
        if block:
            parts.append(block)
    if section_name in P23_BACKLOG:
        parts.append(P23_BACKLOG[section_name])
    return "\n\n".join(parts)


def strip_existing_gap_blocks(lines: list[str]) -> list[str]:
    """既存のギャップ追補ブロックを除去（再実行用）。"""
    out: list[str] = []
    i = 0
    while i < len(lines):
        line = lines[i]
        if line in (MARKER_P01, MARKER_P23):
            i += 1
            while i < len(lines) and not lines[i].startswith("## "):
                i += 1
            continue
        if line.startswith("**ギャップ在庫:**"):
            i += 1
            continue
        if line == "## GapScan（索引）":
            i += 1
            while i < len(lines) and not lines[i].startswith("## "):
                i += 1
            continue
        out.append(line)
        i += 1
    return out


def main() -> None:
    gaps = json.loads(INDEX_PATH.read_text(encoding="utf-8"))["gaps"]
    lines = ROADMAP_PATH.read_text(encoding="utf-8").splitlines()
    lines = strip_existing_gap_blocks(lines)

    # ヘッダにギャップ在庫行
    out: list[str] = []
    header_done = False
    for line in lines:
        out.append(line)
        if not header_done and line.startswith("**チェック規則:**"):
            out.append("")
            out.append(GAP_HEADER)
            header_done = True

    # 各 ## 節の末尾へ挿入（後ろから挿入してインデックスを崩さない）
    section_starts: list[tuple[int, str]] = []
    for i, line in enumerate(out):
        if line.startswith("## ") and not line.startswith("## メモ"):
            section_starts.append((i, line[3:].strip()))

    for idx in range(len(section_starts) - 1, -1, -1):
        start_i, name = section_starts[idx]
        if idx + 1 < len(section_starts):
            end_i = section_starts[idx + 1][0]
        else:
            end_i = len(out)
            for j in range(start_i + 1, len(out)):
                if out[j].startswith("## メモ"):
                    end_i = j
                    break
        block = inject_block(name, gaps)
        if not block:
            continue
        insert_lines = ["", block, ""]
        out[end_i:end_i] = insert_lines

    # GapScan 索引をメモ直前に
    memo_i = next((i for i, line in enumerate(out) if line.startswith("## メモ")), len(out))
    out[memo_i:memo_i] = GAP_INDEX

    ROADMAP_PATH.write_text("\n".join(out) + "\n", encoding="utf-8")
    print(f"Updated {ROADMAP_PATH} ({len(out)} lines)")


if __name__ == "__main__":
    main()
