#include "System/Editor/Entity/EntityEditorUiDrawComponentParamsGenericJson.h"

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsPlayable {

bool TryDraw(ComponentSlot &slot, bool &outChanged) {
  auto draw = [&slot, &outChanged]() -> bool {
    return EntityEditorUiDrawComponentParamsGenericJson::
        TryDrawUnknownComponentParams(slot, outChanged);
  };
  if (slot.typeName == "CmpInput") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpMoveByInput2D") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpDrawPlayerParryField") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpDirection") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpLhRigidBody") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpHealthHit") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpSwing") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpChildEntitys") {
    draw();
    return true;
  }
  if (slot.typeName == "CmpParry") {
    draw();
    return true;
  }
  return false;
}

} // namespace Atrum::EntityEditor::EntityEditorUiDrawComponentParamsPlayable