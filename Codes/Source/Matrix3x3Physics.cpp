#include "Matrix3x3Physics.h"

#include <cmath>
#include <format>
#include <iostream>

Matrix3x3Physics MakeIdentityMatrix3x3Physics() {

    Matrix3x3Physics result{};

    result.m[0][0] = 1.0f;
    result.m[1][1] = 1.0f;
    result.m[2][2] = 1.0f;

    return result;
}

Matrix3x3Physics MakeZeroMatrix3x3Physics() {

    return Matrix3x3Physics{};
}

Matrix3x3Physics Matrix3x3PhysicsTranspose(
    const Matrix3x3Physics& matrix) {

    Matrix3x3Physics result{};

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {
            result.m[y][x] = matrix.m[x][y];
        }
    }

    return result;
}

Matrix3x3Physics Matrix3x3PhysicsMultiply(
    const Matrix3x3Physics& a,
    const Matrix3x3Physics& b) {

    Matrix3x3Physics result{};

    for (int y = 0; y < 3; ++y) {
        for (int x = 0; x < 3; ++x) {

            result.m[y][x] =
                a.m[y][0] * b.m[0][x] +
                a.m[y][1] * b.m[1][x] +
                a.m[y][2] * b.m[2][x];
        }
    }

    return result;
}

Vector3 VectorTransform(
    const Vector3& vector,
    const Matrix3x3Physics& matrix) {

    return Vector3{
        vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0],
        vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1],
        vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2]
    };
}

Matrix3x3Physics MakeMatrix3x3PhysicsFromQuaternion(
    const Quaternion& q) {

    Quaternion nq = q.normalized();

    float x = nq.x;
    float y = nq.y;
    float z = nq.z;
    float w = nq.w;

    Matrix3x3Physics result{};

    result.m[0][0] = 1.0f - 2.0f * y * y - 2.0f * z * z;
    result.m[0][1] = 2.0f * x * y + 2.0f * w * z;
    result.m[0][2] = 2.0f * x * z - 2.0f * w * y;

    result.m[1][0] = 2.0f * x * y - 2.0f * w * z;
    result.m[1][1] = 1.0f - 2.0f * x * x - 2.0f * z * z;
    result.m[1][2] = 2.0f * y * z + 2.0f * w * x;

    result.m[2][0] = 2.0f * x * z + 2.0f * w * y;
    result.m[2][1] = 2.0f * y * z - 2.0f * w * x;
    result.m[2][2] = 1.0f - 2.0f * x * x - 2.0f * y * y;

    return result;
}

Matrix3x3Physics MakeBoxInverseInertiaTensor(
    float mass,
    float width,
    float height,
    float depth) {

#ifdef _DEBUG

    std::cout << std::format(
        "mass={} width={} height={} depth={}",
        mass,
        width,
        height,
        depth)
        << std::endl;

#endif



    if (mass <= 0.0f) {
        return MakeZeroMatrix3x3Physics();
    }

    float ix = mass * (height * height + depth * depth) / 12.0f;
    float iy = mass * (width * width + depth * depth) / 12.0f;
    float iz = mass * (width * width + height * height) / 12.0f;

    Matrix3x3Physics result{};

    if (ix > 0.000001f) {
        result.m[0][0] = 1.0f / ix;
    }

    if (iy > 0.000001f) {
        result.m[1][1] = 1.0f / iy;
    }

    if (iz > 0.000001f) {
        result.m[2][2] = 1.0f / iz;
    }

    return result;
}

Matrix3x3Physics MakeWorldInverseInertiaTensor(
    const Matrix3x3Physics& localInverseInertia,
    const Quaternion& rotation) {

    Matrix3x3Physics r =
        MakeMatrix3x3PhysicsFromQuaternion(rotation.normalized());

    Matrix3x3Physics rt =
        Matrix3x3PhysicsTranspose(r);

    return Matrix3x3PhysicsMultiply(
        Matrix3x3PhysicsMultiply(r, localInverseInertia),
        rt);
}