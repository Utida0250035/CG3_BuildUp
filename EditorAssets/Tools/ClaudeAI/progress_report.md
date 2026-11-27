# 半自律稼働 progress report

**最新サマリー:** 2026-11-22 — atomF で Space/A 射撃と自機弾スポーンを実装。次は自機の当たり半径b。

**記録の分け方:** ここから上の本文は、2026-11-08 から 2026-11-22 13:21 までの要約である。個別 tick の本文は残していない。ギャップの明細は `docs/implementation_gaps.md` と `docs/implementation_gaps_index.json`、完了チェックは `docs/feature-memos/roadmap.md` が正本。これ以降に足すセッションは、末尾へ `cursorInstructions.md` §3 の形式で追記する。

## いまの状態（2026-11-22）

- atomF の下書きは `roadmap.md` の「全方位シューティングゲーム「atomF」」へ起こしてある。自機弾の「高速」は速さb（1.5m/s）の言い換えとして行を消した。
- 自機と敵の数値は `Project/Assets/atomF/parameters.json` の `player` / `enemy`。ゲーム本体はまだ読んでいない。ファイル順で atomF の次の未完了は「ランタイムが parameters.json を読み、自機と敵の数値をそこから取る」。
- 2026-11-22 10:30 に Development | x64 のビルドを通し、`CG2_BuildUp.exe` の生成まで到達した。ランタイム起動の確認は未実施。
- 同日 13:21、`progress_report.md` が約 406 kB で上限 200 kB を超え、roadmap ループは起動直後に停止した。要約後のサイズは上限未満。
- エディタと UX の案内まわりに、未完了の `- [ ]` が残っている。ファイル順の先頭は Editor の「ヒット球は選択 1 球のみ編集」。

## 2026-11-08 エンジン機能の到達点

半自律ループ（1 分、20 分、15 分、Cloud）で roadmap の大枠を実装した。同日、未完了の `- [ ]` が一度尽きたためループは止まった。

- **Audio:** シーン BGM のストリーミング、一時停止と再開、再生位置、ピッチと音量の API。
- **System:** プレハブ JSON の入出力、コンポーネントの追加と実行順、アセットのドロップ取り込みと png プレビュー。
- **Camera:** オデッセイ型ステージカメラの方針とイージング、プレイヤーにカメラ操作を丸投げしない方針。
- **Scene:** ムービーカメラと `SceneMovie`。
- **Render / Lighting:** ディザ透過、地形越しシルエット、リムライト、ボリュームフォグ、ゴッドレイ、接触シェードと平面影。
- **Motion:** モーションブレンドとクロスフェード。
- **Debug:** テストプレイエリア、ストリーミングのメモリと進捗の ImGui。
- **Loader:** `.amesh` 変換と DDS 変換、vcxproj の登録漏れ検査。
- **Editor:** コードテンプレートの生成 UI。
- **UX:** 案内吹き出し、ナビキャラ、チュートリアル置換。

同じ期間に、ビルド 3 回失敗でロールバックして別タスクへ移る運用と、develop へのマージ手順を足した。Unity 風エンティティとコンポーネント分割は方針メモまで。

## ギャップスキャン（2026-11-09 〜 2026-11-18）

- 2026-11-09 にギャップ在庫（`implementation_gaps.md` / `implementation_gaps_index.json`）と「ギャップスキャンを実行して」の手順を作った。
- 2026-11-09 夜から 2026-11-11 朝まで、約 90 tick。Play / Authoring / Debug / Ship のシナリオを繰り返し、遷移 SE、BGM、カメラ、シーン起動、エディタ操作、案内、配布、デバッグシーンなどの不足を ID 化した。同じシナリオの再訪が多い。
- 2026-11-11 にユーザー指示でギャップスキャンのループを停止。未解消 289 件を roadmap に反映した。
- 2026-11-18 朝に、完成形の説明の `・` 行を対象にしたスキャンを 7 tick（外部エディタ、物理の独立性、描画への入り口、クラス分割など）。続けて未掲載分を roadmap 下書きへ足し、正本へマージした（追加 256 件、既存一致 42 件）。ギャップを下書きへ流すループはその日の昼に手動停止。

スキャン 1 件ごとの newIds と scenarioId は、この要約には残していない。在庫ファイルが正本である。

## ロードマップ実行（2026-11-11、2026-11-18 〜 2026-11-22）

15 分間隔の tick で、roadmap の `- [ ]` を 1 件ずつ完了にしていった。2026-11-19 朝は、進行中だった「SetupStreaming が 44.1kHz 固定」の未コミット変更を破棄してループを止めた。その後の tick で音声、シーン、デバッグ、エディタを続け、当該行を含む多くの項目は roadmap 上で完了になっている。

- **Audio:** 遷移 SE、フェードと BGM のタイミング、Pause / Resume / Volume、ストリーミングの再開とサンプルレート、SE の個別停止とキャッシュ。
- **Scene:** ムービーの起動と失敗時の復帰、フェード中の Update、カメラリグ、Title / Game / Result / BuildUp の BGM とライティング、シーン JSON のパス解決、読込失敗の通知。
- **Camera / Debug:** ステージ範囲への追従、tuning JSON、機能検証シーンの起動、フレーム時間表示、ストリーミング診断のログ。
- **Editor:** シーン実行を BuildUp 経路へ接続、プレビューと本番の差分、インスペクタ（複製、タグ、実行順、外部ツール、カメラ、プレハブの一括配置と上書き、Imported の一覧と改名）。
- **運用:** ループ同士の占有宣言（`loopLease.ps1`）。Cloud 上の tick では Windows の `buildDevelopment.cmd` を実行できない回があった。

## 2026-11-13 の個別作業

- Release のシーン巡回を ParticleDemo と BillboardInstancing100 にし、次シーン案内を表示。
- 複数エミッタと複数レイヤーのパーティクル描画を修正し、衝撃の多層サンプルを追加。
- Release が読む JSON を `Project/Assets` へ移した。

## 保留のまま（roadmap の `- [-]`）

純粋 ECS、FNV-1a の型 registry、Collider の AABB と Update 順、factory コンポーネント、エンティティ構成のアセット化の重複、別スレッド読み込み、名前空間の一括整理、ステージギミックの一本化エディタ、ビヘイビアツリー、タイルマップ、Lua のランタイム読込と動的書き換え。再開条件は roadmap 各行と、当時の記録では設計確定または前提機能の完了。

---

以降のセッションは、この要約を消さず、末尾に `## セッション yyyy-mm-dd HH:MM` を追記する。各セッションの項目は `cursorInstructions.md` §3 の 1〜8 のままとする。

---

## セッション 2026-11-22 14:46

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFFloorXzY0`
- **roadmap:** atomF › 戦闘エリア — XZ平面かつ Y座標 0 の床
- `CmpAtomFFloor` がローカル Y=0 の XZ 平面を描く。`Assets/Scenes/atomF.json` を SceneBuildUp で開いたときだけエンティティを足す。
- 既存の本編床（`kGroundHeight = -4`）は変えていない。
- **ビルド:** `buildDevelopment.cmd` は cp932 が UTF-8 の rem 行を誤読し、nuget.exe 不在で止まった。vcxproj 検査のあと MSBuild Development|x64 は成功（exit 0）。ランタイムでの見た目確認は未実施。

### 2. 発生した課題または設計上の選択肢

- 床の一辺は 64m（中心から 32m）の四角。半径 16 の円は次の行。
- 面の表裏を両方描き、上から見ても下から見ても床が見える。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「半径 16.0f の円形の床」。ブランチは `feature/AtomFFloorCircle16`。
- 指示待ちではなく、15 分ループの次 tick で続ける。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **XZ平面かつ Y座標 0 の床**
  - 用途: 全方位シューティングの戦闘が乗る地面
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` の SceneBuildUp）
  - 用途から決めた仕様: 既存の横視点ステージ（地面 Y=-4）には足さない。円の半径はまだ切らない

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| XZ平面かつ Y座標 0 の床 | 有効 | シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22 14:55

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFFloorCircle16`
- **roadmap:** atomF › 戦闘エリア — 半径 16.0f の円形の床
- `CmpAtomFFloor` の描画を、一辺 64m の四角から半径 16m の円盤（48 分割、Y=0）に変えた。表裏の両方を描く。
- 出現条件は前回のまま。`Assets/Scenes/atomF.json` を SceneBuildUp で開いたときだけ床エンティティを足す。
- **ビルド:** `buildDevelopment.cmd` は cp932 が UTF-8 の rem 行を誤読し、nuget.exe 不在で止まった。続けて MSBuild Development|x64 は成功（exit 0、`CmpAtomFFloor.cpp` をコンパイル）。ランタイムでの見た目確認は未実施。

### 2. 発生した課題または設計上の選択肢

- 下書きの 16.0f に単位は無い。他の長さがメートルなので、半径 16m とした。
- 分割数は 48。見た目の円と、三角形を 1 枚ずつ出すコストの中間。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「半径 16.0f の円形の壁あるいは柵に囲まれている（見た目で分かる）」。次ブランチは `feature/AtomFArenaWallRing`。
- 指示待ちではなく、15 分ループの次 tick で続ける。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **半径 16.0f の円形の床**
  - 用途: 全方位シューティングの戦闘が乗る地面の外形
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` の SceneBuildUp）
  - 用途から決めた仕様: 既存の横視点ステージには足さない。壁や出入りの制限はまだ付けない。半径は 16m

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 半径 16.0f の円形の床 | 有効 | シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22 15:10

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFArenaWallRing`
- **roadmap:** atomF › 戦闘エリア — 半径 16.0f の円形の壁あるいは柵に囲まれている（見た目で分かる）
- `CmpAtomFArenaWall` を追加した。内径 16m、厚さ 0.4m、高さ 1.5m の円環を、床の縁の外側に描く。上面と内外の側面を表裏とも描く。
- `Assets/Scenes/atomF.json` を SceneBuildUp で開いたときだけ、床に続けて壁エンティティを足す。当たり判定は付けていない。
- **ビルド:** `buildDevelopment.cmd` は cp932 が UTF-8 の rem 行を誤読し、nuget.exe 不在で止まった。続けて MSBuild Development|x64 は成功（exit 0、`CmpAtomFArenaWall.cpp` をコンパイル）。ランタイムでの見た目確認は未実施。

### 2. 発生した課題または設計上の選択肢

- 下書きは「壁あるいは柵」で高さの指定が無い。見下ろしでも輪郭が残るよう、上面の幅 0.4m・高さ 1.5m の低い壁にした。柵の柱にはしていない。
- 半径 16 は床側の定数と壁側の定数で別々に持っている。揃える共通定義はまだ無い。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「自機、自機弾、敵はこのアリーナから出られない」。次ブランチは `feature/AtomFArenaContainment`。
- 指示待ちではなく、15 分ループの次 tick で続ける。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **半径 16.0f の円形の壁あるいは柵**
  - 用途: 全方位シューティングの戦闘範囲を、プレイ中に目で分かる境界にする
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` の SceneBuildUp）
  - 用途から決めた仕様: 既存の横視点ステージには足さない。内径は床と同じ 16m。出入りの制限はまだ付けない

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 半径 16.0f の円形の壁あるいは柵に囲まれている（見た目で分かる） | 有効 | シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22 15:22

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFArenaContainment`
- **roadmap:** atomF › 戦闘エリア — 自機、自機弾、敵はこのアリーナから出られない
- `CmpAtomFArenaContainment` と `AtomFArenaContainmentAttach` を追加。当たり半径の余白を抜いた円（内径 16m）の内側に座標を押し戻す。
- `SceneBuildUpAtomFArenaContainmentTick` が atomF シーンのみ、更新後に tagRegistry の player / enemy / 管理弾 / PLAYER_WEAPON 役のエンティティを押し戻す（コンテンツ未実装のため現時点では対象無し）。
- 横視点本編の StageBounds には触れていない。
- **ビルド:** `buildDevelopment.cmd` は前回同様停止。MSBuild Development|x64 は成功（exit 0）。ランタイム確認は未実施。

### 2. 発生した課題または設計上の選択肢

- 速度の内向き成分の打ち消しはまだ無い。毎フレーム座標を丸める方式。次の自機／弾移動実装時にエッジで跳ねる可能性あり。
- 敵の押し出し余白は敵当たり未実装のため半径b（0.25m）に合わせた。parameters.json はまだランタイム未読込。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「自機の斜め上から見下ろし、自機に追従する」。次ブランチは `feature/AtomFCombatCamera`。
- 自機スポーン時は `AttachArenaContainmentForPlayer` 等を呼ぶ想定。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **アリーナから出られない制限**
  - 用途: 全方位シューティングの戦闘が円形アリーナの外へ逸れないようにする
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` の SceneBuildUp）
  - 用途から決めた仕様: atomF 以外のシーンでは実行しない。壁は見た目のみのまま。数値余白は parameters.json の半径a/b と一致する定数

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 自機、自機弾、敵はこのアリーナから出られない | 有効 | プリミティブ(直方体や球、円柱やカプセル、面や線)同士の当たり判定が実装してある |

---

## セッション 2026-11-22 15:35

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFCombatCamera`
- **roadmap:** atomF › 戦闘カメラ — 自機の斜め上から見下ろし、自機に追従する
- `SceneBuildUpAtomFCombatCamera` を追加。atomF シーンのみ、tagRegistry の player 役のワールド座標を注視点に、斜め上（高さ 11m・横 −5m・Z −7m）から見下ろす。
- 自機が無い間はアリーナ中心 (0, 0.5, 0) を注視。フレーミングは半径 16m の床向け。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。ランタイム確認は未実施。

### 2. 発生した課題または設計上の選択肢

- 横視点 StageView 用の `ApplyStageViewCameraPose` を流用。FOV は本編と同じ。
- イージングや壁際の押し戻しはまだ無い。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「ランタイムが parameters.json を読み、自機と敵の数値をそこから取る」。次ブランチは `feature/AtomFParametersRuntime`。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **戦闘カメラ（斜め上追従）**
  - 用途: 全方位シューティングをプレイヤーが直接カメラ操作せず追える
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` の SceneBuildUp）
  - 用途から決めた仕様: atomF 以外は従来の StageView を維持。自機未実装時もシーンは中央を見る

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 自機の斜め上から見下ろし、自機に追従する | 有効 | マリオオデッセイのように、プレイヤーがカメラを直接操作せずとも自動でカメラが動作し、見やすいカメラワークをとるようになっている |

---

## セッション 2026-11-22 16:00

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFParametersRuntime`
- **roadmap:** atomF › パラメータ — ランタイムが parameters.json を読み、自機と敵の数値をそこから取る
- `AtomFParametersRuntime` が `Assets/atomF/parameters.json` の player / enemy をパースして保持。
- `SceneBuildUp` 入室時に `SyncAtomFParametersWithScenePath` で atomF のみ読込・他シーンでは `Unload`。
- アリーナ押し戻しの自機・弾半径は `AtomFParametersAccess` 経由で JSON 値を使う（敵本体半径は JSON 未定のため従来定数）。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。ランタイム確認は未実施。

### 2. 発生した課題または設計上の選択肢

- 移動速度・弾速・クールタイム等は読み込むが、自機／敵コンポーネントがまだ無く消費側は containment 半径のみ。
- 読込失敗時は `IsLoaded()` false のまま。フォールバック定数で動く。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「W、A、S、D か L スティックで XZ 平面上を速さaで移動」。次ブランチは `feature/AtomFPlayerMove`。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **parameters.json のランタイム読込**
  - 用途: atomF の自機・敵のバランスをデータ駆動にする
  - 使用場面: SceneBuildUp で atomF シーンに入ったとき
  - 用途から決めた仕様: 本編シーンでは読まない。配布パスは `GameAssets::kAtomFParametersJson`

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| ランタイムが parameters.json を読み、自機と敵の数値をそこから取る | 有効 | 各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-22 16:12

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerMove`
- **roadmap:** atomF › 自機 — W、A、S、D か L スティックで XZ 平面上を速さaで移動
- `CmpAtomFPlayerMove` と `AtomFPlayerMoveInput` を追加。WASD と左スティックを XZ 入力に統合。
- `SceneBuildUpAtomFPlayerSpawn` が atomF で `AtomFPlayer`（PLAYER タグ・球表示・アリーナ containment）をスポーン。
- 移動速度は `ParametersRuntime` の `moveSpeedMetersPerSecond`（速さa）。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。ランタイム確認は未実施。

### 2. 発生した課題または設計上の選択肢

- Y 座標は固定。床 Y=0 の上に当たり半径分だけ離して配置。
- 本編の `CmpInput` / `CmpMoveByInput2D` とは別系統で、横視点操作と混ざらない。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「射撃は parameters.json のクールタイム 1 秒を空ける」。次ブランチは `feature/AtomFPlayerShotCooldown`。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **自機 XZ 移動**
  - 用途: 全方位シューティングの基本操作（キーとパッド両対応）
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json`）
  - 用途から決めた仕様: atomF 専用コンポーネント。カメラ追従と連動する PLAYER タグ付き

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| W、A、S、D か L スティックで XZ 平面上を速さaで移動 | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22 16:20

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerShotCooldown`
- **roadmap:** atomF › 自機 — 射撃は parameters.json のクールタイム 1 秒を空ける
- `CmpAtomFPlayerShotCooldown` を追加。`TryConsumeShotCooldown()` で発射許可時に `shotCooldownSeconds` を開始。
- `AtomFPlayer` にコンポーネントを付与。弾の生成と発射入力は次タスク。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。ランタイム確認は未実施。

### 2. 発生した課題または設計上の選択肢

- クールタイムの消費は発射処理が `TryConsumeShotCooldown` を呼ぶ想定。現状は入力未接続。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「マウスか R スティックで XZ 平面上のスカラー角度でエイム」。次ブランチは `feature/AtomFPlayerAim`。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **射撃クールタイム**
  - 用途: 全方位シューティングの連射制限をデータ駆動にする
  - 使用場面: ランタイム（自機の発射処理）
  - 用途から決めた仕様: JSON 未読時は 1 秒。発射成功時のみタイマー開始

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 射撃は parameters.json のクールタイム 1 秒を空ける | 有効 | 各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-22 16:28

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerAim`
- **roadmap:** atomF › 自機 — マウスか R スティックで XZ 平面上のスカラー角度でエイム
- `CmpAtomFPlayerAim` を追加。R スティック優先、無入力時はマウスで Y=自機高さの水平面をピックして角度を更新。
- `XInputController::GetRightStick` を追加。
- 自機のヨー回転にエイム角度を反映（視認用）。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。ランタイム確認は未実施。

### 2. 発生した課題または設計上の選択肢

- マウスピックはエディタと同じ `TryPickHorizontalPlaneY` を共用。エディタ UI 上の座標は本番ウィンドウ全体のカーソル位置。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「Space かパッド A でエイム方向に自機弾」。次ブランチは `feature/AtomFPlayerFire`。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **XZ エイム**
  - 用途: 全方位シューティングの照準（マウスとパッド両対応）
  - 使用場面: ランタイム（自機の INPUT 更新）
  - 用途から決めた仕様: 角度は `atan2(x, z)`。発射は次タスクで `GetAimDirectionXZ` を参照

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| マウスか R スティックで XZ 平面上のスカラー角度でエイム | 有効 | エディタ上での入力マッピング(ボタンとキーのエイリアス)を設定する（ランタイム入力の拡張） |

---

## セッション 2026-11-22 16:38

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerFire`
- **roadmap:** atomF › 自機 — Space かパッド A でエイム方向に自機弾
- `CmpAtomFPlayerFire` が Space / パッド A のトリガーと `TryConsumeShotCooldown` で発射。
- `AtomFPlayerBulletSpawn` が PLAYER_WEAPON タグ付き弾（`CmpAtomFPlayerBulletMove`・parameters の速さb）を自機の子として生成。
- 自機に子エンティティ保持用コンポーネント（`CmpChildEntitys`）を付与。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。ランタイム確認は未実施。

### 2. 発生した課題または設計上の選択肢

- 弾は子エンティティとして保持。当たり・軌道エフェクトは roadmap の自機弾節が未実装。
- 発射位置は自機ワールド座標（オフセット無し）。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「当たり判定の大きさは半径b」。次ブランチは `feature/AtomFPlayerHitRadius`。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **自機弾の発射**
  - 用途: 全方位シューティングの主攻撃入力
  - 使用場面: ランタイム（自機）
  - 用途から決めた仕様: クールタイムとエイムを既存コンポーネントに委譲

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| Space かパッド A でエイム方向に自機弾を放てる | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・発射マージ完了＋当たり半径b）

### 1. 現在の進捗状況と完了したこと

- **前 tick フォロー:** `exclusion.dic` に `Entitys` を追加し、`loopMergeTickToDevelop.ps1` で発射実装を develop へマージ（`b6e3e1e`）。作業ブランチ `feature/AtomFPlayerFire` 削除、`feature/AtomFPlayerHitRadius` 作成済み。
- **本 tick:** atomF › 自機 — 当たり判定の大きさは半径b。
- `AtomFPlayerHitAttach` が `ResolvePlayerBodyRadiusMeters`（parameters の半径b）で `CmpHitSphere` を付与。
- `SceneBuildUpAtomFPlayerSpawn` から自機スポーン時に呼び出し。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。

### 2. 発生した課題または設計上の選択肢

- 見た目（`CmpMeshSphere`）と当たり（`CmpHitSphere`）は同じ半径源を共有。形状「球」は HitJudge の球当たりとして暗黙。
- 敵とのヒット応答は未実装（自機弾節・敵節の `- [ ]` が残る）。

### 3. 次に実行すべきステップの提案

- 自機弾節の先頭 `- [ ]`「速さbで発射方向に直進する」（実装済みなら §6.2 で `- [x]` のみ）。次ブランチ候補: `feature/AtomFPlayerBulletStraight`.

## 推測した用途（§6.4）

- **自機の球当たり（半径b）**
  - 用途: 全方位シューティングでの自機・弾・敵の衝突判定の基準半径
  - 使用場面: ランタイム（`HitJudge` 登録）
  - 用途から決めた仕様: parameters.json の `hitRadiusMeters` を単一の真値とし、アリーナ containment の body 半径と揃える

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 当たり判定の大きさは半径b | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・自機弾 速さb 直進）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerBulletStraight`
- **roadmap:** atomF › 自機弾 — 速さb で発射方向に直進する
- 発射 tick 時点で `CmpAtomFPlayerBulletMove` が parameters の速さb 沿い方向へ毎フレーム移動済みのため §6.2 を主とし、`ResolvePlayerBulletSpeedMetersPerSecond` で参照を一元化。
- 前 tick 残り: `progress_report` の査定表文字化けを修正。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。

### 2. 発生した課題または設計上の選択肢

- 敵当たり・弾の半径a当たり・軌道エフェクトは未着手。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]`「敵に当たると弾が消え、弾の元の進行方向に敵が吹き飛ぶ」。次ブランチ `feature/AtomFPlayerBulletEnemyHit`。

## 実装済みと判断した項目（コード確認・即時 [x]）

- 速さb で発射方向に直進する（前 tick の `CmpAtomFPlayerBulletMove`・今 tick で parameters アクセサ整理）

## 推測した用途（§6.4）

