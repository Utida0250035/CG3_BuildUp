#!/usr/bin/env python3
"""manifest 駆動で %Token% テンプレから C++ ファイルを生成する。"""

from __future__ import annotations

import argparse
import json
import re
import sys
from pathlib import Path

EDITOR_ASSETS = Path(__file__).resolve().parents[2]
REPO_ROOT = EDITOR_ASSETS.parent
TEMPLATE_DIR = EDITOR_ASSETS / "Templates" / "Code"
MANIFEST_PATH = TEMPLATE_DIR / "manifest.json"

CLASS_NAME_PATTERN = re.compile(r"^[A-Za-z][A-Za-z0-9_]*$")
TOKEN_PATTERN = re.compile(r"%([A-Za-z][A-Za-z0-9_]*)%")


def load_manifest() -> dict:
    if not MANIFEST_PATH.is_file():
        raise FileNotFoundError(f"manifest not found: {MANIFEST_PATH}")
    return json.loads(MANIFEST_PATH.read_text(encoding="utf-8"))


def find_template(manifest: dict, template_id: str) -> dict:
    for entry in manifest.get("templates", []):
        if entry.get("id") == template_id:
            return entry
    raise KeyError(f"unknown template id: {template_id}")


def substitute(text: str, tokens: dict[str, str]) -> str:
    def repl(match: re.Match[str]) -> str:
        key = match.group(1)
        if key not in tokens:
            raise KeyError(f"undefined token %{key}%")
        return tokens[key]

    return TOKEN_PATTERN.sub(repl, text)


def build_token_map(args: argparse.Namespace, template: dict) -> dict[str, str]:
    tokens = {
        "ClassName": args.class_name,
        "AuthorNote": args.author_note or "",
    }
    if args.upd_group:
        tokens["UpdCmpGroup"] = args.upd_group

    required = template.get("requiredTokens", [])
    missing = [t for t in required if not tokens.get(t)]
    if missing:
        raise ValueError(f"missing required tokens: {', '.join(missing)}")
    return tokens


def cmd_list(_: argparse.Namespace) -> int:
    manifest = load_manifest()
    for entry in manifest.get("templates", []):
        print(f"{entry.get('id')}: {entry.get('label', '')}")
        for token in entry.get("requiredTokens", []):
            print(f"  required: %{token}%")
    return 0


def cmd_scaffold(args: argparse.Namespace) -> int:
    if not CLASS_NAME_PATTERN.match(args.class_name):
        print(
            "ClassName must match [A-Za-z][A-Za-z0-9_]*",
            file=sys.stderr,
        )
        return 1

    manifest = load_manifest()
    template = find_template(manifest, args.template_id)
    tokens = build_token_map(args, template)

    planned: list[tuple[Path, str]] = []
    for output in template.get("outputs", []):
        rel_out = substitute(output["relativePath"], tokens)
        out_path = (REPO_ROOT / rel_out).resolve()
        if not str(out_path).startswith(str(REPO_ROOT / "Project")):
            print(f"refusing path outside Project: {out_path}", file=sys.stderr)
            return 1

        template_file = TEMPLATE_DIR / output["templateFile"]
        if not template_file.is_file():
            print(f"missing template file: {template_file}", file=sys.stderr)
            return 1

        body = template_file.read_text(encoding="utf-8")
        body = substitute(body, tokens)
        if not tokens.get("AuthorNote"):
            body = "\n".join(
                line for line in body.splitlines() if not line.strip().startswith("// %AuthorNote%")
            )
            if not body.endswith("\n"):
                body += "\n"
        planned.append((out_path, body))

    for out_path, _ in planned:
        if out_path.exists() and not args.force:
            print(f"exists (use --force): {out_path}", file=sys.stderr)
            return 1

    for out_path, body in planned:
        if args.dry_run:
            print(f"DRY-RUN write {out_path} ({len(body)} bytes)")
            continue
        out_path.parent.mkdir(parents=True, exist_ok=True)
        out_path.write_text(body, encoding="utf-8", newline="\n")
        print(f"Wrote {out_path.relative_to(REPO_ROOT)}")

    if not args.dry_run:
        cpp_written = [p for p, _ in planned if p.suffix == ".cpp"]
        if cpp_written:
            print(
                "Reminder: add ClCompile/ClInclude to Project/CG2_BuildUp.vcxproj "
                "(or run verify_vcxproj_coverage.py after)."
            )
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description="Scaffold C++ from EditorAssets templates")
    sub = parser.add_subparsers(dest="command", required=True)

    list_parser = sub.add_parser("list", help="List template ids")
    list_parser.set_defaults(func=cmd_list)

    gen = sub.add_parser("generate", help="Generate files from a template")
    gen.add_argument("template_id", help="manifest template id")
    gen.add_argument("class_name", help="Value for %%ClassName%%")
    gen.add_argument(
        "--upd-group",
        help="Value for %%UpdCmpGroup%% (component_upd)",
    )
    gen.add_argument("--author-note", default="", help="Optional %%AuthorNote%%")
    gen.add_argument("--dry-run", action="store_true", help="Print paths only")
    gen.add_argument("--force", action="store_true", help="Overwrite existing files")
    gen.set_defaults(func=cmd_scaffold)

    args = parser.parse_args()
    try:
        return args.func(args)
    except (FileNotFoundError, KeyError, ValueError) as err:
        print(err, file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
