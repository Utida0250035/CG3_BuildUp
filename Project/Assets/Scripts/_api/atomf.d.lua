---@meta
-- atomF のステージ進行用 API（LuaLS 型定義）。実体は AtomFLuaBinder（C++）。

---@class AtomFApi
AtomF = {}

--- 雑魚を出す。variantName は parameters.json の enemy.variants の name。未定義なら先頭の種類。
---@param variantName string
---@param x number
---@param z number
function AtomF.spawnEnemy(variantName, x, z) end

--- ボスを出す（すでに居れば無視）。
---@param x number
---@param z number
function AtomF.spawnBoss(x, z) end

--- 雑魚の種類名の配列。
---@return string[]
function AtomF.variantNames() end

--- アリーナ内径（m）。
---@return number
function AtomF.arenaRadius() end

--- 自機の x, z。居なければ nil, nil。
---@return number|nil x
---@return number|nil z
function AtomF.playerPosition() end

--- ボスの現在 Hp。居なければ -1。
---@return integer
function AtomF.bossHealth() end

---@return integer
function AtomF.bossMaxHealth() end

---@return boolean
function AtomF.bossPhase2() end

--- "none" | "appear" | "fight" | "stunned" | "dying"
---@return string
function AtomF.bossState() end
