#include "Engine/EngineAccess.h"

#include "Engine/AtrumEngine.h"
#include "Engine/Resource/AssetModel.h"

namespace Atrum {

uint32_t EngineGetTexture(const std::string &filePath) {
  return AtrumEngine::Instance()->GetTexture(filePath);
}

std::shared_ptr<AssetModel> EngineGetModel(const std::string &directoryPathObj,
                                         const std::string &objFileName,
                                         const std::string &directoryPathMtl,
                                         const std::string &mtlFileName) {
  return AtrumEngine::Instance()->GetModel(directoryPathObj, objFileName,
                                           directoryPathMtl, mtlFileName);
}

void EngineSetDirectionalLightData(const DirectionalLightData &data) {
  AtrumEngine::Instance()->SetDirectionalLightData(data);
}

bool EngineTryGetClientViewportSize(int32_t &outWidth, int32_t &outHeight) {
  if (AtrumEngine *engine = AtrumEngine::Instance()) {
    return engine->TryGetClientViewportSize(outWidth, outHeight);
  }
  return false;
}

} // namespace Atrum