#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include "System/Editor/EditorLuaScriptScaffold.h"

#include "System/Editor/EditorAssetsPaths.h"
#include "ForDebug/ImGui.h"
#include <nlohmann/json.hpp>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <filesystem>

namespace Atrum::Editor {

namespace fs = std::filesystem;

namespace {

constexpr const char *kTemplateRoot = "../EditorAssets/Templates/Lua";
constexpr const char *kManifestFile = "manifest.json";

fs::path TemplateRootPath() { return fs::path(kTemplateRoot); }

fs::path ManifestPath() { return TemplateRootPath() / kManifestFile; }

fs::path OutputPathFromManifest(const std::string &relative) {
  constexpr std::string_view kProjectPrefix = "Project/";
  if (relative.starts_with(kProjectPrefix)) {
    return fs::path(relative.substr(kProjectPrefix.size()));
  }
  return fs::path(relative);
}

void ReplaceAll(std::string &text, const std::string &from,
                const std::string &to) {
  if (from.empty()) {
    return;
  }
  size_t pos = 0;
  while ((pos = text.find(from, pos)) != std::string::npos) {
    text.replace(pos, from.size(), to);
    pos += to.size();
  }
}

std::vector<std::string> SplitLines(const std::string &text) {
  std::vector<std::string> lines;
  size_t start = 0;
  while (start <= text.size()) {
    const size_t end = text.find('\n', start);
    if (end == std::string::npos) {
      lines.emplace_back(text.substr(start));
      break;
    }
    lines.emplace_back(text.substr(start, end - start));
    start = end + 1;
  }
  return lines;
}

std::string StripAuthorNotePlaceholder(std::string text) {
  std::string out;
  out.reserve(text.size());
  for (const std::string &line : SplitLines(text)) {
    if (line.find("-- %AuthorNote%") != std::string::npos) {
      continue;
    }
    out.append(line);
    out.push_back('\n');
  }
  return out;
}

} // namespace

bool EditorLuaScriptScaffold::IsValidScriptName(const std::string &name) {
  if (name.empty()) {
    return false;
  }
  if (!std::isalpha(static_cast<unsigned char>(name.front()))) {
    return false;
  }
  for (const char c : name) {
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') {
      return false;
    }
  }
  return true;
}

bool EditorLuaScriptScaffold::LoadManifest(
    std::vector<LuaScriptManifestEntry> &outEntries, std::string &error) {
  outEntries.clear();
  const fs::path path = ManifestPath();
  if (!fs::is_regular_file(path)) {
    error = "manifest not found: " + path.string();
    return false;
  }

  std::ifstream stream(path);
  if (!stream) {
    error = "cannot open manifest";
    return false;
  }

  nlohmann::json json;
  try {
    stream >> json;
  } catch (const std::exception &ex) {
    error = ex.what();
    return false;
  }

  for (const auto &item : json.at("templates")) {
    LuaScriptManifestEntry entry{};
    entry.id = item.at("id").get<std::string>();
    entry.label = item.value("label", entry.id);
    if (item.contains("requiredTokens")) {
      for (const auto &token : item.at("requiredTokens")) {
        entry.requiredTokens.push_back(token.get<std::string>());
      }
    }
    for (const auto &out : item.at("outputs")) {
      entry.outputs.emplace_back(out.at("relativePath").get<std::string>(),
                                 out.at("templateFile").get<std::string>());
    }
    outEntries.push_back(std::move(entry));
  }
  return true;
}

std::string EditorLuaScriptScaffold::SubstituteTokens(
    std::string text, const LuaScriptScaffoldRequest &request) {
  ReplaceAll(text, "%ScriptName%", request.scriptName);
  ReplaceAll(text, "%UpdCmpGroup%", request.updCmpGroup);
  if (request.authorNote.empty()) {
    ReplaceAll(text, "%AuthorNote%", "");
  } else {
    ReplaceAll(text, "%AuthorNote%", request.authorNote);
  }
  return text;
}

