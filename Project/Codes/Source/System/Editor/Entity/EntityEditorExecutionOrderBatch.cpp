#include "System/Editor/Entity/EntityEditorExecutionOrderBatch.h"

#include <algorithm>
#include <vector>

namespace Atrum::EntityEditor::EntityEditorExecutionOrderBatch {

namespace {

int ClampExecutionOrder(const int value) { return std::max(0, value); }

} // namespace

int ApplyExecutionOrderToAllSlotsOfType(EntityDefinition &definition,
                                        const std::string &typeName,
                                        const int executionOrder) {
  if (typeName.empty()) {
    return 0;
  }
  const int clamped = ClampExecutionOrder(executionOrder);
  int updated = 0;
  for (ComponentSlot &slot : definition.components) {
    if (slot.typeName == typeName) {
      slot.executionOrder = clamped;
      ++updated;
    }
  }
  return updated;
}

int ApplyExecutionOrderToAllSlotsOfKind(EntityDefinition &definition,
                                        const ComponentSlotKind kind,
                                        const int executionOrder) {
  const int clamped = ClampExecutionOrder(executionOrder);
  int updated = 0;
  for (ComponentSlot &slot : definition.components) {
    if (slot.kind == kind) {
      slot.executionOrder = clamped;
      ++updated;
    }
  }
  return updated;
}

int RenumberExecutionOrderSequentiallyForKind(EntityDefinition &definition,
                                              const ComponentSlotKind kind,
                                              const int startOrder) {
  const int clampedStart = ClampExecutionOrder(startOrder);
  std::vector<size_t> indices;
  indices.reserve(definition.components.size());
  for (size_t i = 0; i < definition.components.size(); ++i) {
    if (definition.components[i].kind == kind) {
      indices.push_back(i);
    }
  }
  if (indices.size() <= 1) {
    return 0;
  }
  std::sort(indices.begin(), indices.end(),
            [&definition](const size_t a, const size_t b) {
              const int orderA = definition.components[a].executionOrder;
              const int orderB = definition.components[b].executionOrder;
              if (orderA != orderB) {
                return orderA < orderB;
              }
              return a < b;
            });

  int nextOrder = clampedStart;
  for (const size_t index : indices) {
    definition.components[index].executionOrder = nextOrder;
    ++nextOrder;
  }
  return static_cast<int>(indices.size());
}

} // namespace Atrum::EntityEditor::EntityEditorExecutionOrderBatch