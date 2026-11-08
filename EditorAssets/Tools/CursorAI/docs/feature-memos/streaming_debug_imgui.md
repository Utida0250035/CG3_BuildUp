# ストリーミング再生デバッグ（ImGui）

## メモリ使用量表示（実装済み）

- **API:** `AudioManager::CollectStreamingMemoryDebug`（DEVELOPMENT）
- **UI:** メニュー「デバッグ」→「ストリーミングメモリ」
- **表示:** 短尺ロード済みバイト、ストリーミングバッファ／remaining、ボイス一覧

`GameOrder` 毎フレーム `DrawAudioStreamingMemoryDebugWindow()` を呼び、フラグ ON 時のみ描画。

## 次

- roadmap: ストリーミング進捗表示（再生位置・バッファ充填率）

## 用途

- **デバッグ:** BGM ストリームのメモリ枯渇・remaining 肥大の調査
