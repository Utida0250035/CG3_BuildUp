#pragma once
#include "CommandScene.h"
#include "PlayerInput.h"
#include "CommandChangeScene.h"
#include "WindowSize.h"
#include <memory>
#include <chrono>

/// <summary>
/// ゲーム統制
/// </summary>
class GameOrder final {

private:

	/// <summary>
	/// プレイヤー入力へのアクセス
	/// </summary>
	PlayerInput* playerInput_;

	/// <summary>
	/// シーン指令へのアクセス
	/// </summary>
	CommandScene* commandScene_;

	/// <summary>
	/// シーン遷移指令へのアクセス
	/// </summary>
	CommandChangeScene* commandChangeScene_;

	/// <summary>
	/// 前フレームの実時間
	/// </summary>
	std::chrono::time_point<std::chrono::steady_clock> preTime_;

	/// <summary>
	/// 今フレームの実時間
	/// </summary>
	std::chrono::time_point<std::chrono::steady_clock> currentTime_;

	/// <summary>
	/// 前フレームから今フレームまでの実時間(デルタタイム)
	/// </summary>
	std::chrono::milliseconds deltaTime_;

	/// <summary>
	/// デルタタイムを計算
	/// </summary>
	void DeltaTimeCalc() {

		preTime_ = currentTime_;

		currentTime_ = std::chrono::steady_clock::now();

		deltaTime_ = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime_ - preTime_);

	}

	/// <summary>
	/// コンストラクタ
	/// </summary>
	GameOrder();

public:

	/// <summary>
	/// 実行部分
	/// </summary>
	void Run() {

		// デルタタイムを計算
		DeltaTimeCalc();

		// プレイヤー入力を更新
		playerInput_->Update();

		if (!commandChangeScene_->GetIsExecute()) {
			// シーン遷移の処理中でなければ

			// 現在シーンの更新処理
			commandScene_->GetSceneNow()->Update();

		}

		// シーン遷移の更新処理
		commandChangeScene_->Update();

		// 全シーン共通の裏地の描画
		Novice::DrawBox(
			0, 0,
			static_cast<int>(kWindowWidth), static_cast<int>(kWindowHeight),
			0.0f,
			0xAAAAAAFF,
			kFillModeSolid
		);

		// 現在シーンの描画処理
		commandScene_->GetSceneNow()->Draw();

		// シーン遷移の描画処理
		commandChangeScene_->Draw();

	}

	/// <summary>
	/// デルタタイムを取得(最小単位: マイクロ秒[ms] == 1/1,000,000秒)
	/// </summary>
	/// <returns></returns>
	float GetDeltaTime() const {

		return static_cast<float>(deltaTime_.count()) / 1000.0f;

	}

	/// <summary>
	/// デストラクタ 空
	/// </summary>
	~GameOrder() = default;

	/// <summary>
	/// コピーコンストラクタの削除
	/// </summary>
	/// <param name="source"></param>
	GameOrder(const GameOrder& source) = delete;

	/// <summary>
	/// 代入演算子のオーバーロードの削除
	/// </summary>
	/// <param name="source"></param>
	/// <returns></returns>
	GameOrder& operator=(const GameOrder& source) = delete;

	/// <summary>
	///
	/// </summary>
	/// <returns> ゲーム統制の静的インスタンス </returns>
	static GameOrder* GetInstance() {

		static GameOrder instance;

		return &instance;

	}

};