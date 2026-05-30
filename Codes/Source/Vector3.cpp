#include "Vector3.h"
#include <cmath>

float VectorLength(const Vector3& vector) {

	return std::sqrt(VectorLengthSquare(vector));

}