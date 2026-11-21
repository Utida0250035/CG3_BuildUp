#!/usr/bin/env bash
# Cloud Agent / Linux 上で buildDevelopment.cmd の代わりに実行する前段検証（MSBuild は Windows CI／ローカル）
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"
cd "$REPO_ROOT"

TOOLS="$SCRIPT_DIR"

echo "== Cloud Development preflight (Linux) =="

python3 "$TOOLS/verify_vcxproj_coverage.py"
python3 "$TOOLS/verify_editor_component_registration.py"
python3 "$TOOLS/verify_config_matrix.py"
python3 "$TOOLS/verify_build_development_ci_parity.py"
python3 "$TOOLS/verify_build_development_nuget_restore.py"
python3 "$TOOLS/verify_asset_pack_paths.py"

echo "cloud Development preflight: OK"