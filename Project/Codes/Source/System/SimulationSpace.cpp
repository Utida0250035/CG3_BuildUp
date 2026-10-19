#include "System/SimulationSpace.h"

#include <nlohmann/json.hpp>

namespace Atrum {

NLOHMANN_JSON_SERIALIZE_ENUM(SimulationSpace, {SimulationSpace::LOCAL, "LOCAL"},
                             {SimulationSpace::WORLD, "WORLD"})

}
