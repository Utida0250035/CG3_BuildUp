-- atomF のシーン遷移先と UI 画像パス（タイトル／本編／リザルト共通）
local Paths = {
  TITLE  = "./Assets/Scenes/atomFTitle.json",
  GAME   = "./Assets/Scenes/atomF.json",
  RESULT = "./Assets/Scenes/atomFResult.json",

  IMG_TITLE_LOGO = "./Assets/Images/atomF/title_logo.png",
  IMG_PRESS_ANY  = "./Assets/Images/atomF/press_any_to_start.png",
  IMG_RESULT     = "./Assets/Images/atomF/result_text.png",
  IMG_RETRY      = "./Assets/Images/atomF/retry.png",
  IMG_TO_TITLE   = "./Assets/Images/atomF/to_title.png",
  IMG_CLEAR      = "./Assets/Images/atomF/clear.png",     -- 白地に透過（r,g,b で着色）
  IMG_GAME_OVER  = "./Assets/Images/atomF/game_over.png",
  IMG_WHITE      = "./Assets/Images/white4x4.png",        -- 4x4 の白。scale で拡大して全画面フラッシュに使う
}

return Paths
