#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"
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
		if (exitingSceneBgmHandle_.isSuccess) {
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

}