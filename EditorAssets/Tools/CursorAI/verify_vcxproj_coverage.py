#!/usr/bin/env python3
"""CG2_BuildUp.vcxproj とソース／シェーダの整合を検査し、ビルド穴を検出する。"""

from __future__ import annotations

import re
import sys
from pathlib import Path


def repo_root() -> Path:
    here = Path(__file__).resolve().parent
    return here.parents[2]


def load_allowlist(path: Path) -> set[str]:
    allowed: set[str] = set()
    if not path.is_file():
        return allowed
    for line in path.read_text(encoding="utf-8").splitlines():
        line = line.split("#", 1)[0].strip()
        if line:
            allowed.add(line.replace("\\", "/"))
    return allowed


def normalize_vcx_path(raw: str) -> str:
    return raw.replace("\\", "/")


def parse_vcxproj(vcx_path: Path) -> tuple[set[str], set[str], set[str], set[str]]:
    text = vcx_path.read_text(encoding="utf-8")
    compiled: set[str] = set()
    excluded_development: set[str] = set()
    excluded_release_only: set[str] = set()

    block_pattern = re.compile(
        r'<ClCompile Include="([^"]+)"\s*(?:/>|>(.*?)</ClCompile>)',
        re.DOTALL,
    )
    dev_exclude = re.compile(
        r"ExcludedFromBuild\s+Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=='Development\|x64'\""
        r">\s*true\s*<",
    )
    release_only = re.compile(
        r"ExcludedFromBuild\s+Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=='Release\|x64'\""
        r">\s*true\s*<",
    )

    for match in block_pattern.finditer(text):
        path = normalize_vcx_path(match.group(1))
        compiled.add(path)
        inner = match.group(2)
        if inner:
            if dev_exclude.search(inner):
                excluded_development.add(path)
            elif release_only.search(inner) and not dev_exclude.search(inner):
                excluded_release_only.add(path)

    stale: set[str] = set()
    project_root = vcx_path.parent
    for path in compiled:
        if path.startswith("Codes/") and not (project_root / path).is_file():
            stale.add(path)

    return compiled, excluded_development, excluded_release_only, stale


def collect_source_cpp(project_root: Path) -> set[str]:
    source_root = project_root / "Codes" / "Source"
    if not source_root.is_dir():
        return set()
    return {
        p.relative_to(project_root).as_posix()
        for p in source_root.rglob("*.cpp")
    }


def collect_shader_paths(shader_compiler: Path, project_root: Path) -> set[str]:
    if not shader_compiler.is_file():
        return set()
    text = shader_compiler.read_text(encoding="utf-8")
    paths: set[str] = set()
    for raw in re.findall(r'L"Assets/Shaders/[^"]+"', text):
        rel = raw[2:-1].replace("/", "/")
        paths.add(rel)
    missing: set[str] = set()
    for rel in paths:
        if not (project_root / rel).is_file():
            missing.add(rel)
    return missing


def main() -> int:
    root = repo_root()
    project_root = root / "Project"
    vcxproj = project_root / "CG2_BuildUp.vcxproj"
    allowlist_path = root / "EditorAssets" / "Tools" / "CursorAI" / "vcxproj_coverage_allowlist.txt"

    if not vcxproj.is_file():
        print(f"ERROR: missing vcxproj: {vcxproj}", file=sys.stderr)
        return 2

    allowlist = load_allowlist(allowlist_path)
    compiled, excluded_dev, excluded_release, stale = parse_vcxproj(vcxproj)
    sources = collect_source_cpp(project_root)

    missing = sorted(sources - compiled - allowlist)
    allowlisted_orphans = sorted((sources - compiled) & allowlist)

    shader_compiler = (
        project_root / "Codes" / "Source" / "Engine" / "Renderer" / "Shader" / "ShaderCompiler.cpp"
    )
    missing_shaders = collect_shader_paths(shader_compiler, project_root)

    errors: list[str] = []
    warnings: list[str] = []

    if missing:
        errors.append(
            "The following .cpp files exist under Project/Codes/Source but are not listed in "
            "CG2_BuildUp.vcxproj (add ClCompile or allowlist with justification):"
        )
        for p in missing:
            errors.append(f"  - {p}")

    if stale:
        errors.append("Stale ClCompile entries (file missing on disk):")
        for p in sorted(stale):
            errors.append(f"  - {p}")

    if missing_shaders:
        errors.append("ShaderCompiler references missing hlsl files:")
        for p in sorted(missing_shaders):
            errors.append(f"  - {p}")

    if excluded_dev:
        warnings.append(
            "ClCompile excluded from Development|x64 (CI build will not compile these units):"
        )
        for p in sorted(excluded_dev):
            warnings.append(f"  - {p}")

    if excluded_release:
        warnings.append("Release-only excluded sources (Development CI still compiles them):")
        for p in sorted(excluded_release)[:12]:
            warnings.append(f"  - {p}")
        if len(excluded_release) > 12:
            warnings.append(f"  ... and {len(excluded_release) - 12} more")

    if allowlisted_orphans:
        warnings.append("Allowlisted .cpp not in vcxproj (documented gap):")
        for p in allowlisted_orphans:
            warnings.append(f"  - {p}")

    print(f"vcxproj coverage: {len(compiled)} ClCompile entries, {len(sources)} source .cpp")
    for block in warnings:
        print(block)
    for line in errors:
        print(line, file=sys.stderr)

    if errors:
        return 1

    print("verify_vcxproj_coverage: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
