import re
import json
import os
import sys

# 大文字2文字以上の連続を含むが、すべて大文字ではない単語を抽出する正規表現
BAD_PATTERN = re.compile(r"\b(?=[a-z]*[A-Z]{2,})[a-zA-Z]+\b")
ALL_UPPER_PATTERN = re.compile(r"^[A-Z]+$")


def load_config():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    base_path = os.path.join(script_dir, "ruleCheck.json")
    custom_path = os.path.join(script_dir, "customWords.json")

    # 基本構成
    config = {
        "target_extensions": [],
        "ignore_dirs": [],
        "exact_match": [],
        "partial_match": [],
    }

    # 無視するキーの定義
    ignored_custom_keys = {"target_extensions", "ignore_dirs"}

    # 1. base_rules.json を読み込み（基本設定）
    if os.path.exists(base_path):
        with open(base_path, "r", encoding="utf-8") as f:
            config.update(json.load(f))

    # 2. custom_rules.json を読み込み（データのみマージ）
    if os.path.exists(custom_path):
        with open(custom_path, "r", encoding="utf-8") as f:
            custom_data = json.load(f)
            for key, value in custom_data.items():
                # 指定したキーは無視
                if key in ignored_custom_keys:
                    continue

                # リスト型であれば結合、そうでなければ上書き
                if isinstance(value, list) and key in config:
                    config[key] = list(set(config[key] + value))
                else:
                    config[key] = value
    return config


def check_token(token, exact_list, partial_list):
    """
    1. 完全一致ならOK
    2. 部分一致するパーツを削る
    3. 残った部位に違反がないかチェック
    """
    # 1. 完全一致リストにあるか (優先)
    if token in exact_list:
        return False

    # 2. 部分一致パーツを削る
    temp_token = token
    for part in sorted(partial_list, key=len, reverse=True):
        temp_token = temp_token.replace(part, "")

    # 3. 削った結果、空になったらOK
    if not temp_token:
        return False

    # 4. 残ったものに対して大文字連続ルールを適用
    if BAD_PATTERN.match(temp_token) and not ALL_UPPER_PATTERN.match(temp_token):
        return True
    return False


def main():
    if len(sys.argv) < 2:
        print("Usage: python NameRuleChecker.py <project_root>")
        sys.exit(1)

    project_root = os.path.abspath(sys.argv[1])
    config = load_config()

    # 集合型にすることで比較を高速化
    ignore_dirs = set(config.get("ignore_dirs", []))
    exact_list = set(config.get("exact_match", []))
    partial_list = config.get("partial_match", [])

    print(f"Checking {project_root}...\n")

    for root, dirs, files in os.walk(project_root):
        # --- 除外処理 ---
        # 大文字小文字を区別して判定
        # dirsの中身をインプレースで更新し、サブディレクトリの探索を制御する
        dirs[:] = [d for d in dirs if d not in ignore_dirs]

        # --- デバッグ（確認用） ---
        # 意図通りに除外されているか確認したい場合は以下のコメントアウトを外してください
        # if any(d in ignore_dirs for d in dirs):
        #     print(f"Skipping directories: {dirs}")

        for file in files:
            # 拡張子チェック
            if not any(file.endswith(ext) for ext in config["target_extensions"]):
                continue

            file_path = os.path.join(root, file)
            rel_path = os.path.relpath(file_path, project_root)

            # 念のため、ファイル自体が除外対象ディレクトリ配下にないか再確認するロジック
            # (walkの仕様上は不要なはずですが、保険です)
            if any(part in ignore_dirs for part in rel_path.split(os.sep)):
                continue

            try:
                with open(file_path, "r", encoding="utf-8", errors="ignore") as f:
                    for i, line in enumerate(f, 1):
                        tokens = re.findall(r"\b[a-zA-Z]+\b", line)
                        for token in tokens:
                            if check_token(token, exact_list, partial_list):
                                print(
                                    f"大文字の連続:\033[32m{rel_path}\033[0m: \033[32m{i}\033[0m: \n'\033[33m{token}\033[0m'\n"
                                )
            except Exception as e:
                print(f"\033[33mError reading {rel_path}: {e}\033[0m")


if __name__ == "__main__":
    main()
