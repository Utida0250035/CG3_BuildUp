#include "Math/Vector3.h"
#include "System/Entity/ComponentData.h"

namespace Atrum {

class CmpDirection : public ComponentData {
private:
  Math::Vector3 direction_{};

public:
  Math::Vector3 GetDirection() const { return direction_; }
  void SetDirection(Math::Vector3 direction) { direction_ = direction; }
  Math::Vector3 &RefDirection() { return direction_; };
};

} // namespace Atrum