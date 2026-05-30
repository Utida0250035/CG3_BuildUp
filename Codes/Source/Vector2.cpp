#include "Vector2.h"
#include <cMath>

float VectorLength(const Vector2& me) {

	return std::sqrt(me.x * me.x + me.y * me.y);

}