#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"

namespace Atrum::SceneBuildUpSceneDocumentTagCatalog {

/// エディタ LoadFromJsonPath と同様に registry／インスタンスから tagCatalog を整える
void ApplyAfterJsonLoad(EntityEditor::EditorSceneDocument &document);

} // namespace Atrum::SceneBuildUpSceneDocumentTagCatalog