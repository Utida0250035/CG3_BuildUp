#include "Engine/AtrumEngine.h"
#include "File/SetWorkingDirectoryToExecutable.h"
#include "Scene/GameOrder.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
  Atrum::CoUnInitializer coUnInitializer;

  if (!Atrum::File::SetWorkingDirectoryToExecutableDirectory()) {
    return 1;
  }

  Atrum::Engine *engine = Atrum::Engine::Instance();
  engine->Initialize("CG2 BuildUp");
  engine->SetFps(60);

  // フェード等で使用する白テクスチャを先に読み込む
  engine->GetTexture("./Assets/Images/white4x4.png");

  Atrum::GameOrder *gameOrder = Atrum::GameOrder::Instance();

  while (engine->Process()) {
    gameOrder->Run();
  }

  engine->Finalize();
  Atrum::Engine::Destroy();

  return 0;
}
