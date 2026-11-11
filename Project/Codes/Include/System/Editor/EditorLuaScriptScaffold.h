#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include <string>
#include <vector>

namespace Atrum::Editor {

struct LuaScriptManifestEntry {
  std::string id;
  std::string label;
  std::vector<std::string> requiredTokens;
  std::vector<std::pair<std::string, std::string>> outputs; // relativePath, templateFile
};

struct LuaScriptScaffoldRequest {
  std::string templateId;
  std::string scriptName;
  std::string updCmpGroup = "MOVEMENT";
  std::string authorNote;
  bool forceOverwrite = false;
};

struct LuaScriptScaffoldResult {
  bool success = false;
  std::string message;
  std::vector<std::string> writtenPaths;
};

class EditorLuaScriptScaffold {
public:
  static bool LoadManifest(std::vector<LuaScriptManifestEntry> &outEntries,
                           std::string &error);

  static LuaScriptScaffoldResult
  Generate(const LuaScriptScaffoldRequest &request);

  static void DrawImGuiPanel();

private:
  static std::string SubstituteTokens(std::string text,
                                      const LuaScriptScaffoldRequest &request);
  static bool IsValidScriptName(const std::string &name);
};

} // namespace Atrum::Editor

#endif
