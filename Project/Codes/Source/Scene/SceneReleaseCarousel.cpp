#include "Scene/SceneReleaseCarousel.h"

#include "Input/AbstractInput.h"
#include "Scene/ChangeSceneFade.h"
#include "Scene/CommandChangeScene.h"
#include "Scene/CommandScene.h"
#include "Scene/SceneBuildUp.h"
#include "Scene/SceneGame.h"
#include "Scene/SceneResult.h"
#include "Scene/SceneTitle.h"
#include "System/GameAssetsPaths.h"

#include <algorithm>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>

namespace Atrum::SceneFlow {

struct ReleaseCarouselEntry {
  enum class Kind { Builtin, BuildUp };

  Kind kind = Kind::Builtin;
  std::string builtinId{};
  std::string buildUpPath{};
};

namespace {

using EntryKind = ReleaseCarouselEntry::Kind;
using Entry = ReleaseCarouselEntry;

std::string NormalizePathKey(std::string path) {
  namespace fs = std::filesystem;
  try {
    path = fs::weakly_canonical(fs::path(path)).generic_string();
  } catch (...) {
    std::replace(path.begin(), path.end(), '\\', '/');
  }
  return path;
}

void LoadDefaultEntries(std::vector<Entry> &out) {
  out = {
      Entry{EntryKind::BuildUp, {},
            NormalizePathKey(std::string(GameAssets::kScenesDirectory) +
                             "/ParticleDemo.json")},
      Entry{EntryKind::BuildUp, {},
            NormalizePathKey(std::string(GameAssets::kScenesDirectory) +
                             "/BillboardInstancing100.json")},
  };
}

bool TryLoadManifest(std::vector<Entry> &out) {
  const std::string manifestPath = GameAssets::kReleaseSceneCarouselPath;
  namespace fs = std::filesystem;
  if (!fs::exists(manifestPath)) {
    return false;
  }

  std::ifstream file(manifestPath);
  if (!file.is_open()) {
    return false;
  }

  nlohmann::json json{};
  file >> json;
  if (!json.contains("entries") || !json.at("entries").is_array()) {
    return false;
  }

  out.clear();
  for (const nlohmann::json &item : json.at("entries")) {
    Entry entry{};
    const std::string type = item.value("type", std::string{"builtin"});
    if (type == "buildUp") {
      entry.kind = EntryKind::BuildUp;
      entry.buildUpPath =
          NormalizePathKey(item.value("sceneJsonPath", std::string{}));
      if (!entry.buildUpPath.empty()) {
        out.push_back(std::move(entry));
      }
    } else {
      entry.kind = EntryKind::Builtin;
      entry.builtinId = item.value("scene", std::string{});
      if (!entry.builtinId.empty()) {
        out.push_back(std::move(entry));
      }
    }
  }
  return !out.empty();
}

} // namespace

SceneReleaseCarousel::SceneReleaseCarousel() {
  if (!TryLoadManifest(entries_)) {
    LoadDefaultEntries(entries_);
  }
}

SceneReleaseCarousel &SceneReleaseCarousel::Instance() {
  static SceneReleaseCarousel instance;
  return instance;
}

int SceneReleaseCarousel::FindIndexForScene(Scene *scene) const {
  if (scene == nullptr) {
    return activeIndex_;
  }

  if (scene == SceneTitle::Instance()) {
    for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
      if (entries_[static_cast<size_t>(i)].kind == EntryKind::Builtin &&
          entries_[static_cast<size_t>(i)].builtinId == "Title") {
        return i;
      }
    }
  }
  if (scene == SceneGame::Instance()) {
    for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
      if (entries_[static_cast<size_t>(i)].kind == EntryKind::Builtin &&
          entries_[static_cast<size_t>(i)].builtinId == "Game") {
        return i;
      }
    }
  }
  if (scene == SceneResult::Instance()) {
    for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
      if (entries_[static_cast<size_t>(i)].kind == EntryKind::Builtin &&
          entries_[static_cast<size_t>(i)].builtinId == "Result") {
        return i;
      }
    }
  }

  if (auto *buildUp = dynamic_cast<SceneBuildUp *>(scene)) {
    const std::string key = NormalizePathKey(buildUp->SceneJsonPath());
    for (int i = 0; i < static_cast<int>(entries_.size()); ++i) {
      if (entries_[static_cast<size_t>(i)].kind == EntryKind::BuildUp &&
          entries_[static_cast<size_t>(i)].buildUpPath == key) {
        return i;
      }
    }
  }

  return activeIndex_;
}

void SceneReleaseCarousel::SyncIndexFromActiveScene(Scene *sceneNow) {
  activeIndex_ = FindIndexForScene(sceneNow);
}

bool SceneReleaseCarousel::HasBuildUpEntry(
    const std::string &sceneJsonPath) const {
  const std::string key = NormalizePathKey(sceneJsonPath);
  return std::any_of(entries_.begin(), entries_.end(),
                     [&key](const Entry &entry) {
                       return entry.kind == EntryKind::BuildUp &&
                              entry.buildUpPath == key;
                     });
}

Scene *SceneReleaseCarousel::ResolveSceneForIndex(const int index) const {
  if (entries_.empty()) {
    return SceneTitle::Instance();
  }

  const Entry &entry = entries_[static_cast<size_t>(
      (index % static_cast<int>(entries_.size()) +
       static_cast<int>(entries_.size())) %
      static_cast<int>(entries_.size()))];

  if (entry.kind == EntryKind::Builtin) {
    if (entry.builtinId == "Game") {
      return SceneGame::Instance();
    }
    if (entry.builtinId == "Result") {
      return SceneResult::Instance();
    }
    return SceneTitle::Instance();
  }

  return SceneBuildUpRegistry::Instance().Launch(entry.buildUpPath,
                                                 SceneTitle::Instance());
}

bool SceneReleaseCarousel::AdvanceToNextScene() {
  if (entries_.empty()) {
    return false;
  }

  activeIndex_ = (activeIndex_ + 1) % static_cast<int>(entries_.size());
  Scene *next = ResolveSceneForIndex(activeIndex_);
  if (next == nullptr) {
    return false;
  }

  CommandChangeScene::Instance()->Set(
      next, std::make_unique<ChangeSceneFade>().release());
  return true;
}

bool SceneReleaseCarousel::TryAdvanceOnSubmit() {
  if (CommandChangeScene::Instance()->GetIsExecute()) {
    return false;
  }
  if (!AbstractInput::Instance()->GetData().isSubmitTrigger) {
    return false;
  }
  SyncIndexFromActiveScene(CommandScene::Instance()->GetSceneNow());
  return AdvanceToNextScene();
}

} // namespace Atrum::SceneFlow
