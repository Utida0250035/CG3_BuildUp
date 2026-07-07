#pragma once
#include "Math/Vector3.h"

struct Triangle {
    Vector3 v0, v1, v2;
    Vector3 normal; // 事前に法線を計算しておくこと
};