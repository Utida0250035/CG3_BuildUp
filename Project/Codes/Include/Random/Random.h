#pragma once

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

public:
  static int32_t GetInt32(int32_t min, int32_t max) {
    std::uniform_int_distribution<int> dist(min, max);
  }
};

} // namespace Atrum::Algorithm