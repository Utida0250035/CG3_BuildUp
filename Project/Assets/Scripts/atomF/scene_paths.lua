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

  -- 本編 HUD（白地に透過。r,g,b で色付けして使う）
  IMG_WAVES = {
    "./Assets/Images/atomF/wave_1.png",
    "./Assets/Images/atomF/wave_2.png",
    "./Assets/Images/atomF/wave_3.png",
    "./Assets/Images/atomF/wave_4.png",
  },
  IMG_WARNING   = "./Assets/Images/atomF/warning.png",
  IMG_BOSS_NAME = "./Assets/Images/atomF/boss_name.png",
  IMG_CLEAR     = "./Assets/Images/atomF/clear.png",
  IMG_GAME_OVER = "./Assets/Images/atomF/game_over.png",
  IMG_WHITE     = "./Assets/Images/white4x4.png", -- 4x4 の白。scale で拡大して面・ゲージに使う
}

return Paths
