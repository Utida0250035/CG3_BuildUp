#!/usr/bin/env python3
"""Visual::Lighting が Engine と別 VolumetricFog を抱えていないことを検査。"""

from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
LIGHTING_H = ROOT / "Project" / "Codes" / "Include" / "Engine" / "Renderer" / "Lighting" / "Lighting.h"
BINDING_CPP = (
    ROOT
    / "Project"
    / "Codes"
    / "Source"
    / "Engine"
    / "Renderer"
    / "Lighting"
    / "LightingEngineEffectsBinding.cpp"
)


def main() -> int:
    errors: list[str] = []
    if not LIGHTING_H.is_file():
        errors.append("missing Lighting.h")
    else:
        text = LIGHTING_H.read_text(encoding="utf-8", errors="replace")
        if "VolumetricFog volumetricFog_" in text:
            errors.append("Lighting.h: must not own volumetricFog_")
        if "LightingEngineEffectsBinding" not in text and "SharedVolumetricFog" not in text:
            if "ファサード" not in text and "Engine" not in text:
                errors.append("Lighting.h: expected engine-shared facade")

    if not BINDING_CPP.is_file():
        errors.append("missing LightingEngineEffectsBinding.cpp")
    else:
        text = BINDING_CPP.read_text(encoding="utf-8", errors="replace")
        if "RefVolumetricFog" not in text:
            errors.append("LightingEngineEffectsBinding.cpp: expected RefVolumetricFog")

    if errors:
        print("verify_lighting_effects_shared: FAILED", file=sys.stderr)
        for line in errors:
            print(f"  - {line}", file=sys.stderr)
        return 1

    print("verify_lighting_effects_shared: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())