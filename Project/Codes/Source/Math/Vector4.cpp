#include "Math/Vector4.h"
#include <cmath>

namespace Atrum::Math {

	float Vector4::Length() const {

		return sqrt(LengthSquare());

	}

}