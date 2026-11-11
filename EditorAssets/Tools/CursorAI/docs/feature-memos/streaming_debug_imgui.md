# ストリーミング再生デバッグ（ImGui）

## メモリ使用量表示（実装済み）

- **API:** `AudioManager::CollectStreamingMemoryDebug`（DEVELOPMENT）
- **UI:** メニュー「デバッグ」→「ストリーミングメモリ」
- **表示:** 短尺ロード済みバイト、ストリーミングバッファ／remaining、ボイス一覧

`GameOrder` 毎フレーム `DrawAudioStreamingMemoryDebugWindow()` を呼び、フラグ ON 時のみ描画。

## ストリーミング進捗表示（実装済み）

各ボイス行に以下を表示:

- **再生(s):** `startTime100ns` + XAudio2 `SamplesPlayed`
- **Queue:** `BuffersQueued / kBufferCount` プログレスバー
- **読込:** `remainingData` / チャンクサイズの比率（1 超は赤バー＋ `Nx !` 表示。バーは 1 でクランプ）

## 用途

- **デバッグ:** BGM ストリームのメモリ枯渇・remaining 肥大の調査
