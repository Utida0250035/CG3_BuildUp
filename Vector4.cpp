#include "Vector4.h"
#include <cmath>

float VectorLength(const Vector4& vector) {

	return sqrt(VectorLengthSquare(vector));

}