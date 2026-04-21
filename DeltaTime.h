#pragma once

#include <chrono>

class DeltaTime final{
private:
	std::chrono::milliseconds deltaTime_ = std::chrono::milliseconds(0);

	std::chrono::time_point<std::chrono::steady_clock> currentTime_ = std::chrono::steady_clock::now();
	std::chrono::time_point<std::chrono::steady_clock> preTime_ = currentTime_;

	DeltaTime() = default;
	~DeltaTime() = default;

public:

	/// <summary>
	/// コピーコンストラクタの削除
	/// </summary>
	/// <param name="source"></param>
	DeltaTime(const DeltaTime& source) = delete;

	/// <summary>
	/// 代入演算子のオーバーロードの削除
	/// </summary>
	/// <param name="source"></param>
	/// <returns></returns>
	DeltaTime& operator=(const DeltaTime& source) = delete;

	/// <summary>
	/// 
	/// </summary>
	/// <returns> 静的インスタンス </returns>
	static DeltaTime* GetInstance() {

		static DeltaTime instance;

		return &instance;

	}

	/// <summary>
	/// 時間差分の計算
	/// </summary>
	void CalcDeltaTime();

	/// <summary>
	/// 
	/// </summary>
	/// <returns> 時間差分[s] </returns>
	float GetDeltaTime();

};