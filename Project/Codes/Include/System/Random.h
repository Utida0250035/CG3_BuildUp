#pragma once

#include <concepts>
#include <cstdint>
#include <random>

namespace Atrum::Algorithm {

class Random {
private:
  Random() = default;
  ~Random() = default;

  static std::mt19937 &GetEngine() {
    static thread_local std::random_device rd;
    static thread_local std::mt19937 engine(rd());
    return engine;
  }

  Random(const Random &) = delete;
  Random &operator=(const Random &) = delete;
  Random(Random &&) = delete;
  Random &operator=(Random &&) = delete;

public:
  template <std::integral T> static T Int(T min, T max) {
    std::uniform_int_distribution<T> dist(min, max);
    return dist(GetEngine());
  }

  template <std::floating_point T> static T Float(T min, T max) {
    std::uniform_real_distribution<T> dist(min, max);
    return dist(GetEngine());
  }
};

} // namespace Atrum::Algorithm