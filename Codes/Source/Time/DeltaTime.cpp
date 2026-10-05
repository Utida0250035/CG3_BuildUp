#include "Time/DeltaTime.h"

namespace Atrum {

namespace chrono = ::std::chrono;

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

}  // namespace Atrum