- **自機弾直進（速さb）**
  - 用途: 攻撃弾の見た目とアリーナ containment を伴う基本移動
  - 使用場面: ランタイム（毎フレーム Update）
  - 用途から決めた仕様: 発射時に固定した XZ 単位方向へ parameters の bullet 速さのみ適用

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 速さb で発射方向に直進する | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・自機弾が敵を吹き飛ばす）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerBulletEnemyHit`
- **roadmap:** atomF › 自機弾 — 敵に当たると弾が消え、進行方向に敵を吹き飛ばす
- `CmpAtomFPlayerBulletEnemyHit` が `CmpHitSphere` のヒットを見て敵役に `CmpAtomFEnemyKnockback`（parameters 吹き飛び速さb）を付与し、弾は親から切り離して Finalize。
- 当たり未実装のため `AtomFPlayerBulletHitAttach`（半径a）を弾スポーンに追加。敵側は `AtomFEnemyHitAttach` を用意（敵スポーン roadmap 待ち）。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。敵未スポーンのためランタイム目視は未実施。

### 2. 発生した課題または設計上の選択肢

- atomF シーンに敵エンティティが無いため、ヒット応答はコード上のみ成立。敵出現タスクと合わせて `AttachEnemyHitSphere` が必要。
- 弾の当たり形状・半径a の roadmap 行は HitJudge 球で実装済みだが checklist 行は別途 `- [ ]` のまま。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]`「軌道エフェクトが 0.5 秒残る」。次ブランチ `feature/AtomFPlayerBulletTrail`。

## 推測した用途（§6.4）

- **自機弾の敵ヒット応答**
  - 用途: 全方位シューティングの攻撃命中とノックバック
  - 使用場面: ランタイム（HIT_RESPONSE 更新）
  - 用途から決めた仕様: 1 発 1 敵で弾消滅、弾進行方向 XZ へ enemy 速さb

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 敵に当たると弾が消え吹き飛ばす | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・自機弾軌道 0.5 秒）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerBulletTrail`
- **roadmap:** atomF › 自機弾 — 軌道エフェクトが 0.5 秒残る
- `CmpAtomFPlayerBulletTrail` が弾位置をサンプルし、parameters の `trailSeconds` で古い点を削除。
- `CmpDrawAtomFPlayerBulletTrail` が `DrawLine3D` でフェードする軌道を描画。
- 敵ヒット時は `BeginLingerPhase` で弾メッシュを消し、軌道だけ残してから親切り離しと `Finalize`。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。

### 2. 発生した課題または設計上の選択肢

- 軌道は線分の連続描画。パーティクル JSON は未使用（軽量優先）。
- 弾が画面外で消える経路では軌道 linger 未対応（containment のみ）。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]`「当たり判定の形状は球」。次ブランチ `feature/AtomFPlayerBulletHitShape`。

## 推測した用途（§6.4）

- **自機弾軌道表示**
  - 用途: 弾道の視認性向上（全方位シューティング）
  - 使用場面: ランタイム描画
  - 用途から決めた仕様: parameters 連動、ヒット後も trailSeconds 分描画

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 軌道エフェクトが 0.5 秒残る | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・自機弾当たり形状は球）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerBulletHitShape`
- **roadmap:** atomF › 自機弾 — 当たり判定の形状は球
- 敵ヒット tick で `CmpHitSphere` 済み。今 tick で parameters の `bullet.hitShape` をランタイム読込し、`IsPlayerBulletHitShapeSphere` で球のときだけ `AttachPlayerBulletHitSphere`。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。

### 2. 発生した課題または設計上の選択肢

- 球以外の形状は未対応（付与スキップ）。AABB 等は別タスク想定。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]`「当たり判定の大きさは半径a」（§6.2 候補: 既に `ResolvePlayerBulletRadiusMeters`）。次ブランチ `feature/AtomFPlayerBulletHitRadiusA`。

## 実装済みと判断した項目（コード確認・即時 [x]）

- 当たり判定の形状は球（`CmpHitSphere` + hitShape 連携で明示化）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 当たり判定の形状は球 | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・自機弾当たり半径a）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFPlayerBulletHitRadiusA`
- **roadmap:** atomF › 自機弾 — 当たり判定の大きさは半径a
- §6.2 主。`ResolvePlayerBulletRadiusMeters` が parameters 半径a を既に参照。`SyncPlayerBulletHitRadiusA` でメッシュと `CmpHitSphere` を同一半径に統一。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。

### 2. 次に実行すべきステップの提案

- **atomF › 自機弾** は完了。次は **自機エイムのカプセル当たり判定**「エイム中にカプセルが伸びる」。次ブランチ `feature/AtomFAimCapsuleStretch`。

## 実装済みと判断した項目（コード確認・即時 [x]）

- 当たり判定の大きさは半径a（既存 parameters 連携の整理）

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠（完成形の説明の・行） |
|--------|------|------------------------------|
| 当たり判定の大きさは半径a | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・エイムカプセル伸長）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/AtomFAimCapsuleStretch`
- **roadmap:** atomF › 自機エイムのカプセル当たり判定 — エイム中にカプセルが伸びる
- `CmpAtomFPlayerAim::IsAimInputActive` を追加。
- `CmpAtomFPlayerAimCapsule` がエイム入力中に長さを伸ばし、入力無しで縮める（上限はアリーナ直径）。
- `CmpDrawAtomFPlayerAimCapsule` で軸線を表示。
- **ビルド:** MSBuild Development|x64 成功（exit 0）。

### 2. 発生した課題または設計上の選択肢

- 当たり判定（半径b・壁敵での終端）は次の roadmap 行。現状は長さと描画のみ。

### 3. 次に実行すべきステップの提案

- 次 `- [ ]`「カプセルの半径は半径b」。次ブランチ `feature/AtomFAimCapsuleRadiusB`。

## 推測した用途（§6.4）

- **エイムカプセル伸長**
  - 用途: 敵の「狙われ」判定の可視化と当たり用セグメント長の基礎
  - 使用場面: ランタイム（マウス／R スティックエイム中）
  - 用途から決めた仕様: 伸縮速度は固定定数、アリーナ直径まで伸長

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| エイム中にカプセルが伸びる | 有効 | プリミティブ同士の当たり判定が実装してある（カプセル当たりの前段） |

---

## セッション 2026-11-22（ループ tick・エイムカプセル半径b）

### 1. 完了

- **ブランチ:** `feature/AtomFAimCapsuleRadiusB`
- `ResolvePlayerAimCapsuleRadiusMeters`（parameters 半径b）を `CmpAtomFPlayerAimCapsule` と描画幅に反映。
- `CmpAtomFPlayerAimCapsuleHitSync` がエイム軸上に半径bの球列を `CmpHitSphere` に合成（本体球＋カプセル近似）。
- **ビルド:** Development|x64 成功。

### 2. 次

- 「敵や壁に当たると衝突点が終端になる」→ `feature/AtomFAimCapsuleWallClamp`

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| カプセルの半径は半径b | 有効 | プリミティブ同士の当たり判定が実装してある |

---

## セッション 2026-11-22（ループ tick・エイムカプセル壁敵クランプ）

### 1. 完了

- **ブランチ:** `feature/AtomFAimCapsuleWallClamp`
- `AtomFPlayerAimCapsuleRayClamp` が XZ 円形アリーナ壁と敵 `CmpHitSphere` へのレイ交差で長さ上限を算出。
- `CmpAtomFPlayerAimCapsule` の伸長・縮小後に上限を適用。`HitJudge::RegisteredHitSpheres` で敵走査。
- **atomF › 自機エイムのカプセル当たり判定** 節の `- [ ]` はすべて完了。
- **ビルド:** Development|x64 成功。

### 2. 次

- **敵** 節「自機に向かって速さaで歩いてくる」→ `feature/AtomFEnemyWalkToPlayer`

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵や壁で衝突点が終端 | 有効 | プリミティブ同士の当たり判定が実装してある |

---

## セッション 2026-11-22（ループ tick・敵が自機へ速さaで歩行）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyWalkToPlayer`
- `AppendAtomFEnemyIfScene` で赤い敵を 1 体スポーン（XZ 8,0）。`CmpAtomFEnemyWalkToPlayer`・`ResolveEnemyMoveSpeedMetersPerSecond`（速さa）。
- `AtomFScenePlayerLocator` で自機参照。吹き飛び速度が有効な間は歩行停止。
- **ビルド:** Development|x64 成功。

### 2. 課題

- `CmpAtomFEnemyKnockback` に減衰が無く、弾ヒット後も速度が残り続ける可能性（今後の調整候補）。

### 3. 次

- 「5秒ごとに震えて突進」→ `feature/AtomFEnemyChargeInterval`

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵が自機へ速さaで歩く | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・敵 5 秒震え突進）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyChargeInterval`
- `CmpAtomFEnemyPeriodicCharge` が `chargeIntervalSeconds`（5 秒）ごとに震え（約 0.45 秒）の後、速さb で短時間突進。
- 歩行は震え／突進中は `BlocksLocomotion` で停止。
- 併せて BuildUp RT カメラ FOV 合わせと未使用引数警告を修正（ImGui 非依存ビルド）。
- **ビルド:** Development|x64 成功。

### 2. 次

- 「自機に狙われると立ち止まり焦りモーション」→ `feature/AtomFEnemyAimTargetedPanic`

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 5 秒ごとに震えて突進 | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・敵の狙われ焦り）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyAimTargetedPanic`
- `AtomFEnemyAimCapsuleTargetQuery` でエイム中カプセルと敵の距離判定。
- `CmpAtomFEnemyTargetedPanic` で歩行・突進を止め、位置を震わせる（汗パーティクルは焦りモーション節）。
- **ビルド:** Development|x64 成功。

### 2. 次

- 「自機に当たるとダメージ」→ `feature/AtomFEnemyDamagePlayer`

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 狙われて立ち止まり焦り | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-22（ループ tick・敵接触ダメージ）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyDamagePlayer`
- `AtomFPlayerHealthAttach` で自機に `CmpHealthHit`（Hurter＝敵役）。球当たりと `HitJudge` 経由で接触ダメージ。
- 無敵時間は parameters の `damageFeedback.vignetteSeconds`（0.75秒）。既存のビネット・SE・カメラシェイクを利用。
- **ビルド:** Development|x64 成功。

### 2. 次

- 「吹き飛び後に壁や敵で爆発」→ `feature/AtomFEnemyChainExplosion`

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵接触で自機ダメージ | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## 2026-11-23 ループ手動停止

ユーザー指示によりループを停止しました。占有（lease）を解放済みです。

---

## セッション 2026-11-23（ループ tick・敵の連鎖爆発と壁・敵衝突消滅）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyChainExplosion`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 敵 — 自機弾や別の敵の爆発に吹き飛ばされたあと、壁や別の敵に当たると爆発し、パーティクルを散らして小さくなって消滅する
- `CmpAtomFEnemyChainExplosion` を整え、ノックバック中のアリーナ壁接触（`AtomFEnemyArenaWallContact`）または別敵への接触で爆発パーティクル再生・周囲敵への吹き飛び伝播（`AtomFEnemyExplosionKnockbackSpread`）・メッシュ縮小フェード・`Finalize()` + `RequestDestroy()` による消滅（`PurgeDestroyedSceneEntities`）まで通る状態に仕上げた。
- 爆発縮小中（`IsDying()`）に歩行・突進・焦りコンポーネントが再始動しないようガードを追加し、`HasUpdCmp` / `HasDrawCmp` チェックと `ResolveDependencies()` 呼び出しを補完した。
- 併せて `Entity.h` の `GetUpdCmp()` const / 非 const オーバーロード不整合を修復し、`buildDevelopment.cmd` / `restoreDevelopmentNuGetPackages.cmd` の cp932 文字化けおよび NuGet CLI 不在時の既存パッケージ検出を修正した。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）成功（exit 0）、`Debug|x64` 成功（exit 0）。`Release|x64` は既存課題（Editor 除外に伴う 21 件の未解決外部シンボル）のみで新規エラーなし。

### 2. 次

- 「自機弾や別の敵の爆発に吹き飛ばされる速度は速さb」→ `feature/AtomFEnemyKnockbackSpeedB`

## 推測した用途（§6.4）

- **敵の連鎖爆発と消滅**
  - 用途: 全方位シューティングにおける敵の吹き飛ばし連鎖撃破と戦闘の爽快感・負荷管理
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` の `SceneBuildUp` 戦闘中）
  - 用途から決めた仕様: 爆発後は当たり判定を無効化して 0.6 秒かけて縮小フェードし、終了時に `PurgeDestroyedSceneEntities` でシーンから安全に破棄する

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 吹き飛ばし後の壁・敵衝突爆発と縮小消滅 | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-23（ループ tick・敵吹き飛び速度 速さb の確認）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyKnockbackSpeedB`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 敵 — 自機弾や別の敵の爆発に吹き飛ばされる速度は速さb
- §6.2 コード確認により、自機弾ヒット（`CmpAtomFPlayerBulletEnemyHit`）と別敵爆発の伝播（`AtomFEnemyExplosionKnockbackSpread`）の双方が `AtomF::ResolveEnemyKnockbackSpeedMetersPerSecond()` を参照し、`Project/Assets/atomF/parameters.json` の `enemy.knockbackSpeedMetersPerSecond`（`1.5` m/s = 速さb）で `CmpAtomFEnemyKnockback` を駆動していることを確認した。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）成功（exit 0）。

### 2. 次

- 「敵の爆発 › この爆発に当たると敵が爆心地と反対方向に吹き飛ぶ」→ `feature/AtomFEnemyExplosionKnockbackDirection`

## 実装済みと判断した項目（コード確認・即時 [x]）

- 自機弾や別の敵の爆発に吹き飛ばされる速度は速さb（`ResolveEnemyKnockbackSpeedMetersPerSecond`・`CmpAtomFPlayerBulletEnemyHit`・`AtomFEnemyExplosionKnockbackSpread`・`CmpAtomFEnemyKnockback`）

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵の吹き飛び速度を速さbとする | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-23（ループ tick・敵爆発の反対方向吹き飛び確認）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyExplosionKnockbackDirection`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 敵の爆発 — この爆発に当たると敵が爆心地と反対方向に吹き飛ぶ
- §6.2 コード確認により、`CmpAtomFEnemyChainExplosion::BeginDyingExplosion()` から `AtomF::SpreadEnemyExplosionKnockbackFromCenter()` を呼び出し、影響範囲内の生存している他敵へ `CmpAtomFEnemyKnockback::ApplyKnockbackAwayFromExplosionCenter()` を適用していることを確認した。
- `ApplyKnockbackAwayFromExplosionCenter` は `RefOwner().GetWorldPosition() - explosionCenterWorld`（XZ 平面で爆心地から外向きのベクトル）を正規化し、速さb で吹き飛びと連鎖爆発フラグ（`chainImpactKnockback_`）を付与している。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）成功（exit 0）。

### 2. 次

- 「敵の爆発 › 当たり判定は半径cの球」→ `feature/AtomFEnemyExplosionHitRadiusC`（爆発球半径c と他敵 `CmpHitSphere` 半径・`IsActive`・`hitShape: sphere` 整合を精査予定）

## 実装済みと判断した項目（コード確認・即時 [x]）

- この爆発に当たると敵が爆心地と反対方向に吹き飛ぶ（`AtomFEnemyExplosionKnockbackSpread`・`CmpAtomFEnemyKnockback::ApplyKnockbackAwayFromExplosionCenter`）

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 爆発に当たった敵を爆心地と反対方向へ吹き飛ばす | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-23（ループ tick・敵爆発の当たり判定 半径cの球）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyExplosionHitRadiusC`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 敵の爆発 — 当たり判定は半径cの球
- `Project/Codes/Include/System/AtomF/AtomFParametersRuntime.h` / `Project/Codes/Source/System/AtomF/AtomFParametersRuntime.cpp`: `EnemyExplosionParameters` に `hitShape`（既定 `"sphere"`）を追加し、`parameters.json` の `enemy.explosion.hitShape` を読み込むようにした。
- `Project/Codes/Include/System/AtomF/AtomFParametersAccess.h` / `Project/Codes/Source/System/AtomF/AtomFParametersAccess.cpp`: `IsEnemyExplosionHitShapeSphere()` を追加した。
- `Project/Codes/Source/System/AtomF/AtomFEnemyExplosionKnockbackSpread.cpp`: `IsEnemyExplosionHitShapeSphere()` 確認と `hitSphere->GetIsActive()`・`other->IsActive()` チェックを追加し、`IsExplosionSphereIntersectingTarget()` で爆心地（半径c = `ResolveEnemyExplosionHitRadiusMeters()` = `0.4m`）と他敵 `CmpHitSphere` の各球（`target.GetWorldPosition() + sphere.localOffset`、半径合算 `explosionRadiusMeters + sphere.radius`）の 3D 球交差判定へ改めた。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「敵の焦りモーション › 当たり判定は動かない」→ `feature/AtomFEnemyPanicColliderStatic`

## 推測した用途（§6.4）

- **敵爆発の当たり判定（半径cの球）**:
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` プレイ中の連鎖爆発発生時）
  - 用途: `parameters.json` の `enemy.explosion.hitShape` と `hitRadiusMeters`（半径c = `0.4m`）に従い、爆心地を中心とする球と他敵の `CmpHitSphere` との球交差で吹き飛び伝播先を判定する。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵爆発の当たり判定を半径cの球にする | 有効 | プリミティブ(直方体や球、円柱やカプセル、面や線)同士の当たり判定が実装してある |

---

## セッション 2026-11-23（ループ tick・敵焦りモーション中の当たり判定固定）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyPanicColliderStatic`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 敵の焦りモーション — 当たり判定は動かない
- `Project/Codes/Include/System/Entity/Components/CmpMeshSphere.h` / `Project/Codes/Source/System/Entity/Components/CmpMeshSphere.cpp`: 見た目専用の `drawOffset_`（`SetDrawOffset` / `GetDrawOffset`）を追加し、描画時のワールド行列へ `Matrix4x4::Translate(drawOffset_)` のみ適用するようにした。
- `Project/Codes/Include/System/AtomF/CmpAtomFEnemyTargetedPanic.h` / `Project/Codes/Source/System/AtomF/CmpAtomFEnemyTargetedPanic.cpp`: `RefOwner().RefTransform().translate` を直接揺らす処理を廃止し、`ApplyVisualShakeOffset()` / `ResetVisualShakeOffset()` で `CmpMeshSphere::SetDrawOffset` だけを更新するように分離した。これにより焦り中の `Entity::GetWorldPosition()` および `CmpHitSphere` 当たり判定が一切移動・振動しなくなった。
- `Project/Codes/Source/Scene/SceneBuildUpAtomFEnemySpawn.cpp`: `CmpAtomFEnemyTargetedPanic` を `CmpAtomFEnemyWalkToPlayer` / `CmpAtomFEnemyPeriodicCharge` より先に登録し、狙われた初フレームから移動系より先に `BlocksLocomotion()` が有効になるようにした。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「敵の焦りモーション › その場で小刻みに震える（ぱっと見で分かる程度）」→ `feature/AtomFEnemyPanicShakeVisual`

## 推測した用途（§6.4）

- **敵焦りモーション中の当たり判定固定（見た目オフセット分離）**:
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` で自機エイムカプセルに敵が狙われたとき）
  - 用途: 敵の焦りモーションによる振動をメッシュ描画の見た目オフセットのみに閉じ込め、自機弾やエイム判定・接触ダメージに用いる `CmpHitSphere` 位置がぶれないようにする。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵焦りモーション中に当たり判定を動かさない | 有効 | プリミティブ(直方体や球、円柱やカプセル、面や線)同士の当たり判定が実装してある |

---

## セッション 2026-11-23（ループ tick・敵焦りモーションの小刻みな震え視覚調整）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyPanicShakeVisual`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 敵の焦りモーション — その場で小刻みに震える（ぱっと見で分かる程度）
- `Project/Codes/Include/System/AtomF/CmpAtomFEnemyTargetedPanic.h` / `Project/Codes/Source/System/AtomF/CmpAtomFEnemyTargetedPanic.cpp`:
  - `ApplyVisualShakeOffset(const Math::Vector3 &)` へ拡張し、`ComputePanicVisualShakeOffset()` を切り出した。
  - 斜め俯瞰カメラ（距離約 14m）からの視認性を踏まえ、敵直径 `0.5m` に対してぱっと見で分かる水準（水平振幅 `0.12m`、上方向のみ微小バウンス `0.045m`、角速度 `76.0 rad/s`）へ調整し、狙われ開始時に位相をリセットして即座に小刻み振動が始まるようにした。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「敵の焦りモーション › 天辺から汗のパーティクルを流す」→ `feature/AtomFEnemyPanicSweatParticles`

## 推測した用途（§6.4）

- **敵焦りモーションの小刻み震え**:
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` で自機エイムカプセルに敵が狙われたとき）
  - 用途: 俯瞰視点のゲームプレイ中に「この敵が自機に狙われて焦って立ち止まっている」ことをプレイヤーが一目で識別できる視覚フィードバックを与える。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵が焦りモーションで小刻みに震える視認性を整える | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-23（ループ tick・敵焦りモーション天辺からの汗パーティクル）

### 1. 完了

- **ブランチ:** `feature/AtomFEnemyPanicSweatParticles`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 敵の焦りモーション — 天辺から汗のパーティクルを流す
- `Project/Assets/Particles/enemy_panic_sweat.json`: 水色の水滴球（`BUILTIN_SPHERE`・`CIRCLE_XZ` 放射拡散＋重力 `-5.8`）を放出して下に流れ落ちながら消える汗パーティクル定義を追加した。
- `Project/Codes/Include/System/Particle/GameCombatParticleEffects.h` / `Project/Codes/Source/System/Particle/GameCombatParticleEffects.cpp`: `PlayEnemyPanicSweatEffect(worldPosition, sequenceId)` を追加した。
- `Project/Codes/Include/System/AtomF/CmpAtomFEnemyTargetedPanic.h` / `Project/Codes/Source/System/AtomF/CmpAtomFEnemyTargetedPanic.cpp`: `EmitSweatFromTopIfDue()` を追加し、焦りモーション中は敵球の天辺座標（`GetWorldPosition() + visualOffset + {0, bodyRadius, 0}`）から `0.14s` 間隔で汗パーティクルを連続発生させるようにした（ノックバック・死亡・焦り解除時はタイマーをリセット）。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「自機がダメージを受けたときの演出 › 赤いビネットが画面端に 0.75 秒出る」→ `feature/AtomFPlayerDamageVignette`

## 推測した用途（§6.4）

- **敵焦りモーションの汗パーティクル**:
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` で自機エイムカプセルに敵が狙われている間）
  - 用途: 小刻みな震えと合わせて敵頭頂部から汗しずくを流すことで、狙われた敵の焦り状態を視覚的に分かりやすく伝える。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 敵の天辺から焦り汗パーティクルを流す | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-23（ループ tick・自機被弾時の赤いビネット 0.75 秒表示）

### 1. 完了

