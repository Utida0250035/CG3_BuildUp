#pragma once
#include "ChangeScene.h"
#include <cstdint>

namespace Atrum {

class ChangeSceneFade : public ChangeScene {

private:
	float timeRatio_;
	float waitCount_;
	bool isSeOpenTrigger_;

	// 全面を覆う白テクスチャ
	uint32_t textureIndex_ = 0;

public:
	ChangeSceneFade();

	void Update() override;
	void Draw() override;
};

} // namespace Atrum
