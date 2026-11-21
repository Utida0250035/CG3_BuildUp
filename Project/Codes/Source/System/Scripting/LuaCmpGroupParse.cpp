#include "System/Scripting/LuaCmpGroupParse.h"

#include <string>

namespace Atrum::Script {

UpdCmpGroup ParseUpdCmpGroupFromString(const std::string_view groupName) {
  if (groupName == "NONE_GROUPING") {
    return UpdCmpGroup::NONE_GROUPING;
  }
  if (groupName == "INPUT") {
    return UpdCmpGroup::INPUT;
  }
  if (groupName == "MOVEMENT") {
    return UpdCmpGroup::MOVEMENT;
  }
  if (groupName == "PHYSICS") {
    return UpdCmpGroup::PHYSICS;
  }
  if (groupName == "HIT_JUDGE") {
    return UpdCmpGroup::HIT_JUDGE;
  }
  if (groupName == "HIT_RESPONSE") {
    return UpdCmpGroup::HIT_RESPONSE;
  }
  if (groupName == "ANIMATION") {
    return UpdCmpGroup::ANIMATION;
  }
  if (groupName == "PRE_RENDER") {
    return UpdCmpGroup::PRE_RENDER;
  }
  return UpdCmpGroup::MOVEMENT;
}

DrawCmpGroup ParseDrawCmpGroupFromString(const std::string_view groupName) {
  if (groupName == "NONE" || groupName == "MESH_BODY") {
    return DrawCmpGroup::MESH_BODY;
  }
  if (groupName == "HIT_SPHERE_VISUAL") {
    return DrawCmpGroup::HIT_SPHERE_VISUAL;
  }
  if (groupName == "ATTACK_TELEGRAPH") {
    return DrawCmpGroup::ATTACK_TELEGRAPH;
  }
  if (groupName == "BULLET_VISUAL") {
    return DrawCmpGroup::BULLET_VISUAL;
  }
  return DrawCmpGroup::NONE;
}

} // namespace Atrum::Script