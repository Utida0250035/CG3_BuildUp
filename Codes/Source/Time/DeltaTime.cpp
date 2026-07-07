#include "Time/DeltaTime.h"

namespace chrono = std::chrono;

void DeltaTime::CalcDeltaTime() {

	preTime_ = currentTime_;

	currentTime_ = chrono::steady_clock::now();

	deltaTime_ = chrono::duration_cast<chrono::milliseconds>(currentTime_ - preTime_);

	if (deltaTime_ >= chrono::milliseconds(70)) {

		deltaTime_ = chrono::milliseconds(1);

	} else if (deltaTime_ <= chrono::milliseconds(1)) {

		deltaTime_ = chrono::milliseconds(1);

	}

}

float DeltaTime::GetDeltaTime() {

	return static_cast<float>(deltaTime_.count()) / 1000.0f;

}