- **ブランチ:** `feature/AtomFPlayerDamageVignette`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 自機がダメージを受けたときの演出 — 赤いビネットが画面端に 0.75 秒出る
- `Project/Codes/Include/Camera/PlayerDamageScreenVignette.h` / `Project/Codes/Source/Camera/PlayerDamageScreenVignette.cpp`: 指定秒数で表示できる `RequestShow(float durationSeconds)` オーバーロードを追加し、描画後に `BlendMode::NONE` へ戻すようにした（既定 `0.48s` の既存呼出と互換維持）。
- `Project/Codes/Source/System/Entity/Components/CmpHealthHit.cpp`: 自機被弾時、`AtomF::ParametersRuntime::Instance().IsLoaded()` が真なら `AtomF::ResolvePlayerDamageVignetteSeconds()`（`0.75` 秒）を `PlayerDamageScreenVignette::RequestShow(durationSeconds)` へ渡すようにした。
- `Project/Codes/Source/Scene/SceneBuildUp.cpp`: `EnterScene()` / `ExitScene()` の `Reset()`、`Update()` の `Update(dt)`、および `DrawScene()` / `DrawRuntimeViewContent()` の双方で `PlayerDamageScreenVignette::Draw(engine)` を呼ぶように統合した。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「自機がダメージを受けたときの演出 › 効果音が 1 回鳴る」→ `feature/AtomFPlayerDamageSeOnce`

## 推測した用途（§6.4）

- **自機被弾時の画面端赤ビネット（0.75 秒）**:
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` で自機が敵接触等でダメージを受けたとき）
  - 用途: 無敵時間と同期した `0.75` 秒の画面端赤ビネット表示により、プレイヤーが被弾した瞬間と残りの警戒時間を直感的に視認できるようにする。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 自機被弾時に画面端へ 0.75 秒赤いビネットを出す | 有効 | ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-23（ループ tick・自機被弾時の効果音 1 回再生）

### 1. 完了

- **ブランチ:** `feature/AtomFPlayerDamageSeOnce`
- **roadmap:** 全方位シューティングゲーム「atomF」 › 自機がダメージを受けたときの演出 — 効果音が 1 回鳴る（本件で `atomF` 節の全タスク完了）
- `Project/Codes/Include/System/PlaySe.h`: `PreloadSeCandidates()` を追加し、再生せずに候補音声を事前読み込みできるようにした。
- `Project/Codes/Include/System/AtomF/AtomFParametersAccess.h` / `Project/Codes/Source/System/AtomF/AtomFParametersAccess.cpp`: `ResolvePlayerDamageSePlayCount()` を追加し、`parameters.json` の `player.damageFeedback.sePlayCount`（`1`）を取得できるようにした。
- `Project/Codes/Source/System/AtomF/AtomFPlayerHealthAttach.cpp`: 自機アタッチ時に `sePlayerDamage.mp3` を事前読み込みし、初回被弾時の遅延を防止した。
- `Project/Codes/Source/System/Entity/Components/CmpHealthHit.cpp`: 自機被弾時、`AtomF::ParametersRuntime::Instance().IsLoaded()` の場合は `ResolvePlayerDamageSePlayCount()`（`1` 回）分 `PlayPlayerDamageSe()` を呼び、無敵時間（`0.75s`）と合わせて被弾 1 回につき効果音が 1 回だけ鳴るように整合させた。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「EntityEditorUi › ヒット球は選択 1 球のみ編集」→ `feature/EntityEditorMultiHitSpheres`

## 推測した用途（§6.4）

- **自機被弾時の効果音 1 回再生**:
  - 使用場面: ランタイム（`Assets/Scenes/atomF.json` で自機が敵接触等でダメージを受けたとき）
  - 用途: `parameters.json` の `sePlayCount`（`1`）および事前読み込みにより、被弾の瞬間に遅延なく 1 回だけ被弾音を鳴らし、連続ヒットの多重再生を防ぐ。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 自機被弾時に効果音を 1 回鳴らす | 有効 | mp4やwavなど代表的な拡張子の音源ファイルを読み込める／ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-23（ループ tick・CmpHitSphere 複数球選択・一括編集 UI）

### 1. 完了

- **ブランチ:** `feature/EntityEditorMultiHitSpheres`
- **roadmap:** EntityEditorUi › ヒット球は選択 1 球のみ編集
- `Project/Codes/Include/System/Editor/Entity/EntityEditorUiDrawComponentParamsHitSphere.h` / `Project/Codes/Source/System/Editor/Entity/EntityEditorUiDrawComponentParamsHitSphere.cpp`: `CmpHitSphere` 用のインスペクタ UI を新規ファイルへ分離・実装し、Ctrl/Shift クリックや「全球選択」「選択を先頭のみに戻す」による複数球選択、選択中の複数球に対する「半径を一括適用」「オフセットを加算」の一括編集、「選択球を複製」、最低 1 球を維持する複数球一括削除、および選択中すべての球の `localOffset`・`radius` 同時編集に対応した。
- `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp`: 既存の `CmpHitSphere` 単一選択インスペクタコードを `EntityEditorUiDrawComponentParamsHitSphere::TryDrawCmpHitSphereComponentParams()` 呼び出しへ置き換えた。
- `Project/CG2_BuildUp.vcxproj`: 新規ヘッダ・ソース（`Release|x64` は除外）を登録し、`verify_vcxproj_coverage.py` を通過させた。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「EntityEditorUi › 未対応コンポーネントは警告表示のみ」→ `feature/EntityEditorUnsupportedComponentNotice`

## 推測した用途（§6.4）

- **CmpHitSphere 複数球選択・一括編集 UI**:
  - 使用場面: 自作エディタ（`SceneEntityEditor`）のインスペクタで、複数の当たり判定球を持つエンティティを編集するとき
  - 用途: 1 球ずつ切り替えながら編集する手間をなくし、複数選択した球の同時展開・複製・一括削除・半径一括設定・オフセット一括調整を素早く行えるようにする。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| ヒット球は選択 1 球のみ編集（複数球の選択・一括編集対応） | 有効 | 各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-23（ループ tick・未対応コンポーネント向け汎用 JSON フォールバックエディタ）

### 1. 完了

- **ブランチ:** `feature/EntityEditorUnsupportedComponentNotice`
- **roadmap:** EntityEditorUi › 未対応コンポーネントは警告表示のみ
- `Project/Codes/Include/System/Editor/Entity/EntityEditorUiDrawComponentParamsFallbackJson.h` / `Project/Codes/Source/System/Editor/Entity/EntityEditorUiDrawComponentParamsFallbackJson.cpp`: 専用インスペクタを持たないコンポーネントが警告表示のみで終わらないよう、ツリー形式の再帰 JSON エディタ（スカラー・配列・ネストオブジェクトの編集／要素追加・削除）、新規キー追加（型選択）、既定 `params` リセットボタン、および `ImGui::InputTextMultiline` を用いた生 JSON テキスト直接編集（例外なしの安全パース・構文エラー表示付き）を新規ファイルとして実装した。
- `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp`: 警告テキストのみで `return false;` していた分岐を `EntityEditorUiDrawComponentParamsFallbackJson::DrawFallbackComponentParams()` 呼び出しへ置き換えた。
- `Project/CG2_BuildUp.vcxproj`: 新規ヘッダ・ソース（`Release|x64` は除外）を登録し、`verify_vcxproj_coverage.py` を通過させた。
- **ビルド:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64` ともに成功（exit 0）。

### 2. 次

- 「EntityEditorUi › InputInt で実行順が 1 スロットのみ」→ `feature/EntityEditorInputIntExecOrderBatch`

## 推測した用途（§6.4）

- **未対応コンポーネント向け汎用 JSON フォールバックエディタ**:
  - 使用場面: 自作エディタ（`SceneEntityEditor`）のインスペクタで、専用 UI がまだ実装されていないコンポーネント（または既知型だが汎用フォールバックに落ちたスロット）の `params` を確認・調整するとき
  - 用途: 警告表示だけで行き止まりにせず、エディタ上で直接キー／配列／生 JSON を編集・保存してゲーム開発を継続できるようにする。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| 未対応コンポーネントは警告表示のみ（汎用 JSON フォールバック編集対応） | 有効 | 各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

- 2026-11-23 16:56: ユーザー指示によりループ（`roadmap-dev`）を停止し、占有（`lease.md`）を解放（`feature/EntityEditorInputIntExecOrderBatch` に未コミット作業あり）。

---

## セッション 2026-11-23 17:26（§6.2 既実装確認・EntityEditorUi › InputInt で実行順が 1 スロットのみ）

### 1. 現在の進捗状況と完了したこと

- **ブランチ:** `develop`（最新コミット `cdc7830` / `811b84d` を確認。コード変更不要の §6.2 完了判定のため新規作業ブランチ作成・コード実装はスキップ）
- **対象 roadmap 行:** `EntityEditorUi` › `InputInt で実行順が 1 スロットのみ`
- `Project/Codes/Include/System/Editor/Entity/EntityEditorInspectorMultiSlotExecutionOrderUi.h` / `Project/Codes/Source/System/Editor/Entity/EntityEditorInspectorMultiSlotExecutionOrderUi.cpp` および `Project/Codes/Source/System/Editor/Entity/EntityEditorInspectorExecutionOrderBatchUi.cpp`・`Project/CG2_BuildUp.vcxproj` を確認し、以下がすでに `develop` 上で実装済みであることを確認した：
  - 選択中インスタンス内の同型・同種別一括適用および 0 からの連番整列（`EntityEditorInspectorExecutionOrderBatchUi`）
  - 折りたたみツリー「複数スロット実行順一括編集」での各スロット個別 `InputInt` 直接編集、チェック選択スロットへの「チェック中へ適用」および `-1` / `+1` 一括シフト
  - 複数インスタンス選択時の「選択中 N 体の同型へ実行順を適用」
- `EditorAssets/Tools/CursorAI/docs/feature-memos/roadmap.md`: 当該行を `- [x]` に更新した。
- **ビルド確認:** `Development|x64`（`buildDevelopment.cmd`）・`Debug|x64`（MSBuild `Project/CG2_BuildUp.slnx`）ともに成功（exit 0）。ランタイム手動確認は未実施。

### 2. 発生した課題または設計上の選択肢

- `roadmap.md` の `EntityEditorUi` 節には類似項目（`実行順 InputInt が単体` や既に完了済みの項目など）が残っているため、次回以降に §6.1 / §6.2 に基づき順次確認・整理する。

### 3. 次に実行すべきステップの提案

- 人間レビュー／指示待ち（「ロードマップを実行して。」またはコミット指示）。
- 次候補: `EntityEditorUi` › `Selectable はパス選択のみで即開かない`（`EditorImportedAssetProjectPanelList.cpp` / `PrefabAssetProjectPanelList.cpp` / `EntityEditorUiEditableSceneList.cpp` 等の一覧 `ImGui::Selectable` でダブルクリック等による即開き対応を検討）。

## 実装済みと判断した項目（コード確認・即時 [x]）

- `EntityEditorUi › InputInt で実行順が 1 スロットのみ`: `EntityEditorInspectorExecutionOrderBatchUi.cpp` と `EntityEditorInspectorMultiSlotExecutionOrderUi.cpp` により複数スロット一括 `InputInt`・`±1` シフト・各行インライン編集・複数インスタンス同型適用がすでに実装済みであり、`Development|x64` / `Debug|x64` ビルドも通過したため `- [x]` に更新。

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（本セッションは §6.2 コード確認による `[x]` 更新のみで新規コード変更なし。なお既存実装の用途は自作エディタのインスペクタにおける複数スロット・複数エンティティ間の実行順一括調整）

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| InputInt で実行順が 1 スロットのみ | 有効 | 各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-23 18:55（Release|x64 未解決外部シンボル 21 件の解消）

### 1. 現在の進捗状況と完了したこと

- **ブランチ:** `fix/ExecuteRoadmap`
- **内容:** `Release|x64` リンク時に発生していた 21 件の `LNK2001`（未解決の外部参照）を解消した。
  - `Project/Codes/Include/Scene/CommandSceneBootLaunchPathGate.h` / `Project/Codes/Source/Scene/CommandSceneBootLaunchPathGate.cpp` / `Project/Codes/Include/Scene/CommandSceneDevelopmentBoot.h` / `Project/Codes/Source/Scene/CommandSceneDevelopmentBoot.cpp`: `IsValidBootSceneJsonPath` を `CommandSceneBootLaunchPathGate` 側へ移設し、`Release|x64` の `CommandSceneBootEnterApply.cpp` からも解決できるようにした。
  - `Project/Codes/Source/Scene/SceneMovieAudioPolicy.cpp`: `SceneEntityEditor::Instance()` のインクルードと参照を `#if defined(DEVELOPMENT) && defined(USE_IMGUI)` でガードした（`SceneMovieReturnTarget.cpp` 等と同契約）。
  - `Project/CG2_BuildUp.vcxproj`: ImGui に一切依存せず `Release|x64` のランタイム（`EntityEditorDefinition.cpp`・`EditorSceneAsset.cpp`・`EditorSceneRuntime.cpp`・`CmpAtomFPlayerAim.cpp`・`SceneBuildUpRuntimeSubviewDrawCamera.cpp`）から直接参照されているデータ／ユーティリティ実装 17 ファイル（`EditorFrameFitCameraPerspectiveParams.cpp`・`StageViewCameraTuning.cpp`・`EditorEditViewScreenRay.cpp`・`EntityEditorComponentSlotParamsNormalize.cpp`・`EntityEditorDefaultParamsCmpCamera.cpp`・`EntityEditorComponentSlotKindResolve.cpp`・`EntityEditorKnownComponentRegistry.cpp`・`EditorSceneRuntimeDrawPreviewApply.cpp`・`EditorSceneRuntimeEntityHasComponentForSlot.cpp`・`EditorSceneRuntimeEditorAssetPathPreviewLink.cpp`・`EditorSceneRuntimeEditorAssetPathPreviewRelink.cpp`・`EditorSceneRuntimeMainCameraSupplement.cpp`・`EditorSceneRuntimeBuildWarnings.cpp`・`EditorLaunchConfigLoadDiagnostics.cpp`・`EditorLaunchConfigJsonDocument.cpp`・`EditorLaunchConfigStartupPathNormalize.cpp`・`EditorScenePlayRequestStaging.cpp`）の `Release|x64` `ExcludedFromBuild` を解除した。
- **検証・ビルド確認:** `verify_vcxproj_coverage.py` OK。`Development|x64`・`Debug|x64`・`Release|x64` の全 3 構成すべてでコンパイルおよびリンクが成功（exit 0）することを確認した。

## 推測した用途（§6.4）

- **Release|x64 構成でのランタイムシーン読込・カメラ・エイム・起動構成解決のリンク整合**:
  - 使用場面: ゲーム配布用の `Release|x64` ランタイムビルドおよび実行時（`launch_config.json` からの起動シーン解決、`EditorSceneRuntime` によるシーン JSON からのエンティティ生成、`atomF` マウスエイム平面ピック、ビューポート FOV 解決）。
  - 用途: エディタ UI（ImGui）を除外した配布用 `Release|x64` でも、シーン JSON 読込・起動設定・ゲームプレイ用数学ユーティリティが欠落せずビルド・動作するようにする。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| Release\|x64 の未解決外部シンボル 21 件の解消 | 有効 | シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-24 07:20（着手前確認・ブランチ整理待ちで一時停止）

### 1. 現在の進捗状況と完了したこと

- **現在ブランチ:** `fix/ExecuteRoadmap`（`origin/fix/ExecuteRoadmap` より ahead 1 の `4f56e5c 仮コミット` ＋ 未コミット変更 7 ファイルあり）
- **対象 roadmap 行:** `EntityEditorUi` › `Selectable が path 全文`（ファイル順最初の `- [ ]`）
- **§6.2 コード確認結果:** `Project/Codes/Source/System/Editor/Prefab/PrefabAssetProjectPanelList.cpp`（49行目）および `Project/Codes/Source/System/Editor/EditorImportedAssetProjectPanelList.cpp`（47行目）の `ImGui::Selectable` は依然としてパス全文（`path.c_str()`）をそのままラベル表示しているため未実装と確認（`EntityEditorUiEditableSceneList.cpp` は `SceneDisplayNameFromPath(path) + "  |  " + path`）。
- **§6.7 最終目標査定:** 有効かつ矛盾なし（下記「最終目標との査定（§6.7）」参照）。
- **ブランチ作成・コード編集:** `fix/ExecuteRoadmap` 上の未コミット変更（`EditorAssets/Tools/CursorAI/Control/cursorRoadmapLoop.ps1`、`EditorAssets/Tools/CursorAI/Control/geminiRoadmapLoop.ps1`、`Project/Codes/Include/Scene/SceneBuildUpDedicatedRuntimeViewWindow.h`、`Project/Codes/Source/Engine/AtrumEngine.cpp`、`Project/Codes/Source/Engine/Resource/TextureStorage.cpp`、`Project/Codes/Source/Scene/SceneBuildUp.cpp`、`Project/Codes/Source/Scene/SceneBuildUpDedicatedRuntimeViewWindow.cpp`）が存在するため、`startTaskBranchFromDevelop.ps1`（`develop` 以外の未コミット変更がある場合はエラー停止）および無断コミット／破棄禁止ルールにより新規作業ブランチ（`fix/EntityEditorSelectableShortLabel`）への切替・コード編集を行わず停止した。

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- **課題:** 現在の `fix/ExecuteRoadmap` に別作業の未コミット変更 7 ファイルと `4f56e5c 仮コミット` が残っているため、§6.5 の `develop` 起点での新規作業ブランチ作成へ進めない。
- **選択肢:**
  - **A. `fix/ExecuteRoadmap` をコミット・`develop` へマージ・元ブランチ削除後、`develop` から `fix/EntityEditorSelectableShortLabel` を作成して続行**
    - メリット: 現在の作業内容を失わず `develop` に統合した上でクリーンに次タスクへ進める。
    - デメリット: 現在の未コミット変更がビルド可能・マージ可能な状態であることを前提とする。
  - **B. 未コミット変更を `git stash` に退避し、`develop` から `fix/EntityEditorSelectableShortLabel` を作成して続行**
    - メリット: `fix/ExecuteRoadmap` の作業を一旦そのまま残し、次タスクを独立ブランチで進められる。
    - デメリット: 後で `fix/ExecuteRoadmap` へ戻って stash を復元・整理する必要がある。

### 3. 次に実行すべきステップの提案

- `fix/ExecuteRoadmap` の未コミット変更 7 ファイルおよび `4f56e5c 仮コミット` の扱い（コミットして `develop` マージするか、`git stash` 退避するか、手動整理するか）を人間から指示いただいた後、`startTaskBranchFromDevelop.ps1 -BranchName "fix/EntityEditorSelectableShortLabel"` でブランチを作成し、`EntityEditorUi › Selectable が path 全文` の実装に進む。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし（`roadmap.md` の該当行は `- [ ]` のまま維持）

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（コード未編集・ブランチ整理待ちで停止）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorUi › Selectable が path 全文 | 有効 | ・エンティティをプレハブ化してjsonデータとしてアセット保存することができる / ・ファイル内部の編集とは別の、ファイルの新規作成や削除、名前変更などが行えるようになっている |

---

## セッション 2026-11-24 07:24（EntityEditorUi › Selectable が path 全文）

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `fix/EntityEditorSelectableShortLabel`（`develop` 起点で `startTaskBranchFromDevelop.ps1` により作成・push）
- **対応する roadmap 行:** `EntityEditorUi` › `Selectable が path 全文`（`roadmap.md` で `- [x]` に更新）
- **実装内容:**
  - `Project/Codes/Include/System/Editor/Entity/EntityEditorProjectPanelSelectableLabel.h` / `Project/Codes/Source/System/Editor/Entity/EntityEditorProjectPanelSelectableLabel.cpp` を新規作成し、`BuildPrefabSelectableLabel`（拡張子 `.json` を除いた `stem`＋`##フルパス`）、`BuildImportedAssetSelectableLabel`（拡張子付き `filename`＋`##フルパス`）、およびホバー時に完全な相対パスを表示する `DrawFullPathHoverTooltip` を実装した。
  - `Project/Codes/Source/System/Editor/Prefab/PrefabAssetProjectPanelList.cpp` の `DrawPrefabJsonFileList` で `ImGui::Selectable` に `BuildPrefabSelectableLabel` と `DrawFullPathHoverTooltip` を適用し、フルパス直表示を短縮表示＋ID一意化＋ツールチップ確認へ改善した。
  - `Project/Codes/Source/System/Editor/EditorImportedAssetProjectPanelList.cpp` の `DrawImportedAssetsFolderList` でも同様に `BuildImportedAssetSelectableLabel` と `DrawFullPathHoverTooltip` を適用した。
  - `Project/CG2_BuildUp.vcxproj` および `Project/CG2_BuildUp.vcxproj.filters` へ新規ヘッダ／ソース（`Release|x64` は `ExcludedFromBuild=true`、`Development|x64` は `false`）を登録した。
- **検証状況:** **ビルド未実施・静的矛盾チェック済み**
  - `python EditorAssets/Tools/CursorAI/verify_vcxproj_coverage.py`: OK
  - `python EditorAssets/Tools/CursorAI/verify_prefab_json_rename_delete.py`: OK
  - ヘッダ宣言・実装・呼び出し元の名前空間（`Atrum::EntityEditor::ProjectPanelSelectableLabel` 完全修飾）・シグネチャ・`#include` パス・`DEVELOPMENT && USE_IMGUI` 分岐および `Release|x64` 除外の整合を目視照合済み。

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- プレハブ一覧の短縮表示は拡張子 `.json` を除いた `stem`（同名別階層での ID 衝突防止のため `##フルパス` 付き）、Imported アセット一覧は `.png`/`.obj`/`.gltf` 等の拡張子識別のために `filename`（拡張子付き）＋`##フルパス` とした。フルパス確認が必要なケース向けにホバーツールチップを付与している。

### 3. 次に実行すべきステップの提案

- 人間レビュー／指示待ち（コミット・プッシュ許可、または次の roadmap タスク実行指示）。
- 次候補: `EntityEditorUi` › `タグ全解除は選択 1 件`（想定ブランチ名: `feature/EntityEditorBatchClearTags` または `fix/EntityEditorBatchClearTags`）。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **EntityEditorUi › Selectable が path 全文（プレハブ／Imported 一覧の短縮ラベル表示）**:
  - 用途: 自作エディタ（`SceneEntityEditor`）のプロジェクトパネルで、大量のプレハブ JSON や Imported アセットから対象を素早く識別・選択し、複製・一括追加・一括適用・改名・削除を行う。
  - 使用場面: 制作（エディタ専用 UI）
  - 用途から決めた仕様: プレハブは `.json` 固定のため `stem`、Imported アセットは拡張子識別のために `filename` を表示ラベルとし、`##フルパス` で ImGui ID を一意化しつつホバーツールチップでフルパスを参照可能にした。`Release|x64` からは `ExcludedFromBuild` で除外し、ランタイムコストに影響させない。

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorUi › Selectable が path 全文 | 有効 | ・エンティティをプレハブ化してjsonデータとしてアセット保存することができる / ・ファイル内部の編集とは別の、ファイルの新規作成や削除、名前変更などが行えるようになっている |

---

## セッション 2026-11-24 07:33（§6.2 既実装確認・EntityEditorUi › タグ全解除は選択 1 件）

### 1. 現在の進捗状況と完了したこと

