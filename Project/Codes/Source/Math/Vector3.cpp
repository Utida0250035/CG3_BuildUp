#include "Math/Vector3.h"
#include <cmath>

namespace Atrum::Math {

	float Vector3::Length() const {

		return std::sqrt(LengthSquare());

	}

}