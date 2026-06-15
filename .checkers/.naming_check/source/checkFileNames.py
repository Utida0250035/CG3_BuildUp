import os
import json
import statistics
import sys


def get_base_dir():
    """実行環境に合わせてベースディレクトリを取得する"""
    if getattr(sys, "frozen", False):
        # .exe として実行されている場合
        return os.path.dirname(sys.executable)
    else:
        # 通常の .py として実行されている場合
        try:
            return os.path.dirname(os.path.abspath(__file__))
        except NameError:
            return os.getcwd()


def load_config():
    # スクリプトの場所を基準にconfig.jsonを読み込む
    config_path = os.path.join(get_base_dir(), "config.json")
    try:
        with open(config_path, "r", encoding="utf-8") as f:
            return json.load(f)
    except FileNotFoundError:
        # 設定ファイルがない場合のデフォルト値
        return {
            "exclude_dirs": [".git", "node_modules"],
            "exclude_files": [],
            "target_extensions": [".py"],
        }


def get_target_files(root_dir, config):
    targets = []
    exclude_dirs = set(config.get("exclude_dirs", []))
    exclude_files = set(config.get("exclude_files", []))
    target_exts = tuple(config.get("target_extensions", []))

    for root, dirs, files in os.walk(root_dir):
        # 除外ディレクトリの適用
        dirs[:] = [d for d in dirs if d not in exclude_dirs]

        for d in dirs:
            if d in exclude_dirs:
                continue
            dir_path = os.path.join(root, d)
            targets.append({"type": "dir", "path": dir_path, "name": d})

        for file in files:
            if file in exclude_files:
                continue
            if file.endswith(target_exts):
                targets.append(
                    {"type": "file", "path": os.path.join(root, file), "name": file}
                )

    return targets


import statistics


def analyze_naming_anomalies(targets):
    """
    統計的な異常値（外れ値）を検出する
    """
    # 名前を取得
    names = [t["name"] for t in targets]
    # 長さを取得
    lengths = [len(n) for n in names]

    # 平均と標準偏差を計算
    mean_len = statistics.mean(lengths)
    stdev_len = statistics.stdev(lengths) if len(lengths) > 1 else 0

    anomalies = []

    for t in targets:
        name = t["name"]
        length = len(name)

        # 異常スコアの計算: 平均から標準偏差の2倍以上離れているものを「珍しい」とみなす
        z_score = abs(length - mean_len) / stdev_len if stdev_len > 0 else 0

        # さらに、特殊文字（英数字以外）の割合も判定に含める
        special_chars = sum(1 for c in name if not c.isalnum() and c != ".")
        ratio_special = special_chars / length if length > 0 else 0

        # 基準：Zスコアが2より大きい（かなり長い/短い）、または特殊文字が多い
        if z_score > 2.0 or ratio_special > 0.3:
            anomalies.append(
                {
                    "path": t["path"],
                    "reason": f"統計的逸脱: 長さ={length}(平均{mean_len:.1f}), 特殊文字率={ratio_special:.2%}",
                }
            )

    return anomalies


import re
from collections import Counter
from itertools import chain


def split_into_words(name):
    """
    ファイル名を単語に分割する
    例: myProject_data-file.py -> ['my', 'project', 'data', 'file']
    """
    # 拡張子を除去
    name_without_ext = name.rsplit(".", 1)[0]
    # アンダースコア、ハイフン、スペースで分割
    parts = re.split(r"[_\-\s]+", name_without_ext)
    # さらにキャメルケース（myProject）に対応するため分割
    words = []
    for part in parts:
        # 大文字の直前で分割
        sub_words = re.findall(r"[a-zA-Z][^A-Z]*", part)
        words.extend([w.lower() for w in sub_words])
    return words


def detect_rare_words(targets):
    """
    使用頻度の低い単語を含むファイルを検知する
    """
    # 全ファイルの全単語を収集
    all_words = list(chain.from_iterable(split_into_words(t["name"]) for t in targets))
    word_counts = Counter(all_words)

    # 頻度が高いものを「一般的な単語」とする（例：全体の0.5%以上登場するものなど）
    # ここでは単純に「1回しか登場しない単語」を希少と定義
    rare_words = {word for word, count in word_counts.items() if count == 1}

    anomaliesRare = []
    for t in targets:
        words = split_into_words(t["name"])
        # 希少な単語が含まれていたらリストアップ
        found_rare = [w for w in words if w in rare_words]
        if found_rare:
            anomaliesRare.append({"path": t["path"], "rare_words": found_rare})

    return anomaliesRare


def main():
    config = load_config()

    base_dir = get_base_dir()

    # プロジェクトルート（スクリプトの1つ上の階層）を対象にする
    project_root = os.path.abspath(os.path.join(base_dir, "..\.."))
    print(f"DEBUG: base_dir: {base_dir}")
    print(f"DEBUG: project_root: {project_root}")

    targets = get_target_files(project_root, config)

    print(f"--- チェック対象ファイル数: {len(targets)} ---")
    for target in targets:
        # ここに命名チェックのロジックを入れる
        print(
            f"チェック対象 ({target['type']}): {target['name']} (パス: {target['path']})"
        )

    # 異常値を分析
    anomalies = analyze_naming_anomalies(targets)
    print(f"\n--- 名前が長いか特殊文字を多く含むファイルの数: {len(anomalies)} ---")
    for anomaly in anomalies:
        print(f"長文や特殊文字: {anomaly['path']} - {anomaly['reason']}")

    # 希少単語を検出
    anomaliesRare = detect_rare_words(targets)
    print(f"\n--- 希少単語を含むファイル数: {len(anomaliesRare)} ---")
    for anomaly in anomaliesRare:
        print(f"希少単語が含まれている): {anomaly['path']} - {anomaly['rare_words']}")

    input("\nEnterキーを押して終了...")


if __name__ == "__main__":
    main()