- **現在ブランチ:** `fix/EntityEditorSelectableShortLabel`（前タスクの未コミット変更が残っているため、§6.2 の既存実装確認・チェック更新のみを実施し、新規コード変更およびブランチ作成はスキップ）
- **対応する roadmap 行:** `EntityEditorUi` › `タグ全解除は選択 1 件`（`roadmap.md` で `- [x]` に更新）
- **コード確認結果:**
  - `Project/Codes/Include/System/Editor/Entity/EntityEditorClearAllEntityTags.h` / `Project/Codes/Source/System/Editor/Entity/EntityEditorClearAllEntityTags.cpp` に、複数インデックスへ一括でタグおよび `tagHashes` をクリアする `ClearAllTagsOnInstanceIndices` と単体用 `ClearAllTagsOnDefinition` がすでに実装されていることを確認。
  - `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp` の `DrawInspectorPanel`（936〜943行目）にて複数選択時（`ui_.selectedInstanceIndices.size() > 1`）に `DrawMultiInstanceBatchPanel` が呼ばれ、646〜655行目の「タグをすべて解除（選択全体）」ボタンから `ClearAllTagsOnInstanceIndices(scene, selection)` が実行されることを確認。
  - `implementation_gaps_index.json` 上の先行完了項目「タグをすべて解除が選択 1 体のみ」（`[x]`）と同趣旨の重複ギャップ（629-632行時点の `document.tags.clear()` 単体問題）であり、要件を満たしていると判断した。
- **検証状況:** ビルド未実施・静的矛盾チェック済み（コード変更なしのため `verify_vcxproj_coverage.py` のみ確認）

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- 現在 `fix/EntityEditorSelectableShortLabel` 上に前タスク（`Selectable が path 全文`）および `cursorRoadmapLoop.ps1` の未コミット変更が残っている。次タスク（`EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない`）へ進む際は、`startTaskBranchFromDevelop.ps1` の前提を満たすために現在の変更のコミット／develop マージまたは退避が必要。

### 3. 次に実行すべきステップの提案

- 人間レビュー／指示待ち（現在の未コミット変更のコミット・`develop` マージ許可、または次タスク実行指示）。
- 次候補: `EntityEditorHierarchy` › `ヒエラルキーがタグでフィルタ／一括選択できない`（想定ブランチ名: `feature/EntityEditorHierarchyTagFilter`）。

## 実装済みと判断した項目（コード確認・即時 [x]）

| roadmap の文言 | 根拠（パス・シーン・API） | 判定 |
|----------------|---------------------------|------|
| EntityEditorUi › タグ全解除は選択 1 件 | `EntityEditorClearAllEntityTags::ClearAllTagsOnInstanceIndices`（`EntityEditorClearAllEntityTags.h` / `.cpp`）および `EntityEditorUi.cpp`（646〜655行目「タグをすべて解除（選択全体）」・936〜943行目複数選択分岐）により複数選択インスタンスの一括タグ解除が実装済み | 万全 |

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（本セッションは §6.2 コード確認による `[x]` 更新のみで新規コード変更なし。既存実装の用途は自作エディタでの複数選択エンティティに対するテスト用・分類用タグの一括リセット）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorUi › タグ全解除は選択 1 件 | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---

## セッション 2026-11-24 07:50（着手前査定・ブランチ整理確認待ちで停止）

### 1. 現在の進捗状況と完了したこと

- **現在ブランチ:** `fix/EntityEditorSelectableShortLabel`（前タスクの未コミット変更 9 ファイルが残存）
- **対象 roadmap 行:** `EntityEditorHierarchy` › `ヒエラルキーがタグでフィルタ／一括選択できない`（ファイル出現順最初の `- [ ]`）
- **§6.2 コード確認結果:** `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp` の `DrawHierarchyPanel`（722〜770行目）および `DrawHierarchyNode`（674〜702行目）は全インスタンスを無条件列挙しており、タグによる絞り込みおよび一致インスタンスの一括選択 UI は未実装であることを確認した。
- **§6.7 最終目標査定:** 有効かつ矛盾なし（下記「最終目標との査定（§6.7）」参照）。
- **ブランチ確認待ち:** 現在の `fix/EntityEditorSelectableShortLabel` 上に別タスク（`Selectable が path 全文`・`タグ全解除は選択 1 件`・`cursorRoadmapLoop.ps1`）の未コミット変更が存在し、次タスクは別趣旨（想定ブランチ名: `feature/EntityEditorHierarchyTagFilter`）のため、無断コミット・退避・ブランチ流用を行わずユーザーへ対応方針を確認する状態で停止した。

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- **課題:** `fix/EntityEditorSelectableShortLabel` 上の未コミット変更を先に整理（コミット＆`develop` マージ＆ブランチ削除、または `git stash` 退避）しないと、§6.5 の `startTaskBranchFromDevelop.ps1` で `feature/EntityEditorHierarchyTagFilter` を作成できない。

### 3. 次に実行すべきステップの提案

- ユーザーへ現在の未コミット変更の扱い（`finishTaskGit.ps1` でコミット・`develop` マージ・元ブランチ削除して次ブランチ作成するか、`git stash` 退避するか、手動整理するか）を確認し、指示に応じて `feature/EntityEditorHierarchyTagFilter` で実装を開始する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし（`roadmap.md` の該当行は `- [ ]` のまま維持）

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（コード未編集・ブランチ整理確認待ちで停止。想定用途は制作：エディタ専用 UI にて同一タグを持つエンティティ群をヒエラルキーで絞り込み／一括選択し、一括編集・削除・タグ操作の反復手間を削減する）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる / ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-24 08:06（ロードマップ確認・ブランチ整理指示待ちで停止）

### 1. 現在の進捗状況と完了したこと

- **現在ブランチ:** `fix/EntityEditorSelectableShortLabel`（前タスク等の未コミット変更 9 ファイルが残存）
- **対象 roadmap 行:** `EntityEditorHierarchy` › `ヒエラルキーがタグでフィルタ／一括選択できない`（ファイル出現順最初の `- [ ]`）
- **§6.2 コード確認結果:** `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp` の `DrawHierarchyPanel`（722〜770行目）および `DrawHierarchyNode`（674〜702行目）を再確認し、タグによる絞り込みおよび一致インスタンスの一括選択 UI が未実装であることを確認した。
- **§6.7 最終目標査定:** 有効かつ矛盾なし（下記「最終目標との査定（§6.7）」参照）。
- **停止理由:** 新規タスク（想定ブランチ名: `feature/EntityEditorHierarchyTagFilter`）へ進むには `startTaskBranchFromDevelop.ps1` で `develop` から作業ブランチを新規作成する必要があるが、現在の `fix/EntityEditorSelectableShortLabel` 上に別タスクの未コミット変更（`Selectable が path 全文`、`タグ全解除は選択 1 件`、`cursorRoadmapLoop.ps1` 等）が残っているため、無断コミット・退避・ブランチ流用を行わずユーザーへ対応方針を確認する状態で停止した。

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- **課題:** `fix/EntityEditorSelectableShortLabel` に未コミット変更が残っている状態では、`startTaskBranchFromDevelop.ps1` が前提チェックで停止する。
- **選択肢:**
  1. 現在の変更をコミット・`develop` へマージ・元ブランチ削除（`finishTaskGit.ps1`）した上で `feature/EntityEditorHierarchyTagFilter` を作成する（ただし `cursorRoadmapLoop.ps1` の変更も含めるか分離するか要確認）。
  2. 現在の変更を一時的に `git stash` へ退避してから `feature/EntityEditorHierarchyTagFilter` を作成する。
  3. ユーザー側で手動整理（コミット／マージ等）が完了するまで待機する。

### 3. 次に実行すべきステップの提案

- ユーザーに現在の未コミット変更（`fix/EntityEditorSelectableShortLabel` 上の 9 ファイル）の扱いを指示してもらい、整理完了後に `feature/EntityEditorHierarchyTagFilter` を作成して `EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない` の実装に着手する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし（`roadmap.md` の該当行は `- [ ]` のまま維持）

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（コード未編集・ブランチ整理確認待ちで停止。想定用途は制作：エディタ専用 UI にて同一タグを持つエンティティ群をヒエラルキーで絞り込み／一括選択し、一括編集・削除・タグ操作の反復手間を削減する）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる / ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-24 08:22（着手前査定・未コミット変更整理の指示待ちで停止）

### 1. 現在の進捗状況と完了したこと

- **現在ブランチ:** `fix/EntityEditorSelectableShortLabel`（未コミット変更 9 ファイルが残存）
- **対象 roadmap 行:** `EntityEditorHierarchy` › `ヒエラルキーがタグでフィルタ／一括選択できない`（ファイル出現順最初の `- [ ]`）
- **§6.2 コード確認結果:** `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp` の `DrawHierarchyNode`（674〜702行目）および `DrawHierarchyPanel`（722〜770行目）にて、タグフィルタおよび一致インスタンスの一括選択 UI が未実装であることを確認した。
- **§6.7 最終目標査定:** 有効かつ矛盾なし（下記「最終目標との査定（§6.7）」参照）。
- **停止理由:** 新規タスク（想定ブランチ名: `feature/EntityEditorHierarchyTagFilter`）へ進むには `startTaskBranchFromDevelop.ps1` で `develop` から新規作業ブランチを切る必要があるが、現在の `fix/EntityEditorSelectableShortLabel` 上に別タスクの未コミット変更（`Selectable が path 全文`、`タグ全解除は選択 1 件`、`cursorRoadmapLoop.ps1` 等 9 ファイル）が残っており、無断コミット・退避・別ブランチ流用を行えないためユーザーへ整理方針を確認する状態で停止した。

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- **課題:** `fix/EntityEditorSelectableShortLabel` に未コミット変更があるため `startTaskBranchFromDevelop.ps1` が前提チェックで停止する。
- **選択肢:**
  1. 現在の変更（9ファイルすべて、または `cursorRoadmapLoop.ps1` を除いた8ファイル）を `finishTaskGit.ps1` でコミット・`develop` へマージ・元ブランチ削除し、次ブランチ `feature/EntityEditorHierarchyTagFilter` を作成する。
  2. 現在の変更を一時的に `git stash` で退避した上で `feature/EntityEditorHierarchyTagFilter` を作成する。
  3. ユーザー側で手動整理（コミット／マージ等）が完了するまで待機する。

### 3. 次に実行すべきステップの提案

- ユーザーに現在の未コミット変更（`fix/EntityEditorSelectableShortLabel` 上の 9 ファイル）の扱いを選択してもらい、整理完了後に `feature/EntityEditorHierarchyTagFilter` を作成して `EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない` に着手する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし（`roadmap.md` の該当行は `- [ ]` のまま維持）

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（コード未編集・ブランチ整理確認待ちで停止。想定用途は制作：エディタ専用 UI で同一タグを持つエンティティ群をヒエラルキー上で絞り込み／一括選択し、一括編集・削除・タグ操作の手間を削減する）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる / ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-24 08:39（着手前査定・ブランチ整理の確認待ちで停止）

### 1. 現在の進捗状況と完了したこと

- **現在ブランチ:** `fix/EntityEditorSelectableShortLabel`（未コミット変更 9 ファイルが残存）
- **対象 roadmap 行:** `EntityEditorHierarchy` › `ヒエラルキーがタグでフィルタ／一括選択できない`（ファイル出現順最初の `- [ ]`）
- **§6.2 コード確認結果:** `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp` の `DrawHierarchyNode`（674〜702行目）および `DrawHierarchyPanel`（722〜770行目）にて、タグによる絞り込みおよび一致インスタンスの一括選択 UI が未実装であることを確認した。
- **§6.7 最終目標査定:** 有効かつ矛盾なし（下記「最終目標との査定（§6.7）」参照）。
- **停止理由:** 新規タスク（想定ブランチ名: `feature/EntityEditorHierarchyTagFilter`）へ進むには `startTaskBranchFromDevelop.ps1` で `develop` から新規作業ブランチを作成する必要があるが、現在の `fix/EntityEditorSelectableShortLabel` に前タスク関連（8ファイル）および `cursorRoadmapLoop.ps1`（1ファイル）の未コミット変更が残っている。整理方針の確認質問がスキップされたため、無断コミット・マージ・`git stash`・別ブランチ流用を行わず停止した。

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- **課題:** `fix/EntityEditorSelectableShortLabel` に未コミット変更があるため `startTaskBranchFromDevelop.ps1` が前提チェックで停止する。
- **選択肢:**
  1. 前タスク8ファイルのみを `finishTaskGit.ps1` でコミット・`develop` へマージ・元ブランチ削除し、`cursorRoadmapLoop.ps1` を一時 `stash` 退避した上で `feature/EntityEditorHierarchyTagFilter` を作成する（推奨）。
  2. 全9ファイルをまとめて `finishTaskGit.ps1` でコミット・`develop` へマージ・元ブランチ削除し、`feature/EntityEditorHierarchyTagFilter` を作成する。
  3. 全9ファイルを一時的に `git stash` で退避した上で `feature/EntityEditorHierarchyTagFilter` を作成する。
  4. ユーザー側で手動整理（コミット／マージ等）が完了するまで待機する。

### 3. 次に実行すべきステップの提案

- ユーザーから上記 1〜4 のいずれか（例:「前タスク8ファイルをコミット・マージして次へ」または「stashして次へ」）の指示を受けた上で、`feature/EntityEditorHierarchyTagFilter` を作成し `EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない` の実装に着手する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし（`roadmap.md` の該当行は `- [ ]` のまま維持）

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（コード未編集・ブランチ整理確認待ちで停止。想定用途は制作：エディタ専用 UI で同一タグを持つエンティティ群をヒエラルキー上で絞り込み／一括選択し、一括編集・削除・タグ操作の手間を削減する）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる / ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-24 08:55（着手前査定・ブランチ整理の確認待ちで停止）

### 1. 現在の進捗状況と完了したこと

- **現在ブランチ:** `fix/EntityEditorSelectableShortLabel`（未コミット変更 9 ファイルが残存）
- **対象 roadmap 行:** `EntityEditorHierarchy` › `ヒエラルキーがタグでフィルタ／一括選択できない`（ファイル出現順最初の `- [ ]`）
- **§6.2 コード確認結果:** `Project/Codes/Source/System/Editor/Entity/EntityEditorUi.cpp` の `DrawHierarchyNode`（674〜702行目）および `DrawHierarchyPanel`（722〜770行目）にて、タグフィルタおよび一致インスタンスの一括選択 UI が未実装であることを確認した。
- **§6.7 最終目標査定:** 有効かつ矛盾なし（下記「最終目標との査定（§6.7）」参照）。
- **停止理由:** 新規タスク（想定ブランチ名: `feature/EntityEditorHierarchyTagFilter`）へ進むには `startTaskBranchFromDevelop.ps1` で `develop` から新規作業ブランチを切る必要があるが、現在の `fix/EntityEditorSelectableShortLabel` 上に前タスク関連（8 ファイル）および `cursorRoadmapLoop.ps1`（1 ファイル）の未コミット変更が残っている。ブランチ整理方針の確認質問がスキップされたため、無断コミット・マージ・`git stash`・別ブランチ流用を行わず停止した。

### 2. 発生した課題または設計上の選択肢（メリット・デメリット）

- **課題:** `fix/EntityEditorSelectableShortLabel` に未コミット変更が残っているため、`startTaskBranchFromDevelop.ps1` が前提チェックで停止する。
- **選択肢:**
  1. 前タスク 8 ファイルのみを `finishTaskGit.ps1` でコミット・`develop` へマージ・元ブランチ削除し、`cursorRoadmapLoop.ps1` を一時 `stash` 退避した上で `feature/EntityEditorHierarchyTagFilter` を作成する（推奨）。
  2. 全 9 ファイルをまとめて `finishTaskGit.ps1` でコミット・`develop` へマージ・元ブランチ削除し、`feature/EntityEditorHierarchyTagFilter` を作成する。
  3. 全 9 ファイルを一時的に `git stash` で退避した上で `feature/EntityEditorHierarchyTagFilter` を作成する。
  4. ユーザー側で手動整理（コミット／マージ等）が完了するまで待機する。

### 3. 次に実行すべきステップの提案

- ユーザーから上記いずれかの指示（例: 前タスク 8 ファイルのコミット・マージ許可、全 9 ファイルのコミット・マージ許可、`git stash` 退避指示、または手動整理）を受けた上で、`feature/EntityEditorHierarchyTagFilter` を作成し `EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない` の実装に着手する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし（`roadmap.md` の該当行は `- [ ]` のまま維持）

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

なし（コード未編集・ブランチ整理確認待ちで停止。想定用途は制作：エディタ専用 UI で同一タグを持つエンティティ群をヒエラルキー上で絞り込み／一括選択し、一括編集・削除・タグ操作の手間を削減する）

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| EntityEditorHierarchy › ヒエラルキーがタグでフィルタ／一括選択できない | 有効 | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる / ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |


---

## セッション 2026-11-24 20:43

### 1. 現在の進捗状況と完了したこと

- ユーザー依頼: .cursor/ideas/game_atomF.md をもとに、エディタと配布版の両方でプレイできる形で atomF を実装する。
- 調査のみ。コード未編集。現状ブランチは ix/Editor（エディタ関連の未コミット変更あり）。
- atomF のコア（アリーナ・自機・弾・敵・エイム予測・焦り・連鎖爆発）は既存実装あり。起動導線は未配線（配布 launch_config は BillboardInstancing100、エディタは FeatureTest）。企画メモと parameters.json／roadmap 数値に差分あり。弾・敵側の予測カプセルは未実装。

### 2. 発生した課題または設計上の選択肢

- ブランチ主旨ズレのため、着手前にユーザー確認中（develop から feature 新設／先に fix/Editor を整理／起動導線のみ／企画メモ全面追従）。

### 3. 次に実行すべきステップの提案

- ユーザーのブランチ方針と実装範囲（起動導線のみ／企画メモ追従）の回答後に着手する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- ランタイムのゲームプレイ（トップビュー全方位シューティング）
- エディタ上のテストプレイと配布 exe での本編プレイの両方

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| game_atomF.md をエディタ／配布の両方でプレイ可能にする | 有効 | ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-24 21:12

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** feature/AtomFPlayableEditorAndRelease（fix/Editor の未コミットは stash に退避して develop から分岐）
- .cursor/ideas/game_atomF.md に合わせ、エディタ／配布の両方で atomF を起動できる形に整備。
- 起動導線: Project/Assets/launch_config.json と 
release_scene_carousel.json の先頭を ./Assets/Scenes/atomF.json に変更。
- 数値: parameters.json を企画どおり（弾 0.75m/s・半径 0.25、エイム予測 0.1、敵歩行 0.3、吹き飛び 1.0）。
- 予測判定: 自機弾予測カプセル／敵吹き飛び予測カプセルを新規追加。焦りはエイム＋弾予測で発動。
- 敵を円周上に 6 体スポーン。
- **ビルド:** Development / Debug / Release | x64 すべて成功（exit 0）。

### 2. 発生した課題または設計上の選択肢

- 企画に無い 5 秒突進・被弾ビネット・弾軌道残像は既存のまま残した。

### 3. 次に実行すべきステップの提案

- ランタイム操作確認。問題なければコミット許可後に finishTaskGit.ps1。
- fix/Editor 再開時は git checkout fix/Editor のあと git stash pop。

## 推測した用途（§6.4）

- ランタイムのゲームプレイ（atomF）
- Development／エディタ再生と Release 配布 exe の両方

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| game_atomF.md をエディタ／配布でプレイ可能に | 有効 | ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---
## 2026-11-25 — atomF Entity_0 を空エンティティに

### 1. 実施内容の要約
- `Project/Assets/Scenes/atomF.json` の `Entity_0` から `CmpMeshSphere` / `CmpRotator` を外し、`components` を空配列にした。
- Entity_1 は変更していない。
- コード変更なし（シーン JSON のみ）。ビルド未実施。

### 2. 発生した課題または設計上の選択肢
- なし。

### 3. 次に実行すべきステップの提案
- エディタで atomF を開き、Entity_0 が空であることを確認。

## 推測した用途（§6.4）
- レベル／アセット制作（シーン上のエンティティを空にして、後からコンポーネントを付け直す土台にする）

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| atomF Entity_0 の全コンポーネント削除 | 有効・矛盾なし | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |

---
## 2026-11-25 — atomF 敵色を赤色に（依頼・未着手）

### 1. 実施内容の要約
- ユーザー依頼「atomFの敵の色を赤色にして。」を §6.7 査定した。
- 判定が寄与なしのためコード変更は行っていない。
- 現状: `SceneBuildUpAtomFEnemySpawn.cpp` で `SetColor({0.92f, 0.28f, 0.25f, 1.0f})`（赤寄り）が既に設定済み。照明は `CmpMeshSphere` 既定で有効。

### 2. 発生した課題または設計上の選択肢
- 寄与なしのため実装停止。見た目が赤く見えない場合は照明の影響の可能性がある。

### 3. 次に実行すべきステップの提案
- 最終目標査定を外してでも変更するなら、純赤への変更／照明オフをユーザーが明示する。

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| atomF 敵の色を赤色にする | 寄与なし・矛盾なし | 完成形の説明のどの `・` 行の流れも進めない（ゲーム見た目の色調整のみ） |

---
## 2026-11-25 — atomF 全エンティティのライティングオフ

### 1. 実施内容の要約
- 自機・敵・自機弾の `CmpMeshSphere` に `SetLightingEnabled(false)` を追加。
- シーン JSON の Entity_1 は既に `isLightingEnabled: false`。床・壁の `DrawTriangle` も既に lighting=false。
- ブランチ: `feature/AtomFPlayableEditorAndRelease`（atomF 見た目調整のため継続）。

### 2. 発生した課題または設計上の選択肢
- §6.7 は寄与なしだが、直前ターンで提示した照明オフの明示依頼として着手。

### 3. 次に実行すべきステップの提案
- atomF シーンで自機・敵・弾の色がフラットに見えることを確認。

## 推測した用途（§6.4）
- ランタイムのゲームプレイ（atomF）で色を識別しやすくする
- エディタ再生時の見た目確認

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| atomF 全エンティティのライティングオフ | 寄与なし・矛盾なし（ユーザー明示で着手） | 完成形のどの `・` 行の流れも進めない（見た目の照明フラグ調整） |

---
## セッション 2026-11-25 06:01 — DirectionalLight エディタ設定（着手前）

### 1. 現在の進捗状況と完了したこと
- ユーザー依頼「エディタから DirectionalLight の輝度・色・角度等を設定して保存し、エディタプレイと配布版へ反映」を §6.7 査定した。
- 現状: 読込・反映経路は既にある（ApplySceneGameLightingFromProfile → game_scene_lighting.json）。エディタ UI での編集・保存が欠落（LightingEffectsDebugImGui は案内文のみ）。
- 現在ブランチ feature/AtomFPlayableEditorAndRelease は atomF 主旨のため、本タスク着手前にブランチ切替の確認待ち。

### 2. 発生した課題または設計上の選択肢
- A: 既存 game_scene_lighting.json に DirectionalLight を編集・保存する（推奨・既存読込経路と一致）
- B: Light をエンティティコンポーネント化して配置する（完成形の別 ・ 行。今回依頼より範囲が大きい）
- 作業ブランチが atomF 向けのままなので、develop から feature/DirectionalLightEditorProfile 等へ切る必要がある。

### 3. 次に実行すべきステップの提案
- ブランチ切替方針の承認後、A 案で UI・保存・即時プレビューを実装する。

## 実装済みと判断した項目（コード確認・即時 [x]）
- なし

## 保留・着手見送り（§6.3）
- なし（ブランチ確認待ちで実装未着手）

## 用途不明で確認待ち（§6.4）
- なし