LuaScriptScaffoldResult
EditorLuaScriptScaffold::Generate(const LuaScriptScaffoldRequest &request) {
  LuaScriptScaffoldResult result{};

  if (!IsValidScriptName(request.scriptName)) {
    result.message = "ScriptName は英字始まりの英数字と _ のみ";
    return result;
  }

  std::vector<LuaScriptManifestEntry> entries;
  std::string loadError;
  if (!LoadManifest(entries, loadError)) {
    result.message = loadError;
    return result;
  }

  const LuaScriptManifestEntry *selected = nullptr;
  for (const auto &entry : entries) {
    if (entry.id == request.templateId) {
      selected = &entry;
      break;
    }
  }
  if (selected == nullptr) {
    result.message = "未知の template id";
    return result;
  }

  for (const auto &required : selected->requiredTokens) {
    if (required == "ScriptName" && request.scriptName.empty()) {
      result.message = "ScriptName が空です";
      return result;
    }
    if (required == "UpdCmpGroup" && request.updCmpGroup.empty()) {
      result.message = "UpdCmpGroup が空です";
      return result;
    }
  }

  const fs::path templateRoot = TemplateRootPath();

  struct PlannedWrite {
    fs::path path;
    std::string body;
  };
  std::vector<PlannedWrite> planned;

  for (const auto &[relPath, templateFile] : selected->outputs) {
    const fs::path templatePath = templateRoot / templateFile;
    if (!fs::is_regular_file(templatePath)) {
      result.message = "template missing: " + templatePath.string();
      return result;
    }

    std::ifstream in(templatePath);
    std::string body((std::istreambuf_iterator<char>(in)),
                     std::istreambuf_iterator<char>());
    const std::string rel = SubstituteTokens(relPath, request);
    const fs::path outPath = OutputPathFromManifest(rel);
    const std::string outString = outPath.generic_string();
    if (outPath.is_absolute() || outString.find("..") != std::string::npos) {
      result.message = "invalid output path";
      return result;
    }

    body = SubstituteTokens(body, request);
    if (request.authorNote.empty()) {
      body = StripAuthorNotePlaceholder(body);
    }
    planned.push_back({outPath, body});
  }

  for (const auto &item : planned) {
    if (fs::exists(item.path) && !request.forceOverwrite) {
      result.message =
          "既存ファイルがあります（上書きにチェック）: " + item.path.string();
      return result;
    }
  }

  for (const auto &item : planned) {
    fs::create_directories(item.path.parent_path());
    std::ofstream out(item.path);
    if (!out) {
      result.message = "書き込み失敗: " + item.path.string();
      return result;
    }
    out << item.body;
    result.writtenPaths.push_back(item.path.string());
  }

  result.success = true;
  result.message =
      "Lua スクリプトを生成しました（Phase 2 でランタイム読込予定）。";
  return result;
}

void EditorLuaScriptScaffold::DrawImGuiPanel() {
  static std::vector<LuaScriptManifestEntry> entries;
  static bool loaded = false;
  static std::string loadError;
  static int templateIndex = 0;
  static char scriptNameBuf[128] = "my_component";
  static char authorNoteBuf[256] = "";
  static int updGroupIndex = 2;
  static bool forceOverwrite = false;
  static std::string statusMessage;

  static const char *kUpdGroups[] = {"NONE_GROUPING", "INPUT",       "MOVEMENT",
                                     "PHYSICS",       "HIT_JUDGE",   "HIT_RESPONSE",
                                     "ANIMATION",     "PRE_RENDER"};

  if (!loaded) {
    loaded = LoadManifest(entries, loadError);
  }

  if (!loaded) {
    ImGui::TextColored(ImVec4(1, 0.4f, 0.4f, 1), "%s", loadError.c_str());
    return;
  }

  if (templateIndex < 0 || templateIndex >= static_cast<int>(entries.size())) {
    templateIndex = 0;
  }

  ImGui::TextDisabled("出力先: Project/%s", EditorAssets::kScriptsDirectory);

  if (ImGui::BeginCombo("テンプレート",
                        entries[static_cast<size_t>(templateIndex)].label.c_str())) {
    for (int i = 0; i < static_cast<int>(entries.size()); ++i) {
      const bool selected = i == templateIndex;
      if (ImGui::Selectable(entries[static_cast<size_t>(i)].label.c_str(),
                            selected)) {
        templateIndex = i;
      }
    }
    ImGui::EndCombo();
  }

  ImGui::InputText("ScriptName", scriptNameBuf, sizeof(scriptNameBuf));

  const auto &current = entries[static_cast<size_t>(templateIndex)];
  const bool needsUpdGroup =
      std::find(current.requiredTokens.begin(), current.requiredTokens.end(),
                "UpdCmpGroup") != current.requiredTokens.end();
  if (needsUpdGroup) {
    if (ImGui::BeginCombo("UpdCmpGroup", kUpdGroups[updGroupIndex])) {
      for (int i = 0; i < IM_ARRAYSIZE(kUpdGroups); ++i) {
        const bool selected = i == updGroupIndex;
        if (ImGui::Selectable(kUpdGroups[i], selected)) {
          updGroupIndex = i;
        }
      }
      ImGui::EndCombo();
    }
  }

  ImGui::InputText("AuthorNote（任意）", authorNoteBuf, sizeof(authorNoteBuf));
  ImGui::Checkbox("既存を上書き", &forceOverwrite);

  if (ImGui::Button("生成")) {
    LuaScriptScaffoldRequest req{};
    req.templateId = current.id;
    req.scriptName = scriptNameBuf;
    req.updCmpGroup = kUpdGroups[updGroupIndex];
    req.authorNote = authorNoteBuf;
    req.forceOverwrite = forceOverwrite;
    const LuaScriptScaffoldResult gen = Generate(req);
    statusMessage = gen.message;
    if (gen.success) {
      for (const auto &path : gen.writtenPaths) {
        statusMessage += "\n- " + path;
      }
    }
  }

  if (!statusMessage.empty()) {
    ImGui::Separator();
    ImGui::TextWrapped("%s", statusMessage.c_str());
  }
}

} // namespace Atrum::Editor

#endif
