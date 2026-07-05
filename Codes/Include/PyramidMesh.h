#pragma once

#include "Vector3.h"
#include <vector>

struct PyramidMesh {
    std::vector<Vector3> vertices = {
        {-5.0f, -5.0f, 0.0f}, { 5.0f, -5.0f, 0.0f},
        { 5.0f,  5.0f, 0.0f}, {-2.5f,  2.5f, 0.0f},
        { 0.0f,  0.0f, 10.0f}
    };
    std::vector<uint16_t> indices = {
        0, 1, 2,  0, 2, 3, // 底面
        0, 1, 4,  1, 2, 4, // 側面
        2, 3, 4,  3, 0, 4  // 側面
    };

};