## 推測した用途（§6.4）
- なし（実装前）

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| DirectionalLight をエディタで設定・保存しプレイ/配布へ反映 | 有効・矛盾なし | ・ライトをコンポーネント化してあり、Lightエンティティを配置/編集/削除できる／・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる／・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

### ビルド確認
- Debug|x64 / Development|x64: 成功
- Release|x64: 初回は obj Permission denied、再実行で成功

---

## セッション 2026-11-25 atomF 自機弾の独立エンティティ化

### 1. 現在の進捗状況と完了したこと

- 自機弾を `CmpChildEntitys` の子ではなく、シーン直下の独立エンティティとして発射するよう変更。
- Space / パッド A でクールダウン通過後、自機 `GetWorldPosition()` からエイム方向へ生成。
- 更新ループ中の `entities_` 破壊を避けるため `AtomFSceneEntityAdoptQueue` を新設し、走査後に `Flush`。
- 弾消滅時は `Finalize` + `RequestDestroy` で `PurgeDestroyedSceneEntities` に載せる。
- 自機から `CmpChildEntitys` を除去（弾専用だったため）。
- **ビルド:** Development / Debug / Release | x64 すべて成功（exit 0）。Debug 初回は LNK1104（obj ロック）で再ビルド成功。

### 2. 発生した課題または設計上の選択肢

- 更新中 `push_back` は反復子破壊になるため、待ち行列採用を選択。
- 新規 `.cpp` 追加に伴い `CG2_BuildUp.vcxproj` / `.filters` を更新（コンパイル登録のため）。

### 3. 次に実行すべきステップの提案

- atomF で Space/A 発射・自機移動と弾軌跡の独立性・敵ヒットをランタイム確認。
- 問題なければコミット許可後に `finishTaskGit.ps1`。

## 推測した用途（§6.4）

- ランタイムのゲームプレイ（atomF）で自機弾を親子依存なく管理する
- エディタ再生／配布ランタイム双方の射撃操作

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| atomF 自機弾を独立エンティティとして発射 | 有効・矛盾なし | ・ゲーム内のロジック(スクリプト)によって、保存したエンティティをゲーム内に生成することができる / ・機能として成り立つ最小単位の部品がそれぞれクラスに分けられ整理されている |

## セッション 2026-11-25（atomF マウスエイム・画面中心基準）

### 1. 変更内容

- `AtomFPlayerAimMousePick` の照準角計算を、**自機→カーソル下の地面点** から **画面中心とカーソルがそれぞれ刺さる XZ 水平面上の点の差分** に変更。
- エディタのシーンビュー／配布ランタイムのクライアントビューポート双方で同じロジックを使用。
- **ビルド:** Development | x64 成功。Debug / Release は別途フルビルド進行中。

## 推測した用途（§6.4）

- ランタイムの atomF ゲームプレイで、画面上のカーソル位置に対してツインスティック風にエイムする操作感

## 最終目標との査定（§6.7）

| タスク | 判定 | 根拠 |
|--------|------|------|
| マウスエイムを画面中心→カーソル向きに変更 | 有効・矛盾なし | ・ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

---

## セッション 2026-11-24 20:43

### 1. 現在の進捗状況と完了したこと

- ユーザー依頼: .cursor/ideas/game_atomF.md をもとに、エディタと配布版の両方でプレイできる形で atomF を実装する。
- 調査のみ。コード未編集。現状ブランチは ix/Editor（エディタ関連の未コミット変更あり）。
- atomF のコア（アリーナ・自機・弾・敵・エイム予測・焦り・連鎖爆発）は既存実装あり。起動導線は未配線（配布 launch_config は BillboardInstancing100、エディタは FeatureTest）。企画メモと parameters.json／roadmap 数値に差分あり。弾・敵側の予測カプセルは未実装。

### 2. 発生した課題または設計上の選択肢

- ブランチ主旨ズレのため、着手前にユーザー確認中（develop から feature 新設／先に fix/Editor を整理／起動導線のみ／企画メモ全面追従）。

### 3. 次に実行すべきステップの提案

- ユーザーのブランチ方針と実装範囲（起動導線のみ／企画メモ追従）の回答後に着手する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- ランタイムのゲームプレイ（トップビュー全方位シューティング）
- エディタ上のテストプレイと配布 exe での本編プレイの両方

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| game_atomF.md をエディタ／配布の両方でプレイ可能にする | 有効 | ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある |

## セッション 2026-11-25 13:20

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/HierarchyTagFilterSelect`
- **roadmap:** EntityEditorHierarchy — ヒエラルキーがタグでフィルタ／一括選択できない → `[x]`
- ヒエラルキーに `タグフィルタ` コンボ（カタログ＋フィルタなし）と `このタグで一括選択` を追加。
- フィルタ一致または子孫が一致するノードのみ表示。一括選択は当該タグを持つ全インスタンスを `selectedInstanceIndices` に載せる。
- **ビルド:** 未実施（ロードマップ実行 tick・静的矛盾チェックのみ）。新規ファイルなし。`verify_vcxproj_coverage.py` はリポジトリに無いため未実行。宣言・実装の整合（`UiState::hierarchyTagFilter`）と呼び出しを確認済み。

### 2. 発生した課題または設計上の選択肢

- フィルタ時は祖先も残して木構造を保つ（一致子孫がある親は表示）。
- タグパネルの `tagSearchFilter` とは別フィールド `hierarchyTagFilter` にした。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「Selectable が単一 instanceIndex のみ」。次ブランチ `feature/HierarchySelectableMultiIndex`。
- 15 分ループの次 tick で続行。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **ヒエラルキーがタグでフィルタ／一括選択できない**
  - 用途: レベル／アセット制作時に、同タグのエンティティを素早く絞り込んでまとめて選ぶ
  - 使用場面: エディタのヒエラルキーパネル（Authoring）
  - 用途から決めた仕様: カタログタグのコンボ絞り込み＋一括選択。木は一致子孫の祖先を残す

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| ヒエラルキーがタグでフィルタ／一括選択できない | 有効・矛盾なし | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |


## セッション 2026-11-25 13:37

### 1. 現在の進捗状況と完了したこと

- **着手ブランチ:** `feature/HierarchySelectableMultiIndex`
- **roadmap:** EntityEditorHierarchy — Selectable が単一 instanceIndex のみ → `[x]`（§6.2・実装スキップ）
- `UiState::selectedInstanceIndices` と Ctrl+クリックの `ToggleInstanceSelection`、複数選択時の `DrawMultiInstanceBatchPanel` が既にあることを確認。
- **ビルド:** 未実施（コード変更なし・静的確認のみ）。

### 2. 発生した課題または設計上の選択肢

- なし（既存実装で要件充足）。

### 3. 次に実行すべきステップの提案

- 次の `- [ ]` は「シーン JSON は新規作成と削除だけで、ファイル名の変更が無い」。次ブランチ `feature/EditorSceneJsonRename`。
- 15 分ループの次 tick で続行。

## 実装済みと判断した項目（コード確認・即時 [x]）

- Selectable が単一 instanceIndex のみ（Ctrl+複数選択と `selectedInstanceIndices` 済み）

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **Selectable が単一 instanceIndex のみ**
  - 用途: レベル制作で複数エンティティをまとめて選び、一括編集する
  - 使用場面: エディタのヒエラルキー（Authoring）
  - 用途から決めた仕様: 既存の Ctrl+トグル複数選択を要件充足とみなす

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| Selectable が単一 instanceIndex のみ | 有効・矛盾なし | ・各エンティティのコンポーネント構成やコンポーネント内部のパラメータをjsonデータとして作成/保存/編集/削除することができる |



## セッション 2026-11-26（描画の点滅修正・通常モード）

### 1. 現在の進捗状況と完了したこと

- **症状:** 一部オブジェクトの描画が断続的に欠ける（点滅）。
- **原因:** `Draw` / `DrawSprite` が毎フレーム書き換えるアップロードバッファ（頂点・インデックス・Material/Transformation CB・パーティクル CB/インスタンス）が **1 フレーム分しか無く**、バックバッファ 2 枚でフレームを重ねて実行しているのに全フレームが同じ領域を先頭から上書きしていた。`PostDraw` の `WaitForNextBuffer` は 2 フレーム前の完了しか待たないため、直前に Execute したフレームを GPU が描画中に次フレームの CPU が同じ領域へ書き込み、描画途中のフレームが別オブジェクトのデータを読んで欠ける。
- **頂点不足ではないことの確認:** 既存の計測ログ `Project/.cursor/debug-db6cac.log`（2205 フレーム）で頂点ピーク 1492 / 65536、CB ピーク 108 / 4096、オーバーフロー 0 件。
- **修正:** `VertexBuffer` / `IndexBuffer` / `MultiConstantBuffer` にフレームスロット（`frameSlotCount`・`SetFrameSlot`）を追加し、`Draw` / `DrawSprite` はバックバッファ数ぶん確保。`AtrumEngine::PostDraw` のフェンス待ち後と `InitRenderer` で、バックバッファ番号のスロットへ切り替える。パーティクルインスタンスは全スロットを 1 つの SRV で覆い、`instanceBufferBase` にスロット先頭を加算。
- **保険:** `Draw::HasBatchCapacity` を追加し、頂点・インデックス容量を超える描画は範囲外参照せずスキップ。
- **ビルド:** Development|x64 を実行（結果は下記 3 で報告）。

### 2. 発生した課題または設計上の選択肢

- 平行光源 CB（`SingleConstantBuffer<DirectionalLightData>`）も 1 枚共用だが、値の変化が稀なため今回は対象外。
- `Draw.cpp` / `AtrumEngine.cpp` に前回調査の計測コード（`AgentDebugLog*`、`.cursor/debug-db6cac.log` 出力）が残っている。

### 3. 次に実行すべきステップの提案

- 実機で点滅が消えたことを確認する。
- 確認後、計測コードとログファイルの削除を検討する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **フレームごとのアップロードバッファのスロット化**
  - 用途: ランタイムのゲームプレイおよびエディタ上のプレイで、弾・パーティクル・背景球など毎フレーム生成するジオメトリを欠けなく描画する
  - 使用場面: ランタイム描画（Development / Release 共通）
  - 用途から決めた仕様: CPU/GPU の並列実行（2 フレーム重ね）を維持するため GPU 同期を増やさず、バッファをバックバッファ数ぶん確保する。メモリ増は最大でも数 MB 程度。

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| 描画の点滅（アップロードバッファのフレーム間競合）修正 | 有効・矛盾なし | ・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある ／ ・…描画エンジン…などが基本機能として独立している |

### 追記（同セッション）: ビルド確認

- **Development|x64 ビルド成功**（`Generated/Outputs/Development/CG2_BuildUp.exe` 2026-11-26 11:39）。点滅修正のコードはエラーなし。
- ビルドを通すため、修正前から壊れていた Lua 関連の 3 件を修正（ユーザー承認済み）:
  - `LuaSystem.h` … `sol.hpp` の `u8"\xF0..."` が C5321 を出し TreatWarningAsError で停止 → include 前後で `#pragma warning(disable : 5321)`。
  - `LuaSystem.h/.cpp` … `ILuaBinder` 不完全型のままインラインのコンストラクタ／デストラクタが実体化され C2027 → 定義を `LuaSystem.cpp` へ移動。
  - LuaJIT リンク … `Externals/LuaJIT/lib/lua51.lib`・`bin/lua51.dll` が 32bit 版で LNK2001 ×53 → `src/` の 64bit 版をコピー。`buildLuaJitLib.cmd` は `src` で msvcbuild を実行し `src` の成果物を配置するよう修正（スクリプト自体は未実行）。
- 解析警告 `C6386`（`Draw.cpp` の Map 直後の memset）は確保サイズを解析器が追えないための既存パターンで、ビルドは通る。
- **未確認:** 実機で点滅が消えたかの目視確認。

## 2026-11-26 自機弾・敵の見た目調整（着手前の査定で停止）

依頼: 自機はそのまま、自機の弾は流線形、敵はとげとげしくする。

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| atomF の自機弾と敵の見た目変更 | 寄与なし（矛盾なし） | 関係が最も近いのは「・pngやobj、gltfなど画像や3Dモデル、アニメーションのリソースの作成は外部ツールで行う」。この行に反しないが、タイトル固有の見た目変更であり、この行を含めどの・行の流れも前に進めない |

- ルールどおりコード・アセットは未変更。進めるかどうかをユーザーに確認中。

---

## セッション 2026-11-26 12:31

### 1. 現在の進捗状況と完了したこと

依頼「自機はそのまま、自機の弾は流線形、敵はとげとげしく」を、ユーザー承認の例外（下記 8）として実装した。作業ブランチは切らず `feature/EditorSceneJsonRename` の作業ツリーを編集した。**コミットはしていない。**

- **モデル**（`Project/Assets/Objects/`）
  - `atomFPlayerBullet/` … 流線形の弾。先端は当たり判定の半径の位置、尾は後方へ半径の 2 倍まで伸びる（尾は判定の外）。最大幅は判定直径の 0.58 倍。
  - `atomFEnemy/` … 小さい芯から円錐のとげが 12 本出る形。とげ先が判定の半径。
  - 各フォルダは `.obj`・`.mtl`・`shadeRamp.png`（陰影の濃淡テクスチャ）。
- **表示の仕組み**
  - `CmpMeshSphere` にモデル指定を追加（未指定なら従来どおり球）。自機は変更なし。
  - 新規 `AtomFMeshModelAttach.h/.cpp`。ファイルが無い場合はログを出して球で表示する。
  - モデル名は `atomF/parameters.json` の `player.bullet.modelName`・`enemy.modelName`。
  - 弾は発射方向へ向ける。弾のモデルはシーン入場時に読み込む。
  - 変更: `AtomFPlayerBulletSpawn.cpp`・`SceneBuildUpAtomFPlayerSpawn.cpp`・`SceneBuildUpAtomFEnemySpawn.cpp`・`AtomFParametersRuntime.h/.cpp`・`AtomFParametersAccess.h/.cpp`・`GameAssetsPaths.h`・`CG2_BuildUp.vcxproj`（`.filters` はビルドスクリプトが同期）。
- **依頼に付随して直したもの**
  - 弾の予測表示（`CmpDrawAtomFPlayerBulletPredictCapsule.cpp`）を半透明で描くようにした。不透明の帯が弾の前半分を隠していたため。
  - `ModelStorage::RetainModel` を追加。弾や敵が消えても、描画中の頂点バッファを解放しないようにするため。
  - `AtrumEngine.cpp` の `PostDraw` … 転送用の中間リソースを解放する前に GPU の完了を待つようにした（コードを読んで見つけた解放タイミングの危険。不具合の発生は確認していない）。

### 確認状況

| 項目 | 結果 |
|------|------|
| Development\|x64 ビルド | 成功。ただし**その後の変更**（予測表示の半透明化・濃淡の色の変更）を含む再ビルドはしていない |
| Release\|x64 ビルド | 成功（最後の変更を含む） |
| 画面確認（Release・色変更前） | 敵がとげの形で表示されること、上向きに撃った弾が流線形で表示されること（前半分は帯に隠れていた）を確認 |
| 最終版の色と半透明の帯 | **未確認**（ユーザー指示で画面確認を終了） |
| 上以外へ撃ったときの弾の向き、敵の震え・爆発縮小の見え方 | **未確認** |

- 確認で起動したゲームはすべて終了済み。
- 最初に起動した 1 回は起動直後にプロセスが無くなっていた。原因は未特定（以降の起動では表示できた。Windows のアプリケーションログに該当の記録は見つからなかった）。

### 2. 発生した課題または設計上の選択肢

- **陰影の付け方:** 事前の説明ではライティングを有効にする予定だったが、濃淡テクスチャへの焼き込みに変えた。照明設定（`game_scene_lighting.json` の intensity 8.0）では色が白飛びすると、シェーダーと設定値から判断した（実画面では試していない）。
  - 利点: 照明設定に左右されず、描画コンポーネント側の色指定がそのまま使える。
  - 欠点: ライトを動かしても陰影は変わらない。
- **モデルの作り直し:** 生成に使った Python スクリプトは一時フォルダにあり、リポジトリには入れていない。残さない場合、形の調整は Blender などで OBJ を直接編集する。
- **今回見つけた別件（未修正）**
  - `--smoke-frames` が効いていない可能性が高い。`ResolveReleaseStartupSmokeFrameBudget` は定義と宣言だけで、`Project/` 内に呼び出し元が無い。実行しても 2 分以上終了せず、手動で止めた。`runReleaseStartupSmoke.ps1` は CI（`ReleaseBuild.yml`）からも呼ばれている。
  - Release の案内画像は「次のシーンへ:Space」だが、コード上の決定キーは Enter（`AbstractInput.cpp`）。

### 3. 次に実行すべきステップの提案（指示待ち）

1. ユーザーがゲームを起動して、弾と敵の最終的な見た目を確認する。
2. 問題がなければコミットする（未コミットの別作業が同じ作業ツリーにあるため、分けてコミットする）。
3. 別件のスモーク起動の修正は、指示があれば別タスクで行う。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **atomF の自機弾と敵の見た目変更**
  - 用途: プレイヤーがゲーム中に、自機・弾・敵を形で見分ける
  - 使用場面: ランタイム
  - 用途から決めた仕様: 当たり判定は変えない。敵のとげ先と弾の先端は判定の半径に合わせる。モデルは使い回して撃つたびに読み込まない
- **モデル名の指定（parameters.json）**
  - 用途: 制作者が C++ を変えずに弾や敵の形を差し替える
  - 使用場面: 制作
  - 用途から決めた仕様: `Assets/Objects/<名前>/<名前>.obj` を読む。未指定や読めない場合は球で表示する

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| atomF の自機弾と敵の見た目変更 | 寄与なし（矛盾なし）。**ユーザー承認の例外として実装**（返答「例外で進める」） | 「・pngやobj、gltfなど画像や3Dモデル、アニメーションのリソースの作成は外部ツールで行う」に反しないが、どの・行の流れも前に進めない |

---

## セッション 2026-11-26 12:35

依頼「`--smoke-frames=N` を指定しても終了しない不具合の修正」。項目 8（査定）は着手前に記録した。項目 1〜7 は、このブロックの末尾に追記する。

## 最終目標との査定（§6.7）

| タスク | 判定（有効／矛盾／寄与なし／査定不能） | 根拠（完成形の説明の・行） |
|--------|----------------------------------------|------------------------------|
| `--smoke-frames` が効かない不具合の修正 | 寄与なし（矛盾なし）。**ユーザー承認の例外として実装**（返答「例外で許可する」。依頼元セッション経由で受領） | 「・シーンの構成を保存することと、ゲーム中にエディタ上でのプレイとゲーム配布のランタイム環境上でのプレイを可能にしてある」に反しないが、起動確認の仕組みは、どの・行の流れも前に進めない |

### 1. 現在の進捗状況と完了したこと

- 原因: コミット `4f56e5c`（仮コミット）が `main.cpp` を書き直した際に、`93851ea` で追加した起動スモークの終了処理が消えていた。CLI `--smoke-frames` と環境変数 `ATRUM_RELEASE_SMOKE_FRAMES` の両方が効かなかった。
- 対応: `main.cpp` で `ResolveReleaseStartupSmokeFrameBudget(lpCmdLine)` を呼び、指定フレーム数の描画後にループを抜けるようにした。指定の無い起動では上限なし。**コミットはしていない。**
- 適用: worktree 分離のフックがエージェントの編集を拒否したため、パッチはユーザーが適用した。

| 確認項目 | 結果 |
|------|------|
| Release\|x64 / Development\|x64 ビルド | どちらも成功 |
| スモークスクリプト（90・180 フレーム） | `Smoke OK`・終了コード 0。ただし UTF-8 BOM 付きの一時コピーで実行（下記 2） |
| Release `--smoke-frames=1 / 90 / 600 / 1200` | 終了コード 0。経過 1.3 / 2.4 / 10.9 / 20.9 秒 |
| 空白区切り `--smoke-frames 600`、環境変数のみ | 終了コード 0（約 11 秒） |
| 環境変数と CLI の併用 | CLI を優先（約 2.4 秒） |
| Development `--smoke-frames=90 / 600` | 終了コード 0 |
| 引数・環境変数なしの通常起動 | Release は 3 回とも 40 秒後も動作し、ウィンドウを閉じると終了コード 0。Development は 1 回で同じ結果 |

### 2. 発生した課題または設計上の選択肢

- **スモークスクリプトが Windows PowerShell 5.1 で構文エラーになる（未修正・今回の変更以前から）:** `runReleaseStartupSmoke.ps1` は BOM なし UTF-8 で、コードページ 932 として読まれると 1 行目の日本語コメントで解析が崩れる。CI は `pwsh` を使う。この PC には `pwsh` が見つからなかった。
- **原因未特定:** Release の通常起動の確認で、最初の 1 回は 30 秒以内に終了コード 0 で終了した（経過時間は未記録）。以後の 3 回は再現しない。引数と環境変数が無いときはスモーク処理で終了せず、コード上の終了経路はウィンドウを閉じる操作か `SDL_QUIT` だけ。
- **作業中の事故:** 編集拒否と同時に始めたビルドを中断し、Development の exe が未完成になった。再ビルドで復旧した。

### 3. 次に実行すべきステップの提案（指示待ち）

1. ユーザーが `main.cpp` の差分を確認し、指示があればコミットする（他の未コミット変更と分ける）。
2. `runReleaseStartupSmoke.ps1` を BOM 付き UTF-8 で保存し直す。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **起動スモークの終了処理**
  - 用途: 配布前に、ビルドした exe が起動して描画ループまで進むことを自動で確かめる
  - 使用場面: ビルド・配布
  - 用途から決めた仕様: 引数か環境変数があるときだけ上限フレーム数で終了し、通常のプレイは変えない


---

## atomF タイトル／リザルトシーン（Lua 駆動の SceneBuildUp 化）

### 最終目標との査定（§6.7）

- 判定: 有効・矛盾なし
- 根拠にした `・` 行: 「シーン遷移の条件(処理の呼び出し)をスクリプトに記述して実行することができる」

### 1. 実施内容（ビルド未実施・静的矛盾チェック済み）

- **Lua 基盤（C++）**: これまで Lua からエンジン機能を呼べる binder が 0 件だったため、最小の基本 API を追加した。
  - `EngineLuaBinder`: `Input.anyTrigger/confirmTrigger/upTrigger/downTrigger/stickY`、`SceneFlow.change/isChanging/countByRole/healthByRole`、`Ui.image`
  - `ScriptSceneContext`: 実行中 SceneBuildUp のエンティティ列・returnScene を Lua 側へ橋渡し
  - `ScriptUiSprite`: `Ui.image` が積む 2D スプライトの即時モードキュー。SceneBuildUp が毎フレーム Update 先頭で破棄し、DrawScene 末尾で描画する
- **シーン（JSON）**: `Assets/Scenes/atomFTitle.json` / `atomFResult.json` を新規作成。`atomF.json` に `CmpScriptUpd`（`atomF/game_flow.lua`）を追加。
- **ロジック（Lua）**: `Assets/Scripts/atomF/` に `title_flow.lua`（ロゴ・Press any to start・何かキー／ボタンで本編へ）、`result_flow.lua`（リトライ／タイトルへ・Space/A/Enter で決定）、`game_flow.lua`（敵全滅 or 自機 Hp 0 で 1 秒後にリザルトへ）、`scene_paths.lua`。
- **演出**: 既存 `ChangeSceneFade` の暗転フェードに加え、タイトルは押下後の点滅・ロゴ拡大、リザルトは UI フェードインと決定時の点滅を Lua で実装。
- **画像**: `Assets/Images/atomF/` に白文字 PNG 5 枚（実行時に色を乗算）。IPAGothic / Inter Bold から生成。
- **起動設定**: `launch_config.json` の startupScenePath を `atomFTitle.json` へ変更。`release_scene_carousel.json` から atomF を除外し、`SceneReleaseCarousel::TryAdvanceOnSubmit` は巡回リスト外の BuildUp シーンで Enter/A を横取りしないようにした。
- `CG2_BuildUp.vcxproj` に新規 .cpp/.h を 6 件登録。

