#include "System/Entity/Components/CmpMesh.h"
#include "Engine/AtrumEngine.h"
#include "System/Entity/Entity.h"


namespace Atrum {

void CmpMesh::Initialize() {}

void CmpMesh::Draw() {
  Atrum::AtrumEngine::Instance()->DrawModel(model_, GetOwner()->GetWorldMatrix(),
                                            false);
}

} // namespace Atrum