#pragma once
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

namespace Atrum {

	struct VertexData {
		Math::Vector4 position;
		Math::Vector2 texCoord;
		Math::Vector3 normal;
	};

}