### 2. 要確認

- 実機ビルド・動作確認は未実施（Windows/MSVC 環境が必要）。
- atomF 本編の Hp 0／敵全滅は、既存の C++ 同様に「自機の体力は `CmpHealthHit`」前提で判定している。
- Development（エディタ）では起動シーンがタイトルになる。atomF 単体で試すときはエディタから atomF.json を開く。

## 推測した用途（§6.4）

- **Lua 基本 API（Input / SceneFlow / Ui）**
  - 用途: ゲームロジック（シーン遷移条件・メニュー操作・簡易 UI）を C++ に書かず Lua で記述する
  - 使用場面: ランタイムのゲームプレイ
  - 用途から決めた仕様: Update 中に呼ぶ即時モード。毎フレーム呼び出しのコストが小さい関数だけ公開
- **atomF タイトル／リザルト**
  - 用途: 配布するゲームの導線（起動→本編→結果→再挑戦／タイトル）
  - 使用場面: ランタイムのゲームプレイ
  - 用途から決めた仕様: 連打で即決定されないよう入力ロックアウトを設ける

---

## セッション 2026-11-26（atomF 自機 HP・HP アイコン・無敵点滅）

ユーザーの直接依頼（ロードマップ実行ではない）。ブランチは環境指定の `claude/atomf-player-hp-invincibility-bievzk`。**ビルド未実施・静的矛盾チェック済み**（Linux 環境のため MSBuild 不可。`vcxproj` 登録と `vcxproj.filters` 同期、宣言・実装・呼び出し元・include の照合のみ）。

### 1. 実施内容

- 最大 HP を 3 に変更（`Project/Assets/atomF/parameters.json` の `player.initialHealthCount`: 99 → 3）。
- 無敵時間を 1.0 秒に変更。`player.invincibility`（`seconds` / `blinkPulseSeconds` / `blinkPulseCount` / `blinkMinAlpha`）を新設。従来は `damageFeedback.vignetteSeconds`（0.75 秒）を無敵時間に流用していたため、`AttachPlayerHealthHit` の参照先を差し替えた（ビネット表示時間は据え置き）。
- 無敵中の点滅: 新規 `CmpAtomFPlayerInvincibleBlink`。無敵開始 0 秒から 0.3 秒かけて不透明度が 1.0 → 0.2 へ下がる周期を 3 回。最後の 0.1 秒は演出なし（アルファ 1.0）。
- HP アイコン: 新規 `AtomFPlayerHealthIconDraw`。自機球の上端をカメラの viewProjection で画面へ投影し、その真上にハートを最大 HP 個、中央揃えで描く（残り HP は水色、失った分は暗色）。`SceneBuildUp::DrawScene` と `DrawRuntimeViewContent` の atomF 時にビネット直前で呼ぶ。ハート描画は `SceneGameBattleHud` から `ScreenHeartIconDraw` へ切り出して共用した（見た目は変更なし）。
- 敵に当たる: 既存の `CmpHealthHit`（ENEMY / ENEMY_WEAPON タグで被弾）をそのまま使用。
- 爆発に当たる: `CmpAtomFEnemyChainExplosion::BeginDyingExplosion` で `ApplyEnemyExplosionDamageToPlayer` を呼び、爆心から半径c（`enemy.explosion.hitRadiusMeters`）の球が自機の当たりに触れていれば `CmpHealthHit::TryTakeDamageUnconditional()` で HP を 1 減らす。球交差判定は `IsExplosionSphereIntersectingTarget` を公開して敵の連鎖吹き飛ばしと共用。

### 2. 最終目標との査定（§6.7）と方針の例外

- 査定: **寄与なし（矛盾なし）**。根拠は「完成形の説明」のどの `・` 行にも atomF の自機 HP を直接進める行が無いため。ユーザーが明示的に依頼したので実装した。
- `game-logic-via-script.md`: HP・無敵・点滅は本来 Lua で書くゲームロジック。ただし Lua ランタイムが未整備（`Project/Assets/Scripts/README.md` の Phase 2、`Project/Codes` に Lua VM 呼び出しなし）で、atomF の既存挙動もすべて C++ コンポーネントのため、既存パターンに合わせて C++ で実装した。値は `parameters.json` に出し、将来 Lua へ移す際に差し替えやすくしてある。

### 3. 未確認・注意

- 実機での目視確認（点滅の見え方、アイコン位置、爆発の当たり）は未実施。
- 爆発ダメージは爆発の瞬間（連鎖爆発開始時）の 1 回判定。爆発中に居座ってもダメージは続かない。無敵中に爆発に当たってもダメージは入らない。
- 点滅は `CmpMeshSphere` のアルファに依存するため、自機メッシュの描画がアルファ合成されることが前提（敵の消滅演出と同じ方式）。

### 4. 次に実行すべきステップの提案（指示待ち）

1. Windows で `buildDevelopment.cmd` を実行してビルド確認し、atomF を起動して HP・点滅・アイコンを確認する。
2. 問題がなければコミット許可を出す。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **自機 HP・HP アイコン**
  - 用途: トップビュー全方位シューティング atomF で、被弾回数の上限を決めて緊張感を作り、残り HP をプレイ中に一目で読めるようにする
  - 使用場面: ランタイムのゲームプレイ
  - 用途から決めた仕様: アイコンは視線が集まる自機の真上に置く。最大 HP は 3 で、失った分も暗色で残して最大値が分かるようにする
- **被弾後の無敵時間と点滅**
  - 用途: 密集した敵や連鎖爆発での連続被弾を防ぎ、被弾したことと無敵の残りをプレイヤーに伝える
  - 使用場面: ランタイムのゲームプレイ
  - 用途から決めた仕様: 無敵 1 秒のうち前半 0.9 秒だけ点滅し、最後の 0.1 秒は通常表示に戻して「もうすぐ当たる」ことが分かるようにする。値は `parameters.json` で調整できる

## セッション 2026-11-26（atomF 吹き飛び中エフェクト＋エミッタの親子関係）

ユーザーの直接依頼（ロードマップ実行ではない）。ブランチは環境指定の `claude/atomf-knockback-effects-pk9twz`。**ビルド未実施・静的矛盾チェック済み**（Linux 環境のため MSBuild 不可。`vcxproj` 登録、宣言・実装・呼び出し元・include の照合のみ）。

### 1. 実施内容

- エミッタの親子関係: `Visual::Particle::Emitter` に親ワールド行列（`SetParentWorldMatrix` / `ClearParentWorldMatrix`）を追加。ワールド行列 = ローカル変換 × 親行列。発生位置と、WORLD シミュレーション時の初速方向が親の回転に従う（加速度は重力など世界固定の力を想定して回転させない）。親行列なしでは従来と同一。
- `CmpParticleEmitter`（エンティティにアタッチするエミッタ）を拡張: `SetInheritOwnerMatrix`（所有エンティティのワールド行列＝親子チェーン込みを親にする。既定 false で従来の位置のみ追従）、`SetLocalTransform`、`SetEmitEnabled`（CONTINUOUS の発生 ON/OFF）、`SetEmitCircleRadiusOverride`（発生円の半径を実行時に上書き）。エディタのインスペクタに `inheritOwnerMatrix` を追加。スポーン直後にシーンのエミッタ管理が無い場合に備え、Update で未登録なら再登録を試す。
- 新規 `CmpAtomFEnemyKnockbackFx`: 敵の `CmpAtomFEnemyKnockback::HasActiveKnockback()` の間だけ子エミッタを発生させ、後方噴射の向きを「吹き飛び方向の反対」に更新する。子エンティティは Finalize されないため、このコンポーネントの Finalize で子を Finalize する。
- 新規 `Project/Assets/Particles/atomf_knockback_trail.json`（後方噴射）: 吹き飛びと反対へ噴出。発生円は進行方向に垂直（CIRCLE_XY）で、半径は敵の半径＝幅は敵の直径と同程度（スポーン時に `ResolveEnemyBodyRadiusMeters()` で上書き）。色は発生時が黄色、消失時に赤（終端アルファ 0.5、スケールは縮小）。
- 新規 `atomf_knockback_aura.json`（纏い）: 敵の周囲の球内から出る黄色の粒。
- `SceneBuildUpAtomFEnemySpawn.cpp`: 敵ごとに子エンティティ 2 個（`_KnockbackTrail` / `_KnockbackAura`）を `CmpChildEntitys` で追加。

### 2. 最終目標との査定（§6.7）

- 査定: **寄与なし（矛盾なし）**。「完成形の説明」のどの `・` 行にも atomF の演出やエミッタの親子関係を直接進める行が無いため。ユーザーが明示的に依頼したので実装した。
- `game-logic-via-script.md`: Lua ランタイム未整備で、atomF の既存挙動もすべて C++ コンポーネントのため、既存パターンに合わせて C++ で実装した。粒子の見た目は JSON（データ駆動）に出してある。

### 3. 未確認・注意

- 実機での目視確認は未実施（噴出の向き、色の変化、発生範囲の幅）。
- 依頼の「CmpParticle」は、既存のエンティティ用エミッタ `CmpParticleEmitter` に親子機能を足す形で対応した（別クラスは作っていない）。
- 描画は ADD ブレンドのため、赤終端はアルファ 0.5 で暗めの赤に見える。見た目は JSON の `endColor` で調整できる。
- 親のスケールは速度・位置に反映される（敵のスケールは 1 の想定）。

### 4. 次に実行すべきステップの提案（指示待ち）

1. Windows で `buildDevelopment.cmd` を実行してビルド確認し、atomF で敵を吹き飛ばして見た目を確認する。

## 実装済みと判断した項目（コード確認・即時 [x]）

なし

## 保留・着手見送り（§6.3）

なし

## 用途不明で確認待ち（§6.4）

なし

## 推測した用途（§6.4）

- **エミッタの親子関係（`CmpParticleEmitter` の親行列継承）**
  - 用途: 動く／回転するエンティティに付けたパーティクルを、親の向きに合わせて発生・噴射させる（後方噴射、武器の軌跡など）
  - 使用場面: ランタイムのゲームプレイ、レベル制作（エディタでの配置）
  - 用途から決めた仕様: 既存シーンを壊さないため既定は従来どおり位置のみ追従。継承は明示的にオンにする
- **吹き飛び中の後方噴射（黄→赤）と纏いの黄色粒**
  - 用途: 敵が吹き飛んでいる最中であることと、その向き・勢いをプレイヤーに瞬時に伝える
  - 使用場面: ランタイムのゲームプレイ（atomF）
  - 用途から決めた仕様: 吹き飛び中のみ発生させ、止まったら新規発生を止める（既存粒は寿命で消える）。発生幅は敵の直径に合わせる

## セッション 2026-11-26（atomF 雑魚敵バリエーション・/loop 第 1 tick）

ユーザー依頼の `/loop 15m`（優先: 雑魚バリエーション → ボス → 演出。ステージは 1 つ）。ブランチは環境指定の `claude/atomf-game-implementation-g0s0rq`。**ビルド未実施・静的矛盾チェック済み**（Linux 環境のため MSBuild 不可。宣言・実装・呼び出し元・include の照合のみ。新規は `.h` のみで vcxproj 追記不要）。

### 1. 実施内容

- `parameters.json` の `enemy.variants`（配列）で雑魚の種類をデータ駆動に定義。`EnemyVariantParameters`（速度倍率・突進の有無／間隔・蛇行振幅と周波数・色）を追加。全種 Hp1・耐久系能力なし（被弾で爆発消滅する仕様は共通）。
- 新規 `CmpAtomFEnemyVariant`（データのみ）。`CmpAtomFEnemyWalkToPlayer` が速度倍率と蛇行を、`CmpAtomFEnemyPeriodicCharge` が突進の有無・間隔を参照。
- 種類: walker（従来）、runner（1.9 倍速・突進なし・黄）、weaver（1.2 倍速・蛇行・紫）、charger（0.6 倍速・2.5 秒ごと突進・橙）。スポーンは `index % 件数` で割り当て。

### 2. 最終目標との査定（§6.7）

- 査定: **寄与なし（矛盾なし）**。`・` 行に atomF 敵の種類を進める行は無いが、ユーザーが明示依頼。
- `game-logic-via-script.md`: Lua 実行基盤が atomF 側で未使用のため既存パターンに合わせ C++。種類の中身は JSON に出した。

### 3. 次 tick 以降の予定

1. 雑魚: 種類ごとの予備動作・攻撃エフェクト（演出優先度は低いので後回し）、スポーン波（ウェーブ）の用意。
2. ボス（Hp128・アクティブスキル・特殊アクション）。
3. 演出（出現／死亡・シーン遷移・平行光源のみ）。

## 推測した用途（§6.4）

- **雑魚敵の種類（variants）**
  - 用途: 1 ステージを作り込むため、同じ「当たると爆発する玉」でも動きの癖で位置取りと連鎖爆発の狙い方を変えさせる
  - 使用場面: ランタイムのゲームプレイ（atomF）
  - 用途から決めた仕様: 全種 Hp1・耐久なし。差は速度・蛇行・突進間隔・色のみで、調整は JSON で行う

## セッション 2026-11-27（Lua からエンティティ関連を一通り見えるようにする）

ユーザー依頼「Lua 側からエンティティ関連の実装が全て見えるようにしたい」。ブランチは環境指定の `claude/awesome-wright-qtog34`。**ビルド未実施・静的矛盾チェック済み**（Linux 環境のため MSBuild 不可。宣言・実装・include・vcxproj 登録の照合のみ）。

### 1. 実施内容

- 新規 `EntityLuaBinder`（`ILuaBinder`、`LuaJitScriptHost::Initialize` で登録）。公開 API:
  - 値型 `Vec3` / `Quat`（演算子・`length`/`normalized`/`dot`・`fromEuler`/`fromAxisAngle`/`lookRotation`/`slerp`）
  - `Entities.find / findAll / findByTag / findFirstByTag / findByRole / findFirstByRole / all / count / spawn / destroy / componentTypes / tagCatalog`
  - `Entity` ハンドル: 状態（`isValid/state/isActive/id/isNamed/destroy`）、ローカル位置・回転・スケール、ワールド位置、親子（`getParent/setParent/children`）、タグ・ロール、コンポーネント有無・一覧、`CmpHealthHit`（`getHealth/setHealth/damage/...`）
  - `Entities.spawn(prefabPath, opts)`: プレハブ/シーン JSON の先頭エンティティを生成。`opts` で名前・位置・回転・スケール・親・タグ追加・コンポーネントパラメータ上書き（未保有型は既定値で追加）。更新走査中でも安全なよう `AtomF::EnqueueSceneEntityAdopt` 経由でシーンへ採用
- フックに `self` を追加（互換のため末尾引数）: `onResolve(self)` / `onUpdate(dt, self)`
- エンジン側: `EditorSceneRuntime` に `BuildEntityForRuntimeSpawn`、`EntityEditorKnownComponentRegistry::TypeNames()` を追加（いずれも Release 除外なし）
- `Project/Assets/Scripts/_api/entity.d.lua`（LuaLS 型定義）と `Samples/entity_api_sample.lua` を追加
- `CG2_BuildUp.vcxproj` に `EntityLuaBinder.cpp/.h` を登録

### 2. 最終目標との査定（§6.7）

- 査定: **有効（矛盾なし）**。根拠: 「[スクリプト]〇C++との連携・C++側のバインドができていて…」「[エディタ]〇エンティティ・ゲーム内のロジック(スクリプト)によって、保存したエンティティをゲーム内に生成することができる」。
- `game-logic-via-script.md`: C++ 側は公開 API（バインド）のみ。ゲームロジックは Lua 側に残る。

### 3. 制約・未対応（次の候補）

- ハンドルは生ポインタで、参照のたびにシーン所有リスト（と子孫・採用待ち）を線形走査して有効性を確認する。エンティティ数が数千規模なら要最適化（id 採番＋索引化）。
- コンポーネント個別の型付きバインドは `CmpHealthHit` のみ。他コンポーネントは有無確認と spawn 時のパラメータ上書きまで。生きているエンティティへのコンポーネント追加・パラメータ変更は、初期化順の保証が無いため未対応。
- 名前は Hash64 のみ保持のため、エンティティ名文字列の取得は未対応（`isNamed` で照合）。

## 推測した用途（§6.4）

- **Lua からのエンティティ操作 API**
  - 用途: ステージ進行・ウェーブ・ギミックを Lua で書き、敵や弾などをプレハブから生成・探索・破棄・移動させる
  - 使用場面: ランタイムのゲームプレイ（スクリプト）
  - 用途から決めた仕様: 生成は走査後採用（反復中の破壊を避ける）。破棄済みハンドルは安全に無害化（クラッシュさせない）。互換のため `self` は末尾引数

## セッション 2026-11-27（atomF: 雑魚バリエーション拡充 → ボス → 演出、1 ステージ作り込み）

ユーザー依頼（スケジュール実行）: develop 基軸で `game_atomF.md` をもとに atomF を進める。優先順位は「雑魚のバリエーション（全種 Hp1・耐久系なし）→ ボス（Hp128・耐久系パッシブなし・アクティブスキル・特殊アクション）→ 演出（予備動作・攻撃エフェクト・出現/死亡・シーン遷移。ライティングは平行光源のみ）」、ステージは 1 つ。ブランチは環境指定の `claude/bold-lamport-tzno0c`。**ビルド未実施**（Linux 環境のため MSBuild 不可）。clang の `-fsyntax-only`（プラットフォームヘッダはスタブ）で、変更・追加した cpp に自ファイル起因のエラーが無いことのみ確認。cspell は 0 件。

### 1. 実施内容

- **ステージ進行役** `CmpAtomFStageDirector`: `parameters.json` の `stage.waves`（6 ウェーブ。前ウェーブの生存数が閾値以下で前倒し）→ 全滅でボス登場 → ボス戦中は増援（上限 9 体）。旧「起動時に 6 体を固定配置」は廃止。
- **雑魚（全種 Hp1・耐久系なし）**: 既存 walker/runner/weaver/charger に **orbiter**（自機の周りを回りながら円を縮める）と **shooter**（射程で停止→震え＋照準線の予備動作→弾を撃つ）を追加。生成は `AtomFEnemyFactory` に切り出し。
- **雑魚の出現演出** `CmpAtomFEnemySpawnIn`: 床の輪が収束し、球が弾むように実体化（出現中は当たり判定・行動停止）。爆散時の色が赤固定だった不具合を種類色に修正。
- **敵弾** `CmpAtomFEnemyProjectile`（ENEMY_WEAPON + HitSphere。ダメージは既存の自機 CmpHealthHit が付与）。
- **ボス** `CmpAtomFBoss`（Hp128、タグ ENEMY + BOSS）: 登場演出（魔法陣・拡大・揺れ・Hp バー伸長）/ 通常は自機へ接近 / アクティブスキル 3 種（全方位弾・狙い撃ち連射・雑魚召喚、いずれも予備動作あり）/ **特殊アクション**（突進の予告帯 → 突進 → 着地衝撃波 → 硬直。硬直中は被ダメ 1.5 倍）/ Hp 40% 以下で行動間隔短縮（耐久ではなく攻撃の激化）/ 撃破演出（震え・連続爆発・フィナーレ。残った雑魚と敵弾は無害化して爆散）。耐久系パッシブ（軽減・回復・シールド）は無し。
- **ボスへのダメージ**: 自機弾 1 発 = 2、雑魚の爆発 1 回 = 8（爆風に巻き込まれた雑魚がボスに当たって爆発するのが主ダメージ源）。ボスは爆風で吹き飛ばない。
- **演出基盤** `CmpDrawAtomFTelegraph`（床の線・輪の予告表示）、`AtomFBossHealthBarDraw`（画面上部のボス Hp バー。白い残像が遅れて縮む）、`CombatCameraShake::RequestCustomShake`、`PlayNamedOneShotEffect`、パーティクル 8 種（`atomf_*.json`）、SE は既存アセットを役割名で包む `AtomFSe.h`。
- **シーン進行（Lua）** `game_flow.lua`: 敵全滅ではなく「ボスが登場後に居なくなった」ことで撃破判定（クリア 1.8 秒 / 敗北 1.2 秒の余韻 → 暗転 → リザルト）。タグ `BOSS`（role `boss`）を `tagRegistry.json` に追加。
- 数値はすべて `parameters.json`（`boss` / `stage` / `enemy.variants`）。`vcxproj` に新規 cpp/h を登録しフィルターを同期。

### 2. 最終目標との査定（§6.7）

- 査定: **有効（矛盾なし）**。ユーザーが明示依頼した atomF 本編の実装。
- `game-logic-via-script.md`: 進行（クリア/敗北判定・遷移）は Lua のまま。ウェーブ・ボス行動は、Lua から生きたエンティティへコンポーネントを足す／個別コンポーネントを操作する API が未整備（前セッションの報告どおり）のため、既存の atomF の流儀に合わせて C++ コンポーネントにしたが、**数値と構成は JSON に出した**。Lua 側に API が揃ったら進行役・ボスのスキル選択を Lua へ移す余地あり。

### 3. 未確認・次の候補

- **実機での動作確認が未実施**（ビルド・プレイ）。特に: 突進/衝撃波の体感難度、ボス Hp128 に対する爆発ダメージ 8 の釣り合い、増援間隔、パーティクル JSON の見た目、予告線の描画（`DrawLine3DOnFloor` の幅・アルファ）。数値は `parameters.json` だけで調整可能。
- ライティングは既存の平行光源設定（`game_scene_lighting.json`）のまま変更していない。
- 次の候補: 雑魚ごとの攻撃エフェクト差別化、ボス撃破時のスローモーション、勝敗をリザルト画面へ渡す表示、BGM のボス戦切替。

## 推測した用途（§6.4）

- **ボス戦と雑魚の連鎖爆発の結び付け**
  - 用途: 「何かに当たると爆発するビリヤードの玉」というコンセプトを保ったまま、ボス Hp128 を削る主手段を“雑魚をぶつけて爆発させる”にする
  - 使用場面: ランタイムのゲームプレイ（atomF ボス戦）
  - 用途から決めた仕様: 自機弾の直撃は小ダメージ、雑魚の爆発が大ダメージ。ボスは吹き飛ばず、増援・召喚で弾（雑魚）を補給する

## セッション 2026-11-27（atomF: develop 先行実装との統合）

develop に別セッションのボス・ステージ進行役・雑魚追加が先に入っていたため、先の実装（`claude/epic-faraday-6g70v7`）は develop 側を採用して破棄し、特有の要素だけを移植した。**ビルド未実施**（Linux 環境）。

