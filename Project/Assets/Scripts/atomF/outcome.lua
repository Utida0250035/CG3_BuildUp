-- 本編 → リザルトへ結果を渡す共有テーブル。
-- require のキャッシュで同じ Lua VM の間は保持されるため、本編が書き、リザルトが読む。
return {
  cleared = false, -- true: ボス撃破でクリア / false: 自機の Hp 0（敗北）
}
