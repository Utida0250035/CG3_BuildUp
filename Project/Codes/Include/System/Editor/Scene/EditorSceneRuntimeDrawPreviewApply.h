#pragma once

#include <string>

namespace Atrum {
class CmpEditorTextureBillboard;
class CmpMesh;
}

namespace Atrum::EntityEditor::EditorSceneRuntimeDrawPreviewApply {

bool IsImageAssetExtensionLower(const std::string &extensionLower);

void TryApplyCmpMeshObjAssetPath(CmpMesh &mesh, const std::string &path);

void TryApplyCmpEditorTextureBillboardPath(CmpEditorTextureBillboard &billboard,
                                           const std::string &path);

} // namespace Atrum::EntityEditor::EditorSceneRuntimeDrawPreviewApply