- 移植: クリア／敗北でリザルトの見出しを切り替え（`outcome.lua`・`result_flow.lua`・`Images/atomF/clear.png`・`game_over.png`）、ボス登場とクリア時の白フラッシュ（`game_flow.lua`）。
- 不採用: C++ ボス・敵弾・射撃型・周回型・演出一式（develop の同等実装を使用）、Lua `AtomF.*` バインダ、Lua ウェーブ進行。
- 推測した用途: 勝敗の見出し切替は、同じリザルトシーンで結果を伝えるため。Lua モジュールのキャッシュ（`require`）で本編→リザルトへ値を渡す。

## セッション 2026-11-27（atomF: 本編の画面演出追加・スケジュール実行）

develop 上でボス・雑魚・ステージ進行・主要演出が実装済みだったため、新規 C++ は足さず、ビルド不要で安全な Lua のみ追加した（Linux 環境のためビルド不可）。`game_flow.lua` に、開始時の黒からのフェードイン、自機被弾時の赤フラッシュ、敗北余韻の赤黒い暗転を追加。ライティングは変更なし。

- 未実施: ボス撃破のスローモーション（エンジンにタイムスケール機構が無く、エンジン変更が必要）、雑魚ごとの攻撃エフェクト差別化、ボス戦 BGM 切替。実機での動作確認も未実施。
- 推測した用途: 被弾・敗北の視覚フィードバックを強め、ゲームの手触りを上げるため。

## セッション 2026-11-27（atomF: 雑魚バリエーション追加・スケジュール実行）

雑魚・ボス・主要演出は develop に実装済みのため、ビルド不要の JSON のみ追加した（Linux 環境でビルド不可）。`parameters.json` の `enemy.variants` に **rusher**（高速・短間隔突進）/ **drifter**（大きくゆっくり蛇行）/ **sniper**（遠距離・長い予備動作・高速弾）を追加し、ウェーブ 5〜7 に配置（ウェーブ 7 を新設）。全種 Hp1・耐久系なし（ファクトリが固定）。

- 未実施: 実機確認、雑魚ごとの攻撃エフェクト差別化、ボス撃破スローモーション（タイムスケール機構が無くエンジン変更が必要）、ボス戦 BGM。
- 推測した用途: 既存の挙動部品の組み合わせだけで 1 ステージの密度と緩急を上げるため。

## セッション 2026-11-27（atomF: 雑魚の攻撃エフェクト差別化・スケジュール実行）

雑魚・ボス・主要演出は develop に実装済み（企画ファイルは `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md`）。残課題のうち「雑魚ごとの攻撃エフェクト差別化」として、射手型（shooter/sniper）の敵弾と予告線の色を `variants[].color` に追従させた（`CmpAtomFEnemyShooter`。従来は赤橙固定）。**ビルド未実施**（Linux 環境）。ヘッダ宣言・実装の整合のみ静的確認。

- 未実施: 実機確認、ボス戦 BGM 切替（`AtomFSe` 周りの BGM API 要確認）。ボス撃破スローモーションはタイムスケール機構が無くエンジン変更が必要なため保留。
- 推測した用途: 複数種の射手が同時に居る場面で、色だけで種類と脅威を読み取れるようにするため。

## セッション 2026-11-27（atomF: ボス Hp ゲージ追加・スケジュール実行）

雑魚・ボス・主要演出は develop に実装済みのため、ビルド不要の Lua のみ追加した（Linux 環境でビルド不可）。`game_flow.lua` に、ボス登場でフェードインし撃破でフェードアウトする画面上部の Hp ゲージ（白 4x4 画像を 32 分割で並べた一様スケールのセグメントバー。Hp 40% 以下で赤く脈動）を追加。`Ui.image` は一様スケールのみのため分割バーにした。ゲージの最大 Hp は `boss.maxHealth`（128）に合わせて Lua 側に定数で持つ。

- 未実施: 実機確認（セグメント間隔・サイズは解像度依存のため要調整）、ボス戦 BGM 切替、ボス撃破スローモーション（タイムスケール機構が無くエンジン変更が必要）。
- 推測した用途: 128 という大きい Hp の削れ具合と、狂暴化（Hp 40% 以下）への移行を視覚的に伝えるため。

## セッション 2026-11-27（atomF: 狂暴化の画面演出・クリア白転・スケジュール実行）

雑魚・ボス・主要演出は develop に実装済みのため、ビルド不要の Lua のみ追加した（Linux 環境でビルド不可）。`game_flow.lua` に、ボス Hp 40% 以下の間の薄い赤の画面脈動と、クリア余韻後半の白転（リザルトへの遷移を滑らかに）を追加。`BOSS_MAX_HEALTH` 定数を上方へ移動（参照順の都合）。**実機確認未実施**。

- 未実施: ボス戦 BGM 切替、ボス撃破スローモーション（タイムスケール機構が無くエンジン変更が必要）。
- 推測した用途: 終盤の緊張感とクリアの達成感を、画面全体の色で伝えるため。

## セッション 2026-11-27（atomF: ボス登場の警告帯・スケジュール実行）

develop 上で雑魚・ボス・主要演出は実装済みのため、ビルド不要の Lua のみ追加した（Linux 環境でビルド不可）。`game_flow.lua` に、ボス登場時の画面上下の赤い警告帯（伸長→点滅→フェードアウト、約 2.4 秒）を追加。**実機確認未実施**（帯の太さは解像度依存のため要調整）。

- 未実施: ボス戦 BGM 切替・ボス撃破スローモーション（いずれもエンジン変更が必要）。
- 推測した用途: ボス登場を予告し、緊張感を高めるため。

## セッション 2026-11-27（atomF: シネマ帯演出・スケジュール実行）

雑魚（9 種・全 Hp1）・ボス（Hp128）・主要演出は develop に実装済みのため、ビルド不要の Lua のみ追加した（Linux 環境でビルド不可）。`game_flow.lua` に、開始時に上下の黒帯が引いていく演出と、ボス撃破（クリア余韻）で黒帯が寄ってくる演出を追加。**実機確認未実施**（帯の太さは解像度依存）。

- 未実施: ボス戦 BGM 切替、ボス撃破スローモーション（いずれもエンジン変更が必要）。
- 推測した用途: シーン遷移の緩急と、ボス撃破の「決着感」を高めるため。

## セッション 2026-11-27（atomF: 残課題の棚卸し・スケジュール実行）

develop を確認したところ、雑魚 9 種（全 Hp1・耐久系なし）・ボス（Hp128・アクティブスキル・ダッシュ衝撃波）・登場/撃破/狂暴化/シネマ帯/警告帯/フェード等の演出・タイトル/リザルト遷移は実装済みで、ビルド不要の Lua/JSON で足せる項目は尽きていた。コードの変更は行っていない（Linux 環境でビルド不可のため、未検証の C++ は足さない方針）。

- 残課題（エンジン変更が必要）: ボス撃破スローモーション（`Script`/ゲームループにタイムスケール機構が無い。`SceneFlow` バインダに時間倍率 API を足し、更新 dt に乗せる必要あり）、ボス戦 BGM 切替。ビルドできる環境で着手すること。
- 補足: 企画書は `.claude/ideas/` ではなく `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` にある。

## セッション 2026-11-27（atomF: ボス行動・雑魚バリエーション追加・スケジュール実行）

develop 上の実装を基に、ボスに新スキル **スパイラル**（回転する対向 2 方向の弾幕。予備動作は回る 3 本の腕）を追加し、狂暴化（Hp 40% 以下）後は専用の行動順（スパイラルを織り込み、突進を増やす）に切り替えるようにした（`CmpAtomFBoss`）。雑魚は **sentinel**（遠い軌道を回る）/ **dasher**（短間隔突進）を `parameters.json` に追加し、ウェーブ 8 を新設。全種 Hp1・耐久系なし。**Linux 環境のためビルド未実施**。C++ は既存パターンの踏襲で静的確認のみ（要ビルド確認）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（タイムスケール機構が無くエンジン変更が必要）。演出の更なる複雑化（次の候補: スパイラル専用パーティクル、雑魚の死亡演出の種類別化）。
- 推測した用途: 狂暴化で攻撃パターン自体が変わることで、ボス戦後半の緊張感と再挑戦時の学習要素を作るため。

## セッション 2026-11-27（atomF: 雑魚出現リングの種類色化・スケジュール実行）

develop を基に、雑魚の実体化前リング（`CmpAtomFEnemySpawnIn`）を `variants[].color` に寄せた色にし、出現前に種類が分かるようにした（従来は水色固定）。雑魚 11 種（全 Hp1・耐久系なし）・ボス（Hp128）・主要演出は実装済み。**Linux 環境のためビルド未実施**（C++ は既存コンポーネントの API のみ使用、静的確認のみ）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 次の候補: スパイラル専用パーティクル、雑魚の死亡演出の種類別化。

## セッション 2026-11-27（atomF: 雑魚の死亡演出の種類別化・スケジュール実行）

develop を基に、雑魚の爆散時に床へ広がる輪を種類別にした（`CmpAtomFEnemyChainExplosion::UpdateDeathRings`）。射手系は 3 重の波紋、周回系は外へ広がる輪＋内へ縮む白輪、高速系は大きく素早い 1 重輪、その他は種類色の輪＋白い核の輪。色は `variants[].color`。全種 Hp1・耐久系なし。**Linux 環境のためビルド未実施**（既存の `CmpDrawAtomFTelegraph` API のみ使用、宣言・実装の整合は静的確認）。

- 未実施: 実機確認、スパイラル専用パーティクル、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 連鎖爆発で多数の雑魚が同時に散る場面でも、どの種類が爆ぜたかを一目で読めるようにするため。

## セッション 2026-11-27（atomF: ボス召喚バリエーション追加・スケジュール実行）

develop を基に、ボスの雑魚召喚を 3 種に増やした（`CmpAtomFBoss`）。従来の散開に加え、**囲み召喚**（自機の周囲 5 点へ shooter/sniper。予告は黄）と **両翼召喚**（ボスの両脇へ縦隊で rusher/dasher。予告は赤橙）を新設。予告はボスから出現点へ伸びる細線を追加、出現時は各地点に輪のバーストを再生。通常行動順に両翼召喚、狂暴化行動順に囲み召喚と両翼召喚を織り込んだ。**Linux 環境のためビルド未実施**（既存 API のみ使用、switch 網羅・include を静的確認）。

- 未実施: 実機確認、近接攻撃バリエーション（薙ぎ払い等）、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）、スパイラル専用パーティクル。
- 推測した用途: 弾幕の代わりに「召喚する種類と位置」で戦況を変え、ボス戦の読み合いを増やすため。

## セッション 2026-11-27（atomF: スパイラル専用パーティクル・スケジュール実行）

develop を基に、ボスのスパイラル弾幕専用のオレンジ色バースト（`atomf_boss_spiral_burst.json`）を追加し、`FireSpiralShot` のマズル演出を通常の `atomf_shot_muzzle.json` から差し替えた。**Linux 環境のためビルド・実機確認は未実施**（C++ は文字列 1 箇所のみの変更）。

- 未実施: 近接攻撃バリエーション（薙ぎ払い等）、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。

## セッション 2026-11-27（atomF: 隙間リング・ジグザグ雑魚追加・スケジュール実行）

develop を基に、ボスの新スキル **隙間リング**（自機の向きから 90° ずれた一方向だけ弾を空けた全方位弾。予備動作は弾線＋隙間を示す緑の太線）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **zigzagger**（細かく蛇行しつつ周期突進。Hp1・耐久系なし、`parameters.json` のみ）を追加しウェーブ 7 に配置。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装・include を静的確認）。企画書の所在は `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md`（`.claude/ideas/` には無い）。

- 未実施: 実機確認、近接攻撃バリエーション（薙ぎ払い等）、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 「動いて隙間へ入る」回避を求める弾幕を足し、既存の直進回避との差を出すため。

## セッション 2026-11-27（atomF: ボス薙ぎ払い・skirmisher 追加・スケジュール実行）

develop を基に、ボスの新スキル **薙ぎ払い**（自機方向を中心とした約 120° の扇へ弾列を振る。左右どちらから振るかは毎回ランダム。予備動作は扇の両端線と弧、振り始め側を強調）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **skirmisher**（蛇行しながら距離を保って連射する射手。Hp1・耐久系なし、`parameters.json` のみ）を追加しウェーブ 7 に配置。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装を静的確認）。企画書の所在は `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md`。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 全方位・直線・渦とは異なる「扇を横へ抜ける」回避を求め、ボス戦の読み合いを増やすため。

## セッション 2026-11-27（atomF: ボス跳躍叩きつけ・hunter/lurcher 追加・スケジュール実行）

develop を基に、ボスの近接スキル **跳躍叩きつけ**（`Skill::Slam` / `State::Leap`。予備動作で自機の足元に縮む輪と十字を表示し、確定後は赤く太くなる。確定位置へ放物線で跳び、着地点に小さな衝撃波＋着地パーティクル、短めの硬直）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **hunter**（周回しながら速射する射手）と **lurcher**（ゆっくり蛇行し周期的に長く突進）を `parameters.json` に追加しウェーブ 9 に配置（全種 Hp1・耐久系なし）。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装・既存 API のみ使用を静的確認）。

- 未実施: 実機確認、ボス戦 BGM 切替。
- 推測した用途: 弾幕・召喚に偏らない近接の読み合い（落下点を見て離れる）を増やすため。

## セッション 2026-11-27（atomF: ボス扇弾3連・swooper/dancer 追加・スケジュール実行）

develop を基に、ボスの新スキル **扇弾3連**（`Skill::Fan`。自機狙いの 7 発扇を毎波狙い直して 3 波、奇数波は 6 発で隙間をずらす。予備動作は扇の両端と中央の黄緑線、確定後に中央線が太くなる。専用パーティクル `atomf_boss_fan_burst.json`）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **swooper**（蛇行しつつ短周期で突進）と **dancer**（大きく蛇行しながら距離を保って射撃）を `parameters.json` に追加し最終ウェーブへ配置（全種 Hp1・耐久系なし）。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装を静的確認）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 一直線の連射より広く、毎波狙い直す扇で「横へ抜ける」判断を増やすため。

## セッション 2026-11-27（atomF: 雑魚 phantom/gunner 追加・スケジュール実行）

develop を基に、雑魚 **phantom**（大きく速く蛇行しつつ周期突進）と **gunner**（遠距離から速射）を `parameters.json` に追加し最終ウェーブへ配置した（全種 Hp1・耐久系なし、データのみ・JSON 構文は検証済み）。**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 最終ウェーブの密度を上げ、ボス前の総仕上げの難度を作るため。

## セッション 2026-11-27（atomF: ボス風車・warden/bruiser 追加・スケジュール実行）

develop を基に、ボスの新スキル **風車**（`Skill::Pinwheel`。4 本の腕で回転しながら撃ち、弾数の半分で回転方向が反転する。予備動作は 4 本の水色の腕と外周リング、確定後に腕が太くなる）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **warden**（外周を周回しつつ射撃）と **bruiser**（遅いが短周期で突進）を `parameters.json` に追加し最終ウェーブへ配置（全種 Hp1・耐久系なし、JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装を静的確認）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 腕の間を抜けるだけでは避けきれない反転弾幕でボス戦の読み合いを増やし、雑魚は最終ウェーブの密度を上げるため。

## セッション 2026-11-27（atomF: stalker 追加・スケジュール実行）

develop を基に、雑魚 **stalker**（周回しつつ長い予備動作の高速弾を撃ち、周期的に突進も混ぜる。Hp1・耐久系なし、`parameters.json` のみ）を追加しウェーブ 9 に配置。JSON 構文のみ検証、**ビルド・実機確認は未実施**（Linux 環境）。

- 未実施: ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。

## セッション 2026-11-27（atomF: marksman/flitter 追加・スケジュール実行）

develop を基に、雑魚 **marksman**（遠距離から長い予備動作の高速単発）と **flitter**（細かく速く蛇行する突進なしの高速雑魚）を `parameters.json` に追加し最終ウェーブへ配置した（全種 Hp1・耐久系なし、データのみ・JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。

## セッション 2026-11-27（atomF: ボス十字砲火・circler/volleyer 追加・スケジュール実行）

develop を基に、ボスの新スキル **十字砲火**（`Skill::Crossfire`。自機の向きから半ピッチずらした 8 方向へ 5 波、波ごとに半ピッチずらして市松状の隙間を作る。予備動作は 8 本の桃色の放射線と収束する外周リング、確定後に線が太く濃くなる）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **circler**（外周を大きく周回しつつ周期突進）と **volleyer**（蛇行しながら短間隔で連射）を `parameters.json` に追加し最終ウェーブへ配置（全種 Hp1・耐久系なし、JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装を静的確認）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 全方位弾の変種として「隣り合う弾の間へ入る」判断を求め、ボス戦の読み合いを増やすため。

## セッション 2026-11-27（atomF: ボス包囲収束・pacer/lancer 追加・スケジュール実行）

develop を基に、ボスの新スキル **包囲収束**（`Skill::Converge`。外周の 9 点から自機の現在位置へ向けて弾を撃つ 3 波。波ごとに自機を狙い直し、奇数波は発射点を半ピッチずらす。予備動作は発射点の紫の輪と自機へ伸びる線、確定後に線が太くなる）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **pacer**（周回しつつ蛇行し周期突進）と **lancer**（遠距離から長い予備動作の高速単発）を `parameters.json` に追加しウェーブ 6 に配置（全種 Hp1・耐久系なし、JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装を静的確認）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 既存の「ボス中心から放射」型と異なり、外周から収束する弾で位置取りの読み合いを増やすため。

## セッション 2026-11-27（atomF: ボス退路封鎖召喚・skulker/burster 追加・スケジュール実行）

develop を基に、ボスの召喚バリエーション **退路封鎖召喚**（`Skill::SummonCutoff`。自機の背後＝ボス→自機の延長線上に横一列 5 体を召喚し、後退を塞ぐ。種類は lurcher / bruiser の突進型。予備動作は紫の輪とボスからの線）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **skulker**（小さく周回しつつ短周期で突進）と **burster**（蛇行しつつ短い予備動作の速射）を `parameters.json` に追加し最終ウェーブへ配置（全種 Hp1・耐久系なし、JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**（switch 網羅・宣言/実装を静的確認）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 召喚の「位置」の違いで、前進・後退・横移動のどれが安全かを毎回考えさせ、ボス戦の読み合いを増やすため。

## セッション 2026-11-27（atomF: ボス二重螺旋・tumbler/sentry 追加・スケジュール実行）

develop を基に、ボスの新スキル **二重螺旋**（`Skill::Helix`。時計回り（緑）・反時計回り（橙）の 2 組の腕が交差しながら広がる弾幕。予備動作は逆向きに回り始める 2 組の腕と外周リング、確定後に線が太くなる）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **tumbler**（大きく速く蛇行しつつ周期突進）と **sentry**（狭い軌道を周回しつつ短い予備動作で連射）を `parameters.json` に追加し最終ウェーブへ配置（全種 Hp1・耐久系なし、JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**（switch 3 箇所の網羅を静的確認）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 逆回転の 2 系統を同時に読ませ、風車（反転型）とは別の避け方を求めるため。

## セッション 2026-11-27（atomF: glider/turret 追加・スケジュール実行）

develop を基に、雑魚 **glider**（大きくゆっくり蛇行する高速接近・突進なし）と **turret**（ほぼ動かず短間隔で射撃）を `parameters.json` に追加しウェーブ 9 に配置した（全種 Hp1・耐久系なし、データのみ・JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 推測した用途: 動かない射手と大きく動く接近型を混ぜ、狙う優先順位を考えさせるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス残響・lurker/spitter 追加・スケジュール実行）

develop を基に、ボスの新スキル **残響**（`Skill::Echo`。波ごとに速さが上がる 14 方向リングを 5 連発し、後続の波が前の波に追いつく弾幕。予備動作は 3 重に広がる水色リング、確定後に濃くなる）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **lurker**（遠めから縮む軌道で近づき低頻度で突進）と **spitter**（蛇行しつつ短い予備動作で細かく連射）を `parameters.json` に追加し最終ウェーブへ配置（全種 Hp1・耐久系なし、JSON 構文検証済み）。**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替。
- 推測した用途: 速度差で「待つ／抜ける」の判断を迫り、既存の等速弾幕と違う読み合いを作るため。

## セッション 2026-11-27（atomF: ボス連続突進・ravager/snapper 追加・スケジュール実行）

develop を基に、ボスの近接スキル **連続突進**（`Skill::Rampage`。自機を狙い直しながら 3 回突進する。途中の再予告は 0.45 秒と短く、最後の 1 回だけ既存の衝撃波を出す。予備動作は既存の突進と同じ赤い帯）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ織り込んだ。雑魚は **ravager**（短い間隔で突進を繰り返す）と **snapper**（遠い外周から短い予備動作で低速弾を連射）を `parameters.json` に追加し最終ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替。
- 推測した用途: 単発突進だけだった近接攻撃に連続性を与え、動き続けて避ける判断を求めるため。

## セッション 2026-11-27（atomF: ボス花弁・wisp/mirage 追加・スケジュール実行）

develop を基に、ボスの新スキル **花弁**（`Skill::Bloom`。5 枚の花弁（各 3 発の小扇）を 6 波放ち、波ごとに花弁の向きを回して咲かせる弾幕。予備動作は 5 本の桃色の軸線と縮む外周リング、確定後に線が太くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **wisp**（周回しつつ細かい弾を高頻度で撃つ）と **mirage**（周回・突進・低頻度の射撃を併せ持つ）を `parameters.json` に追加し最終ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**ビルド・実機確認は未実施**（Linux 環境）。

- 未実施: 実機確認、ボス戦 BGM 切替。
- 推測した用途: 回転する小扇の弾幕で、移動方向を波ごとに切り替える判断を求めるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス格子・drone/seeker 追加・スケジュール実行）

develop を基に、ボスの新スキル **格子**（`Skill::Lattice`。速さの異なる 2 層のリング（遅い青・速い水色、半ピッチずらし）を 5 波交互に放ち、弾同士の間に格子状の隙間を作る弾幕。予備動作は外へ広がる 2 重リング、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **drone**（高速周回しつつ細かい弾を撃つ）と **seeker**（距離を保ち突進と射撃を併せ持つ）を `parameters.json` に追加し最終ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**ビルド・実機確認は未実施**（Linux 環境）。

- 未実施: 実機確認、ボス戦 BGM 切替。
- 推測した用途: 速度差のある 2 層リングで、弾の隙間を抜ける位置取りと間合いの判断を求めるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス振り子・swayer/pouncer 追加・スケジュール実行）

develop を基に、ボスの新スキル **振り子**（`Skill::Pendulum`。自機方向を中心に弾列が左右へ約 1.5 往復しながら 30 発連射される弾幕。予備動作は振れ幅の両端と中央の橙の線＋円弧、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **swayer**（大きく横揺れしつつ距離を保って射撃）と **pouncer**（周回しながら短間隔で突進）を `parameters.json` に追加し最終ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替。
- 推測した用途: 連射の向きが時間で動く弾幕で、先読みと待ちの判断を求めるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス前衛召喚・cadet/knight 追加・スケジュール実行）

develop を基に、ボスの召喚スキル **前衛召喚**（`Skill::SummonVanguard`。ボスと自機の間に V 字の 3 列隊形で雑魚を召喚し、前進してくる壁を作る。予備動作は既存の召喚と同じ出現点リングで緑色）を `CmpAtomFBoss` に追加し、通常・狂暴化の両行動順へ組み込んだ。雑魚 **cadet**（距離を保ち横揺れしつつ連射）と **knight**（周回しながら突進と射撃を併用）を `parameters.json` に追加し最終ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替。
- 推測した用途: 召喚の配置を「囲む/両翼/背後」に加え「正面の壁」にし、押し返される圧を作るため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス弾幕の専用バースト演出・スケジュール実行）

