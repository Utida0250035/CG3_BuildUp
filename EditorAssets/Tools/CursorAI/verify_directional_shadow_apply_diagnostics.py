#!/usr/bin/env python3
"""DirectionalShadow の適用状態ログ・可視化の配線を静的検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]


def main() -> int:
    errors: list[str] = []
    shadow_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "Engine"
        / "Renderer"
        / "Lighting"
        / "DirectionalShadow.cpp"
    )
    diag_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "Engine"
        / "Renderer"
        / "Lighting"
        / "DirectionalShadowApplyDiagnostics.cpp"
    )
    panel_cpp = (
        ROOT
        / "Project"
        / "Codes"
        / "Source"
        / "Engine"
        / "Renderer"
        / "Lighting"
        / "LightingEffectsImGuiPanel.cpp"
    )
    log_h = ROOT / "Project" / "Codes" / "Include" / "ForDebug" / "DirectionalShadowApplyDebugLog.h"
    fog_md = (
        ROOT
        / "EditorAssets"
        / "Tools"
        / "CursorAI"
        / "docs"
        / "feature-memos"
        / "directional_shadow.md"
    )

    if shadow_cpp.is_file():
        text = shadow_cpp.read_text(encoding="utf-8", errors="replace")
        if "RecordPlanarGroundShadowSkip" not in text:
            errors.append("DirectionalShadow.cpp: expected skip diagnostics")
        if "RecordPlanarGroundShadowDraw" not in text:
            errors.append("DirectionalShadow.cpp: expected draw diagnostics")
        if "RecordDirectionalLightDataMerge" not in text:
            errors.append("DirectionalShadow.cpp: expected merge diagnostics")
    else:
        errors.append("missing DirectionalShadow.cpp")

    if not diag_cpp.is_file():
        errors.append("missing DirectionalShadowApplyDiagnostics.cpp")
    elif "DrawApplyStateOverlay" not in diag_cpp.read_text(encoding="utf-8", errors="replace"):
        errors.append("DirectionalShadowApplyDiagnostics: expected overlay")

    if panel_cpp.is_file():
        text = panel_cpp.read_text(encoding="utf-8", errors="replace")
        if "DrawApplyStateOverlay" not in text:
            errors.append("LightingEffectsImGuiPanel: expected apply overlay wire")
    else:
        errors.append("missing LightingEffectsImGuiPanel.cpp")

    if not log_h.is_file():
        errors.append("missing DirectionalShadowApplyDebugLog.h")

    if fog_md.is_file():
        md = fog_md.read_text(encoding="utf-8", errors="replace")
        if "DirectionalShadowApplyDiagnostics" not in md:
            errors.append("directional_shadow.md must document diagnostics")
        if "適用状態をログへ" not in md:
            errors.append("directional_shadow.md must mention log export")
    else:
        errors.append("missing directional_shadow.md")

    if errors:
        print("verify_directional_shadow_apply_diagnostics: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_directional_shadow_apply_diagnostics: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())