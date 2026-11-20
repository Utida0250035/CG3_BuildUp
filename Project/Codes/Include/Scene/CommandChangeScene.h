#pragma once

#include "Audio/AudioHandle.h"
#include "CommandScene.h"
#include "ChangeScene.h"
#include "Scene.h"
#include <cassert>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>

namespace Atrum {

/// <summary>
/// 指令類 > シーン遷移指令
/// 
/// シーン間の遷移処理の指令クラス
/// </summary>
	class CommandChangeScene final {

	private:

		/// <summary>
		/// 遷移エフェクトのインスタンス
		/// </summary>
		std::unique_ptr<ChangeScene> changeSceneEffect_;

		/// <summary>
		/// シーン処理へのアクセス
		/// </summary>
		CommandScene* commandScene_;

		/// <summary>
		/// 次のシーン
		/// </summary>
		Scene* nextScene_;

		/// <summary>
		/// 実行フラグ
		/// </summary>
		bool isExecute_;

		/// <summary>
		/// ChangeScene 演出中は EnterScene の BGM 開始をフェードイン完了まで遅らせる
		/// </summary>
		bool deferEnterSceneBgmUntilFadeEnds_ = false;

		std::vector<std::string> deferredEnterSceneBgmCandidates_;

		Audio::Handle pendingEnterSceneBgmHandle_{};

		/// ExitScene がフェード中に保持していた BGM（フェードイン完了で停止）
		Audio::Handle exitingSceneBgmHandle_{};

		/// 遷移実行中に届いた Set（最後の要求のみ保持）
		bool hasQueuedSet_ = false;
		Scene *queuedNextScene_ = nullptr;
		std::unique_ptr<ChangeScene> queuedChangeSceneEffect_;
		bool queuedSetUsesFadeEffect_ = false;

		void ApplySetInternal(Scene *nextScene, ChangeScene *newEffect);

		void TryStartQueuedSet();

		void OnSceneTransitionEffectFinished();

		/// <summary>
		/// コンストラクタ
		/// </summary>
		CommandChangeScene();

		/// <summary>
		/// デストラクタ 空
		/// </summary>
		~CommandChangeScene() = default;

	public:

		/// <summary>
		/// コピーコンストラクタの削除
		/// </summary>
		/// <param name="source"></param>
		CommandChangeScene(const CommandChangeScene& source) = delete;

		/// <summary>
		/// 代入演算子のオーバーロードの削除
		/// </summary>
		/// <param name="source"></param>
		/// <returns></returns>
		CommandChangeScene& operator=(const CommandChangeScene& source) = delete;

		/// <summary>
		/// 
		/// </summary>
		/// <returns> 遷移エフェクト指令の静的インスタンス </returns>
		static CommandChangeScene* Instance() {

			static CommandChangeScene instance;

			return &instance;

		}

		/// <summary>
		/// シーン遷移の更新処理 (注: ゲーム指令のRun関数でのみ実行すること)
		/// </summary>
		void Update() {

			if (isExecute_) {

				if (changeSceneEffect_ == nullptr) {
					// シーン遷移エフェクトが空のポインタであれば

					// 即座にシーン遷移
					commandScene_->ChangeScene(nextScene_);

					// 次のシーンに空のポインタを設定
					nextScene_ = nullptr;

					// 処理中フラグを折る
					isExecute_ = false;
					TryStartQueuedSet();

				} else {
					// シーン遷移エフェクトが空のポインタでなければ

					// シーン遷移エフェクトの処理
					//==============================

					// 更新
					changeSceneEffect_.get()->Update();

					// 更新後に処理中フラグfalseかつ遷移フラグfalse→エラー 使用したシーン遷移クラスの処理を見直すこと
					assert(changeSceneEffect_.get()->GetIsProcess() || changeSceneEffect_.get()->GetIsSceneChange());

					if (changeSceneEffect_.get()->GetIsSceneChange()) {

						if (nextScene_ != nullptr) {
							// 遷移先シーンが空のポインタでなければ

							// シーン遷移を行なう
							commandScene_->ChangeScene(nextScene_);

							// 遷移先シーンに空のポインタを設定
							nextScene_ = nullptr;

						}

						if (!changeSceneEffect_.get()->GetIsProcess()) {
							// シーン遷移エフェクトが終了していれば
							//======================================

							// エフェクトを破棄する
							changeSceneEffect_.reset();

							OnSceneTransitionEffectFinished();

							// 処理中フラグを折る
							isExecute_ = false;
							TryStartQueuedSet();

						}

					}

				}

			}

		}

		/// <summary>
		/// シーン遷移の描画処理 (注:ゲーム指令のRun関数でのみ実行すること)
		/// </summary>
		void Draw() {

			if (isExecute_) {
				// 実行中であれば

				if (changeSceneEffect_.get() != nullptr) {
					// シーン遷移エフェクトが空のポインタでなければ

					// シーン遷移エフェクトを描画
					changeSceneEffect_.get()->Draw();

				}

			}

		}

		/// <summary>
		/// 
		/// </summary>
		/// <returns> シーン遷移の処理中フラグ </returns>
		bool GetIsExecute() const {

			return isExecute_;

		}

		/// フェードアウト中（未切替）は false。フェードイン中は現シーンの Update を許可する。
		bool AllowsCurrentSceneUpdate() const;

		bool ShouldDeferEnterSceneBgm() const {
			return deferEnterSceneBgmUntilFadeEnds_;
		}

		void RegisterDeferredEnterSceneBgm(
			const std::initializer_list<const char *> candidatePaths);

		Audio::Handle TryTakePendingEnterSceneBgmHandle();

		/// フェード演出中の ExitScene から BGM スロットを引き継ぎ、即 Stop を避ける
		void AdoptExitingSceneBgmPendingFadeStop(Audio::Handle &handle);

		Scene* GetPendingNextScene() const { return nextScene_; }

		/// <summary>
		/// シーン遷移の設定 (注: 各シーンの更新処理でのみ実行すること)
		/// </summary>
		/// <param name="nextScene"> 次のシーンのインスタンス</param>
		/// <param name="newEffect"> 遷移エフェクトのインスタンス (無しにするも可) </param>
		void Set(Scene *nextScene, ChangeScene *newEffect = nullptr);

	};

}