develop を基に、ボスの十字砲火・残響・花弁・格子のマズル演出を共用の `atomf_boss_ring_burst.json` から専用パーティクル（`atomf_boss_crossfire_burst` / `atomf_boss_echo_burst` / `atomf_boss_bloom_burst` / `atomf_boss_lattice_slow` / `atomf_boss_lattice_fast`）へ差し替えた（C++ は各 Fire*Wave の文字列のみ変更。格子は速い層/遅い層で色を分ける）。雑魚・ボス本体の仕様は変更なし。**Linux 環境のためビルド・実機確認は未実施**（JSON 構文のみ検証）。

- 未実施: 実機確認、ボス戦 BGM 切替、ボス撃破スローモーション（エンジン変更が必要）。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス緞帳・bulwark/streaker 追加・スケジュール実行）

develop を基に、ボスの新スキル **緞帳**（`Skill::Curtain`。自機方向へ横一列 13 発の弾の壁を 5 波連続で放ち、波ごとに隙間（幅約 3m）の位置を中央→右→左→さらに外へずらす弾幕。予備動作は壁の幅を示す 2 本の縦線と迫る横線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **bulwark**（大きな遅い弾を撃つ遠距離型）と **streaker**（高速横揺れ＋突進）を `parameters.json` に追加した（全種 Hp1・耐久系なし）。最終ウェーブが肥大化しているため、新雑魚は第 7 ウェーブ（skirmisher の波）へ配置した。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替。最終ウェーブの敵数過多（約 60 体）の再配分は要レビュー。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス散弾・最終ウェーブ分割・スケジュール実行）

develop を基に、ボスの新スキル **散弾**（`Skill::Scatter`。自機方向へ角度（±約 24°）と速さ（1.8〜3.8m/s）がばらつく 11 発を一度に撒く散弾を 4 連続で放つ。ばらつきは弾番号から決まる擬似乱数で再現性あり。予備動作は散らばりの両端と中央を示す 3 本の橙の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。あわせて前回報告で懸案だった **最終ウェーブの敵数過多（61 体）を 4 ウェーブ（各約 15 体、待機 12〜14 秒）に分割**した（雑魚の種類・数は変更なし、全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替、分割後のウェーブ難度バランス確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-28（atomF: ボス三日月・crescenter/hurdler 追加・スケジュール実行）

develop を基に、ボスの新スキル **三日月**（`Skill::Crescent`。自機方向へ幅約 126° の円弧状の弾列（15 発）を 4 波連続で放ち、波ごとに中央の速さを変えて円弧の形をずらす弾幕。予備動作は円弧の両端と中央の線＋端を結ぶ折れ線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **crescenter**（距離を保ち横揺れしつつ射撃）と **hurdler**（高速周回しつつ短間隔で突進）を `parameters.json` に追加し第 7 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替、第 7 ウェーブの難度確認。
- 推測した用途: 広い円弧の弾幕で「端へ逃げず中央の速い弾の間を抜ける」判断を求めるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-28（atomF: ボス驟雨・drizzler/stomper 追加・スケジュール実行）

develop を基に、ボスの新スキル **驟雨**（`Skill::Downpour`。外周のランダムな位置から、発射のたびに自機の現在位置（±0.8m のばらつき）を狙い直した水色の弾を 0.12 秒間隔で 26 発降らせる弾幕。予備動作は外周を縁取る輪と自機へ狭まる輪、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **drizzler**（距離を保ち横揺れしつつ速い小弾を撃つ）と **stomper**（周回しつつ短間隔で突進）を `parameters.json` に追加し第 12 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替、第 12 ウェーブの難度確認。
- 推測した用途: 全方位から時間差で飛来する弾で、自機の位置取りを継続的に動かさせるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-27（atomF: ボス挟撃・flanker/pincerer 追加・スケジュール実行）

develop を基に、ボスの新スキル **挟撃**（`Skill::Pincer`。自機の方位から左右 ±90° の外周を発射点に、交互に自機の現在位置（±0.5m のばらつき）を狙う桃色の弾を 0.14 秒間隔で 24 発撃ち込む弾幕。予備動作は左右 2 つの発射点から自機へ伸びる線と狭まる輪、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **flanker**（周回しつつ横揺れ突進）と **pincerer**（距離を保ち横揺れしつつ射撃）を `parameters.json` に追加し第 12 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: 実機確認、ボス戦 BGM 切替、第 12 ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。

## セッション 2026-11-28（atomF: ボス回転木馬・rotator/whirler 追加・スケジュール実行）

develop を基に、ボスの新スキル **回転木馬**（`Skill::Carousel`。外周の発射点が自機の方位を起点に 1 周しながら、発射のたびに自機の現在位置を狙う緑の弾を 0.1 秒間隔で 30 発撃つ弾幕。予備動作は外周を縁取る輪・起点から自機へ伸びる線・狭まる輪、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **rotator**（周回しつつ突進）と **whirler**（距離を保ち横揺れしつつ射撃）を `parameters.json` に追加し第 12 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: ビルド、実機確認、ボス戦 BGM 切替、第 12 ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128b`（develop から作成）。

## セッション 2026-11-28（atomF: ボス護衛召喚・escort/retainer 追加・スケジュール実行）

develop を基に、ボスの召喚スキル **護衛召喚**（`Skill::SummonGuard`。ボス自身の周囲 2.6m の輪に雑魚 6 体を等間隔で召喚し、本体への接近を阻む。予備動作は既存の召喚と同じ出現点リングで桃色）を `CmpAtomFBoss` に追加し、通常・狂暴化の両行動順へ組み込んだ。雑魚 **escort**（近距離を周回しつつ突進と射撃を併用）と **retainer**（近めの距離を保ち横揺れしつつ連射）を `parameters.json` に追加し、肥大化した最終ウェーブを避けて第 5 ウェーブ（hunter の波）へ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: ビルド、実機確認、ボス戦 BGM 切替、該当ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128c`（develop から作成）。

## セッション 2026-11-28（atomF: ボス潮汐・tidal/ebber 追加・スケジュール実行）

develop を基に、ボスの新スキル **潮汐**（`Skill::Tide`。外周の発射点が自機の方位を中心に ±約 70° を 1.5 往復しつつ、予告確定時点の自機位置へ固定の狙いで水色の弾を 0.11 秒間隔で 28 発撃ち込む弾幕。予備動作は往復範囲の両端と中央から狙い位置へ伸びる線と狭まる輪、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **tidal**（周回しつつ突進）と **ebber**（距離を保ち横揺れしつつ射撃）を `parameters.json` に追加し第 12 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: ビルド、実機確認、ボス戦 BGM 切替、第 12 ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128d`（develop から作成）。

## セッション 2026-11-28（atomF: ボス稲妻・bolter/zapper 追加・スケジュール実行）

develop を基に、ボスの新スキル **稲妻**（`Skill::Zigzag`。自機方向へ弾列を三角波で左右（±約 40°）に折り返しながら 32 発・0.09 秒間隔で撃ち、ジグザグの弾の道を作る。予備動作は折り返しを示す黄色のジグザグ線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **bolter**（高速横揺れ突進）と **zapper**（距離を保ち速い小弾を短間隔で撃つ）を `parameters.json` に追加し第 12 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**（switch・宣言/実装を静的確認）。

- 未実施: ビルド、実機確認、ボス戦 BGM 切替、第 12 ウェーブの難度確認。
- 推測した用途: 向きが急反転する弾列で、直線的な回避を封じ小刻みな位置調整を求めるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128e`（develop から作成）。

## セッション 2026-11-27（atomF: ボス彗星・meteor/stargazer 追加・スケジュール実行）

develop を基に、ボスの新スキル **彗星**（`Skill::Comet`。自機方向へ ±約 17° の 3 本の高速な橙の弾の尾を 0.1 秒間隔で 24 回同時発射し、直線的な逃げ道を塞ぐ。予備動作は 3 本の進路線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **meteor**（周回しつつ突進）と **stargazer**（距離を保ち横揺れしつつ射撃）を `parameters.json` に追加し第 12 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**（switch・宣言/実装を静的確認）。

- 未実施: ビルド、実機確認、ボス戦 BGM 切替、第 12 ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `work/atomf-1127-auto`（develop から作成）。

## セッション 2026-11-28（atomF: ボス滝・rapids/weir 追加・スケジュール実行）

develop を基に、ボスの新スキル **滝**（`Skill::Cascade`。自機方向へ平行な 7 本の弾の列を 0.38 秒間隔で 14 波放ち、1 本だけ抜ける隙間の列が三角波で左右に往復する。予備動作は平行な 7 本の青緑の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **rapids**（周回しつつ突進）と **weir**（距離を保ち横揺れしつつ射撃）を `parameters.json` に追加し第 12 ウェーブへ配置した（全種 Hp1・耐久系なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**。

- 未実施: ビルド、実機確認、ボス戦 BGM 切替、第 12 ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128f`（develop から作成）。

## セッション 2026-11-28（atomF: ボス砂時計・sandglass/dusker 追加・スケジュール実行）

develop を基に、ボスの新スキル **砂時計**（`Skill::Hourglass`。自機方向とその正反対へ 5 発ずつの扇を同時に 16 波放ち、扇の中心が波ごとに左右へ交互に振れて括れを作る紫の弾幕。予備動作は両扇の端を示す紫の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の両行動順へ組み込んだ。雑魚 **sandglass**（周回突進）/ **dusker**（距離を保つ射撃）を `parameters.json` に追加し最終ウェーブへ各 2 体加えた（いずれも最大 Hp1・耐久能力なし）。

- 未実施: ビルド、実機確認、ボス戦 BGM 切替、最終ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128g`（develop から作成）。

## セッション 2026-11-27（atomF: ボス五芒星・starling/astral 追加・スケジュール実行）

develop を基に、ボスの新スキル **五芒星**（`Skill::Pentacle`。5 方向へ速さの異なる 3 発ずつの弾列を 0.4 秒間隔で 12 波放ち、波ごとに 36° 回して星形の隙間を作る黄色の弾幕。予備動作は初波の 5 本の腕を示す黄色の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し、通常／狂暴化の行動順へ組み込んだ。雑魚 `starling`（突進型）・`astral`（射撃型）を `parameters.json` に追加し最終ウェーブへ各 2 体配置（いずれも最大 Hp1・耐久能力なし）。

- 未実施: ビルド（Windows/DirectX 環境外のため）、実機確認、最終ウェーブの難度確認。
- 推測した用途: 回転する放射状の弾列で、安全地帯が波ごとに移動する状況判断を求めるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1127-d`（develop から作成）。

## セッション 2026-11-27（atomF: ボス六花・frostling/glacier 追加・スケジュール実行）

develop を基に、ボスの新スキル **六花**（`Skill::Snowflake`。6 方向へ中央 1 発＋左右の枝 2 発（±約 16°）を 0.45 秒間隔で 10 波放ち、波ごとに 15° 回して雪の結晶状の水色の弾幕を作る。予備動作は初波の 6 本の腕を示す水色の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の行動順へ組み込んだ。雑魚 `frostling`（突進型）・`glacier`（射撃型）を `parameters.json` に追加し最終ウェーブへ各 2 体配置（いずれも最大 Hp1・耐久能力なし）。JSON 構文のみ検証、**Linux 環境のためビルド・実機確認は未実施**（switch・宣言/実装を静的確認）。

- 未実施: ビルド、実機確認、最終ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1127-develop`（develop から作成）。

## セッション 2026-11-27（atomF: ボス風車・windmill/vane 追加・スケジュール実行）

develop を基に、ボスの新スキル **風車**（`Skill::Windmill`。4 本の腕が回りながら 0.09 秒間隔で 36 波・橙の弾を連射し、中盤で回転方向を反転して渦巻きを織る弾幕。予備動作は初期の 4 本の腕を示す橙の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し、通常・狂暴化の両行動順へ組み込んだ。雑魚は突進型 `windmill` と射撃型 `vane`（いずれも最大 Hp1・耐久系能力なし）を `parameters.json` に追加し、最終ウェーブへ各 2 体を加えた。

- 未実施: ビルド（Windows/DirectX 環境外のため）、実機確認、最終ウェーブの難度確認。
- 推測した用途: 回転方向が反転する渦で、安全地帯の読みを途中で切り替えさせるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128h`（develop から作成）。

## セッション 2026-11-27（atomF: ボス薔薇窓・rosette/corolla 追加・スケジュール実行）

develop を基に、ボスの新スキル **薔薇窓**（`Skill::Rosette`。3 方向へ 0.1 秒間隔で 30 波、向きの揺れと弾速が位相をずらして脈打つ桃色の弾幕。予備動作は 3 本の花弁の軸を示す桃色の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し、通常・狂暴化の両行動順に組み込んだ。雑魚は **rosette**（突進型）と **corolla**（遠距離射撃型）を `parameters.json` に追加（最大 Hp1・耐久系能力なし）し、最終ウェーブに配置した。

- 未実施: ビルド（Windows/DirectX 環境外のため）、実機確認、最終ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/epic-faraday-jpjk1k`（develop から作成）。

## セッション 2026-11-27（atomF: ボス万華鏡・kaleido/prism 追加・スケジュール実行）

develop を基に、ボスの新スキル **万華鏡**（`Skill::Kaleidoscope`。4 方向の軸それぞれから、軸を挟んで鏡写しに逆回りする 2 発を 0.11 秒間隔で 28 波放ち、左右対称の青紫の模様を織る弾幕。予備動作は 4 本の軸を示す青紫の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し、通常・狂暴化の行動順へ組み込んだ。雑魚 `kaleido`（突進型）・`prism`（射撃型）を `parameters.json` に追加し最終ウェーブへ各 2 体配置（いずれも最大 Hp1・耐久能力なし）。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機確認、最終ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128i`（develop から作成）。

## セッション 2026-11-27（atomF: ボス渦潮・eddy/maelstrom 追加・スケジュール実行）

develop を基に、ボスの新スキル **渦潮**（`Skill::Whirlpool`。3 本の腕が回りながら 0.08 秒間隔で 36 波・水色の弾を放ち、弾速が波ごとに遅くなるため内側に残る弾と外へ抜ける弾で渦の縞を描く弾幕。予備動作は 3 本の腕の初期位置を示す水色の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し、通常・狂暴化の行動順へ組み込んだ。雑魚 `eddy`（突進型）・`maelstrom`（射撃型）を `parameters.json` に追加し最終ウェーブへ各 2 体配置（いずれも最大 Hp1・耐久能力なし）。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機確認、最終ウェーブの難度確認。
- 推測した用途: 弾速差で時間とともに形が変わる渦を読ませ、安全地帯を探す状況判断を求めるため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/bold-lamport-p8d1c0`（develop から作成）。

## セッション 2026-11-27（atomF: ボス近接技「震脚」・quaker/fissure 追加・スケジュール実行）

develop を基に、ボスの新しい近接スキル **震脚**（`Skill::Quake`。その場で 3 度足を踏み鳴らし、踏むたびに到達半径が 3.6→5.6→7.6m と大きくなる衝撃波を放った後に硬直する。予備動作は 3 つの到達半径を示す同心円と足元の脈打つ輪、確定後に赤く太くなる）を `CmpAtomFBoss` に追加し、通常・狂暴化の両行動順に組み込んだ。あわせて雑魚 **quaker**（突進型）/ **fissure**（遠距離型）を `parameters.json` に追加し、最終ウェーブへ 2 体ずつ配置した（いずれも最大 Hp 1、耐久系能力なし）。

- 未実施: ビルド（Linux 環境のため）、実機確認、最終ウェーブの難度確認。静的チェックとして宣言・実装・enum・switch 分岐の整合のみ確認した。
- 推測した用途: 突進・跳躍と異なる「その場で連続して広がる衝撃波」を飛び越える／距離を取る判断を求め、近接技の種類を増やすため。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。指定の develop 基点で作業ブランチ `claude/serene-hypatia-kfbrgp` を使用した。

## セッション 2026-11-28（atomF: ボス蜘蛛の巣・weaver/spinner 追加・スケジュール実行）

develop を基に、ボスの新スキル **蜘蛛の巣**（`Skill::Cobweb`。8 本の放射状の糸を 0.5 秒間隔で 10 波放ち、波ごとに半ピッチずらし弾速を交互に変えて網目状の白銀の弾幕を織る。予備動作は 8 本の糸の初期位置を示す白銀の線、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し通常・狂暴化の行動順へ組み込んだ。雑魚 `weaver`（突進型）・`spinner`（射撃型）を `parameters.json` に追加し最終ウェーブへ各 2 体配置（いずれも最大 Hp1・耐久能力なし）。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機確認、最終ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128j`（develop から作成）。

## セッション 2026-11-28（atomF: ボス花火・sparkler/rocketeer 追加・スケジュール実行）

develop を基に、ボスの新スキル **花火**（`Skill::Fireworks`。ボスの周囲 5 点を星形の順に 0.45 秒間隔で打ち上げ、各点で 14 発の全方位リングを炸裂させる弾幕。予備動作は 5 つの炸裂点を示す橙の輪、確定後に太く濃くなる）を `CmpAtomFBoss` に追加し、通常・狂暴化の行動順へ組み込んだ。雑魚 `sparkler`（突進型）・`rocketeer`（射撃型）を `parameters.json` に追加し最終ウェーブへ各 2 体配置（いずれも最大 Hp1・耐久能力なし）。JSON 構文と enum・宣言・実装・switch 分岐の整合のみ静的確認。

- 未実施: ビルド（Linux 環境のため）、実機確認、最終ウェーブの難度確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず、`EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128k`（develop から作成）。

## セッション 2026-11-28（atomF: 花火の専用バーストパーティクル・演出の複雑化・スケジュール実行）

ボス・雑魚のバリエーションが十分に揃ったため、優先順の次段である演出の複雑化に着手した。花火スキル（`Skill::Fireworks`）の炸裂が汎用の `atomf_boss_bloom_burst.json` を流用していたため、3 層構成の専用 `atomf_boss_fireworks_burst.json`（中心の閃光＋金色の火花の殻＋重力で落ちる残り火）を追加し、`FireFireworksShot` の再生先を差し替えた。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128l`（develop から作成）。

## セッション 2026-11-28（atomF: 万華鏡・蜘蛛の巣の専用バーストパーティクル・演出の複雑化・スケジュール実行）

演出の複雑化の続き。汎用の `atomf_boss_fan_burst.json` / `atomf_boss_ring_burst.json` を流用していた万華鏡（`FireKaleidoscopeShot`）と蜘蛛の巣（`FireCobwebShot`）に、3 層構成（中心の閃光＋破片/糸の殻＋残り火）の専用 `atomf_boss_kaleidoscope_burst.json`（紫）・`atomf_boss_cobweb_burst.json`（白銀）を追加し再生先を差し替えた。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128m`（develop から作成）。

## セッション 2026-11-28（atomF: 五芒星・六花・風車・薔薇窓・渦潮の専用バーストパーティクル・演出の複雑化・スケジュール実行）

演出の複雑化の続き。汎用の `atomf_boss_fan_burst.json` / `atomf_boss_ring_burst.json` を流用していた五芒星（金）・六花（氷青）・風車（緑）・薔薇窓（桃）・渦潮（青緑）の炸裂に、3 層構成（中心の閃光＋破片の殻＋残り火）の専用 `atomf_boss_{pentacle,snowflake,windmill,rosette,whirlpool}_burst.json` を追加し、各 `Fire*Shot` の再生先を差し替えた。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128n`（develop から作成）。

## セッション 2026-11-28（atomF: 残りの汎用バーストを専用 3 層パーティクルへ置換・演出の複雑化・スケジュール実行）

演出の複雑化の続き。汎用の `atomf_boss_fan_burst.json` を流用していた幕・散弾・三日月・豪雨・鋏・回転木馬・潮・稲妻・彗星・滝・砂時計・振り子（`Fire*` 11 + 振り子）の炸裂に、スキル弾色に合わせた 3 層構成（中心の閃光＋破片の殻＋残り火。個数・速度・重力・閃光の大きさをスキルごとに変えた）の専用 `atomf_boss_{curtain,scatter,crescent,downpour,pincer,carousel,tide,zigzag,comet,cascade,hourglass,pendulum}_burst.json` を追加し、各 `Fire*` の再生先を差し替えた。扇（`FireFanWave`）は `atomf_boss_fan_burst.json` を緑の 3 層構成へ作り直した（これで汎用バーストの流用は無くなった）。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1127-o2`（develop から作成）。

## セッション 2026-11-28（atomF: ボス基本技の専用バーストパーティクル・演出の複雑化・スケジュール実行）

演出の複雑化の続き。汎用の `atomf_boss_ring_burst.json` を流用していたボスの基本技 5 種（リング=桃、隙間リング=水色、召喚の出現点=白、風車弾=水色、二重螺旋=黄緑）に、3 層構成（中心の閃光＋破片の殻＋残り火）の専用 `atomf_boss_ringshot_burst.json` / `gapring_burst` / `summon_burst` / `pinwheel_burst` / `helix_burst` を追加し、`CmpAtomFBoss.cpp` の再生先を差し替えた。これでボスの全スキルの炸裂が専用パーティクルになった。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `claude/atomf-work-1128o`（develop から作成）。

## セッション 2026-11-28（atomF: 雑魚出現エフェクトの 3 層化・演出の複雑化・スケジュール実行）

演出の複雑化の続き。単層だった雑魚の出現エフェクト `atomf_enemy_spawn.json` を、地面を走る広がる輪＋立ち上る光の粒＋中心の閃光の 3 層構成へ作り直した（再生コード `CmpAtomFEnemySpawnIn.cpp` は変更なし）。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `work/atomf-1128p`（develop から作成）。

## セッション 2026-11-28（atomF: 射撃マズルフラッシュの 3 層化・演出の複雑化・スケジュール実行）

演出の複雑化の続き。単層だった雑魚・ボス共通の発射時エフェクト `atomf_shot_muzzle.json` を、中心の閃光＋火花の殻＋落ちる残り火の 3 層構成へ作り直した（再生コードは変更なし）。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `work/atomf-1127-run`（develop から作成）。

## セッション 2026-11-28（atomF: ボス激昂・突進軌跡・吹き飛び軌跡/オーラの多層化・演出の複雑化・スケジュール実行）

演出の複雑化の続き。単層だった `atomf_boss_enrage.json`（地を走る衝撃輪＋炎柱＋中心の閃光＋舞い上がる火の粉の 4 層）、`atomf_boss_dash_trail.json`（紫の煙＋白紫の芯の閃光＋火花の 3 層）、`atomf_knockback_trail.json`（炎＋煙＋火花の 3 層）、`atomf_knockback_aura.json`（光の粒＋外周の輪＋金の火花の 3 層）を多層構成へ作り直した（再生コードは変更なし。吹き飛び軌跡は `SetEmitCircleRadiusOverride` が全層に適用される）。JSON 構文のみ検証。

- 未実施: ビルド（Linux 環境のため）、実機での見た目確認。
- 備考: 指定の `.claude/ideas/game_atomF.md` は存在せず `EditorAssets/Tools/ClaudeAI/docs/ideas/game_atomF.md` を参照した。作業ブランチは `work/atomf-1128q`（develop から作成）。
