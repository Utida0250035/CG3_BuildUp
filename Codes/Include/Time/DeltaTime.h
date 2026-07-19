#pragma once

#include <chrono>

namespace Atrum {

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
		float GetDeltaTime();

	};

}