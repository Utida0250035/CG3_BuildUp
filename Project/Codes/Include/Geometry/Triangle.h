#pragma once
#include "Math/Vector3.h"

namespace  Atrum::Geometry {

    struct Triangle {
        Math::Vector3 v0, v1, v2;
        Math::Vector3 normal; // 事前に法線を計算しておくこと
    };

}