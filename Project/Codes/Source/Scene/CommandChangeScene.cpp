#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"
#include "Scene/SceneTransitionBgmPolicy.h"
#include "System/PlaySe.h"

namespace Atrum {

/// <summary>
/// 指令類 > シーン遷移指令 > コンストラクタ
/// </summary>
	CommandChangeScene::CommandChangeScene() {

		commandScene_ = CommandScene::Instance();
		nextScene_ = nullptr;
		changeSceneEffect_.reset();
		isExecute_ = false;

	}

	void CommandChangeScene::RegisterDeferredEnterSceneBgm(
		const std::initializer_list<const char *> candidatePaths) {
		deferredEnterSceneBgmCandidates_.clear();
		for (const char *path : candidatePaths) {
			if (path != nullptr) {
				deferredEnterSceneBgmCandidates_.emplace_back(path);
			}
		}
	}

	Audio::Handle CommandChangeScene::TryTakePendingEnterSceneBgmHandle() {
		if (!pendingEnterSceneBgmHandle_.isSuccess) {
			return {};
		}

		const Audio::Handle handle = pendingEnterSceneBgmHandle_;
		pendingEnterSceneBgmHandle_ = {};
		return handle;
	}

	void CommandChangeScene::AdoptExitingSceneBgmPendingFadeStop(
		Audio::Handle &handle) {
		if (!handle.isSuccess) {
			return;
		}
		if (exitingSceneBgmHandle_.isSuccess) {
			StopBgmLoop(exitingSceneBgmHandle_);
			exitingSceneBgmHandle_ = {};
		}
		exitingSceneBgmHandle_ = handle;
		handle = {};
	}

	void CommandChangeScene::OnSceneTransitionEffectFinished() {
		if (SceneTransitionBgm::kUseFadeDuckInsteadOfInstantStop &&
		    exitingSceneBgmHandle_.isSuccess) {
			StopBgmLoop(exitingSceneBgmHandle_);
			exitingSceneBgmHandle_ = {};
		}

		deferEnterSceneBgmUntilFadeEnds_ = false;

		if (deferredEnterSceneBgmCandidates_.empty()) {
			return;
		}

		if (deferredEnterSceneBgmCandidates_.size() >= 2) {
			pendingEnterSceneBgmHandle_ = PlayBgmLoopAfterSceneFade(
				{deferredEnterSceneBgmCandidates_[0].c_str(),
				 deferredEnterSceneBgmCandidates_[1].c_str()});
		} else {
			pendingEnterSceneBgmHandle_ = PlayBgmLoopAfterSceneFade(
				{deferredEnterSceneBgmCandidates_[0].c_str()});
		}
		deferredEnterSceneBgmCandidates_.clear();
	}

	bool CommandChangeScene::AllowsCurrentSceneUpdate() const {
		if (!isExecute_) {
			return true;
		}
		if (changeSceneEffect_ == nullptr) {
			return true;
		}
		return changeSceneEffect_->GetIsSceneChange();
	}

	void CommandChangeScene::ApplySetInternal(Scene *nextScene,
	                                          ChangeScene *newEffect) {
		if (newEffect != nullptr) {
			changeSceneEffect_.reset(newEffect);
			deferEnterSceneBgmUntilFadeEnds_ = true;
			deferredEnterSceneBgmCandidates_.clear();
			pendingEnterSceneBgmHandle_ = {};
		} else {
			changeSceneEffect_.reset();
			deferEnterSceneBgmUntilFadeEnds_ = false;
			deferredEnterSceneBgmCandidates_.clear();
			pendingEnterSceneBgmHandle_ = {};
		}

		nextScene_ = nextScene;
		isExecute_ = true;
	}

	void CommandChangeScene::Set(Scene *nextScene, ChangeScene *newEffect) {
		if (isExecute_) {
			// フェードアウト前なら遷移先だけ差し替え可能（同一演出を継続）
			if (changeSceneEffect_ != nullptr &&
			    !changeSceneEffect_->GetIsSceneChange()) {
				nextScene_ = nextScene;
				if (newEffect != nullptr) {
					changeSceneEffect_.reset(newEffect);
					deferEnterSceneBgmUntilFadeEnds_ = true;
					deferredEnterSceneBgmCandidates_.clear();
					pendingEnterSceneBgmHandle_ = {};
				}
				return;
			}

			hasQueuedSet_ = true;
			queuedNextScene_ = nextScene;
			queuedSetUsesFadeEffect_ = (newEffect != nullptr);
			if (newEffect != nullptr) {
				queuedChangeSceneEffect_.reset(newEffect);
			} else {
				queuedChangeSceneEffect_.reset();
			}
			return;
		}

		ApplySetInternal(nextScene, newEffect);
	}

	void CommandChangeScene::TryStartQueuedSet() {
		if (!hasQueuedSet_ || isExecute_) {
			return;
		}

		hasQueuedSet_ = false;
		Scene *next = queuedNextScene_;
		queuedNextScene_ = nullptr;

		ChangeScene *effect = nullptr;
		std::unique_ptr<ChangeScene> adoptedEffect;
		if (queuedSetUsesFadeEffect_) {
			adoptedEffect = std::move(queuedChangeSceneEffect_);
			effect = adoptedEffect.release();
		} else {
			queuedChangeSceneEffect_.reset();
		}

		ApplySetInternal(next, effect);
	}

}