# サブシステム別フレームプロファイラ

本番相当との切り分け: `development_opt_profile.md`

## 用途

- **DEVELOPMENT ビルド:** 1 フレーム内の CPU 区間（Process / Update / Draw / Present 前後）の当たりを付ける
- **デバッグ:** フレーム時間オーバーレイ（`FrameDeltaTimeDebugImGui`）と併用

## 区間

| ID | 区間 |
|----|------|
| ProcessPump | `AtrumEngine::Process`（SDL・デルタタイム） |
| ImGuiPrep | ImGui NewFrame + デバッグ窓 |
| Input | `AbstractInput::Update` |
| SceneUpdate | 現シーン `Update` |
| SceneTransition | `CommandChangeScene::Update` |
| Audio | `TickGameOrderFrameAudio` |
| CameraPrepare | `ActiveCameraSystem::PrepareSceneAndBindEngine` |
| GfxPreDraw | `AtrumEngine::PreDraw` |
| GfxSceneDraw | `DrawScene` + 遷移描画 + ImGui Render |
| GfxPostDraw | `AtrumEngine::PostDraw`（Present まで） |

## UI

- 画面左上付近オーバーレイ（区間ごとの ms バー）
- メニュー「描画診断」→「サブシステム フレームプロファイラ」で詳細窓

実装: `ForDebug/SubsystemFrameProfiler.*`