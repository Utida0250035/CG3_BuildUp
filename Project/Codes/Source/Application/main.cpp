#include "Application/ReleaseStartupSmokeFrames.h"
#include "Engine/AtrumEngine.h"
#include "File/SetWorkingDirectoryToExecutable.h"
#include "Scene/GameOrder.h"
#if defined(DEVELOPMENT)
#include "ForDebug/SubsystemFrameProfiler.h"
#endif

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR lpCmdLine, _In_ int) {
  Atrum::CoUnInitializer coUnInitializer;

  if (!Atrum::File::SetWorkingDirectoryToExecutableDirectory()) {
    return 1;
  }

  Atrum::Engine *engine = ::Atrum::EngineInstance();
  engine->Initialize("LE2A_03_ウチダ_コウタ");
  engine->SetFps(60);

  // フェード等で使用する白テクスチャを先に読み込む
  engine->GetTexture("./Assets/Images/white4x4.png");

  Atrum::GameOrder *gameOrder = Atrum::GameOrder::Instance();

  const int smokeFrameBudget =
      Atrum::Application::ResolveReleaseStartupSmokeFrameBudget(lpCmdLine);
  int smokeFrameCount = 0;

  while (true) {
#if defined(DEVELOPMENT)
    {
      Atrum::Debug::SubsystemFrameScope processScope(
          Atrum::Debug::SubsystemFrameId::ProcessPump);
      if (!engine->Process()) {
        break;
      }
    }
#else
    if (!engine->Process()) {
      break;
    }
#endif
    gameOrder->Run();

    if (smokeFrameBudget > 0) {
      ++smokeFrameCount;
      if (smokeFrameCount >= smokeFrameBudget) {
        break;
      }
    }
  }

  engine->Finalize();
  Atrum::AtrumEngine::Destroy();

  return 0;
}
