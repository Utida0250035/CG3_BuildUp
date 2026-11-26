#pragma once

#include <memory>
#include <string>

namespace Atrum {
class AssetModel;
class CmpMeshSphere;
} // namespace Atrum

namespace Atrum::AtomF {

/// `Assets/Objects/<modelName>/<modelName>.obj`（と同名の mtl）を読み、
/// エンジン終了まで保持する。名前が空かファイルが無ければ nullptr。
std::shared_ptr<AssetModel> LoadMeshModel(const std::string &modelName);

/// 読めたときだけ、球の代わりに描くモデルとして設定する。
void AttachMeshModel(CmpMeshSphere &mesh, const std::string &modelName);

} // namespace Atrum::AtomF
