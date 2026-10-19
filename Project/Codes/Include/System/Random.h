#pragma once

#include <concepts>
#include <cstdint>
#include <limits>
#include <random>
#include <type_traits>

namespace Atrum::Algorithm {

// 乱数クラス
class Random {
private:
  Random() = default;
  ~Random() = default;

  static std::mt19937 &Engine() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 engine(rd());
    return engine;
  }

  Random(const Random &) = delete;
  Random &operator=(const Random &) = delete;
  Random(Random &&) = delete;
  Random &operator=(Random &&) = delete;

public:
  // 整数
  template <std::integral T> static T Int(T min, T max) {
    std::uniform_int_distribution<T> dist(min, max);
    return dist(Engine());
  }

  // 浮動小数点数
  template <std::floating_point T> static T Float(T min, T max) {
    std::uniform_real_distribution<T> dist(min, max);
    return dist(Engine());
  }

  // 0から1の浮動小数点数 比率
  template <typename T = float> static T Canonical() {
    // コンパイル時に浮動小数点型以外の型指定を検知する
    static_assert(std::is_floating_point_v<T>,
                  "Template parameter must be a floating-point type.");

    constexpr size_t bits = std::numeric_limits<T>::digits;

    return std::generate_canonical<T, bits>(Engine());
  }
};

} // namespace Atrum::Algorithm