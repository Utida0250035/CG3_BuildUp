---@meta
-- エンティティ関連 Lua API の型定義（VS Code + Lua Language Server 用。実行はされない）。
-- 実装: Project/Codes/Source/System/Scripting/EntityLuaBinder.cpp

---@class Vec3
---@field x number
---@field y number
---@field z number
---@operator add(Vec3): Vec3
---@operator sub(Vec3): Vec3
---@operator mul(number): Vec3
---@operator unm: Vec3
local Vec3 = {}
function Vec3:length() end
---@return Vec3
function Vec3:normalized() end
---@param other Vec3
---@return number
function Vec3:dot(other) end
---@overload fun(): Vec3
---@param x number
---@param y number
---@param z number
---@return Vec3
function Vec3(x, y, z) end

---@class Quat
---@field x number
---@field y number
---@field z number
---@field w number
local Quat = {}
---@overload fun(): Quat
---@return Quat
function Quat(x, y, z, w) end
---@param euler Vec3 ラジアン
---@return Quat
function Quat.fromEuler(euler) end
---@param axis Vec3
---@param angle number ラジアン
---@return Quat
function Quat.fromAxisAngle(axis, angle) end
---@param forward Vec3
---@return Quat
function Quat.lookRotation(forward) end
---@param a Quat
---@param b Quat
---@param t number
---@return Quat
function Quat.slerp(a, b, t) end

--- エンティティハンドル。破棄済みなら isValid()==false で、他メソッドは何もしない／既定値を返す。
--- onResolve(self) / onUpdate(dt, self) の self もこの型。
---@class Entity
local Entity = {}
---@return boolean
function Entity:isValid() end
---@return string "initialize"|"sleep"|"active"|"destroy"|"invalid"
function Entity:state() end
---@return boolean
function Entity:isActive() end
---@return number
function Entity:id() end
---@param name string
---@return boolean
function Entity:isNamed(name) end
--- 破棄を要求する（フレーム内の更新走査後にシーンから除去）
function Entity:destroy() end

---@return Vec3 ローカル位置
function Entity:getPosition() end
---@param p Vec3
function Entity:setPosition(p) end
---@param delta Vec3
function Entity:translate(delta) end
---@return Vec3
function Entity:getScale() end
---@param s Vec3
function Entity:setScale(s) end
---@return Quat
function Entity:getRotation() end
---@param q Quat
function Entity:setRotation(q) end
---@param euler Vec3 ラジアン
function Entity:setRotationEuler(euler) end
---@param forward Vec3
function Entity:lookDirection(forward) end
---@return Vec3
function Entity:getWorldPosition() end
---@param local_ Vec3
---@return Vec3
function Entity:localToWorld(local_) end

---@return Entity|nil
function Entity:getParent() end
--- nil で親を外す。自分の子孫を親にする指定は無視される
---@param parent Entity|nil
function Entity:setParent(parent) end
---@return Entity[]
function Entity:children() end

---@param tag string
---@return boolean
function Entity:hasTag(tag) end
---@param tag string
function Entity:addTag(tag) end
---@param tag string
function Entity:removeTag(tag) end
--- tagRegistry.json のタグカタログに載っているタグ名のみ列挙される
---@return string[]
function Entity:tags() end
---@param role string tagRegistry.json の roles キー
---@return boolean
function Entity:hasRole(role) end

---@param typeName string 例 "CmpHitSphere"（Entities.componentTypes() の名前）
---@return boolean
function Entity:hasComponent(typeName) end
---@return string[] エディタ登録コンポーネントのうち付いているもの
function Entity:components() end

-- CmpHealthHit
---@return boolean
function Entity:hasHealth() end
---@return integer|nil
function Entity:getHealth() end
---@return integer|nil
function Entity:getMaxHealth() end
---@param value integer
function Entity:setHealth(value) end
--- 攻撃元を問わず 1 ダメージ。無敵中・HP0 なら false
---@return boolean
function Entity:damage() end
---@return boolean
function Entity:isInvincible() end

---@class SpawnOptions
---@field name? string            インスタンス名（Entities.find で引ける）
---@field position? Vec3          ローカル位置（x,y,z より先に適用）
---@field x? number
---@field y? number
---@field z? number
---@field rotation? Quat
---@field scale? Vec3
---@field parent? Entity
---@field tags? string[]          プレハブのタグに追加
---@field components? table<string, table>  型名 -> パラメータ上書き。無ければ既定値で追加

---@class Entities
Entities = {}
---@param name string
---@return Entity|nil
function Entities.find(name) end
---@param name string
---@return Entity[]
function Entities.findAll(name) end
---@param tag string
---@return Entity[]
function Entities.findByTag(tag) end
---@param tag string
---@return Entity|nil
function Entities.findFirstByTag(tag) end
---@param role string
---@return Entity[]
function Entities.findByRole(role) end
---@param role string
---@return Entity|nil
function Entities.findFirstByRole(role) end
---@return Entity[]
function Entities.all() end
--- Active なシーン直下エンティティ数
---@return integer
function Entities.count() end
--- プレハブ/シーン JSON の先頭エンティティを生成。更新走査後にシーンへ採用される
---@param prefabPath string 実行ディレクトリ基準の JSON パス（例 "../EditorAssets/Entities/enemy.json"）
---@param options? SpawnOptions
---@return Entity|nil
function Entities.spawn(prefabPath, options) end
---@param entity Entity
function Entities.destroy(entity) end
---@return string[]
function Entities.componentTypes() end
---@return string[]
function Entities.tagCatalog() end
