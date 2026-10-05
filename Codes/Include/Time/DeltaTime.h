#pragma once

#include <chrono>
#include <memory>

#include <cassert>

namespace Atrum {

// 任意のタイミング間の時間差分の計測用クラス
class DeltaTime final {
   private:
	std::chrono::milliseconds deltaTime_ = std::chrono::milliseconds(0);

	std::chrono::time_point<std::chrono::steady_clock> currentTime_ = std::chrono::steady_clock::now();
	std::chrono::time_point<std::chrono::steady_clock> preTime_ = currentTime_;

   public:
	DeltaTime() = default;
	~DeltaTime() = default;

	/// <summary>
	/// 時間差分の計算
	/// </summary>
	void CalcDeltaTime();

	/// <summary>
	///
	/// </summary>
	/// <returns> 時間差分[s] </returns>
	constexpr float GetDeltaTime() { return static_cast<float>(deltaTime_.count()) * 0.001f; }
};

// フレーム間の時間差分の計測用クラス
class FrameDeltaTime final {
   private:
	std::unique_ptr<DeltaTime> deltaTime_;
	FrameDeltaTime() = default;
	~FrameDeltaTime() = default;

   public:
	FrameDeltaTime(const FrameDeltaTime& source) = delete;
	FrameDeltaTime operator=(const FrameDeltaTime& source) = delete;

	static FrameDeltaTime* GetInstance() {
		static FrameDeltaTime instance;

		return &instance;
	}

	void Initialize() { deltaTime_ = std::make_unique<DeltaTime>(); }
	void CalcDeltaTime() { 
		deltaTime_->CalcDeltaTime(); 
	}

	constexpr float GetDeltaTime() { 
		return deltaTime_->GetDeltaTime(); 
	}
};

}  // namespace Atrum