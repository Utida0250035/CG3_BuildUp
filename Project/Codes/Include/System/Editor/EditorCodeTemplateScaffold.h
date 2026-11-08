#pragma once

#if defined(DEVELOPMENT) && defined(USE_IMGUI)

#include <string>
#include <vector>

namespace Atrum::Editor {

struct CodeTemplateManifestEntry {
  std::string id;
  std::string label;
  std::vector<std::string> requiredTokens;
  std::vector<std::pair<std::string, std::string>> outputs; // relativePath, templateFile
};

struct CodeTemplateScaffoldRequest {
  std::string templateId;
  std::string className;
  std::string updCmpGroup = "MOVEMENT";
  std::string authorNote;
  bool forceOverwrite = false;
};

struct CodeTemplateScaffoldResult {
  bool success = false;
  std::string message;
  std::vector<std::string> writtenPaths;
};

class EditorCodeTemplateScaffold {
public:
  static bool LoadManifest(std::vector<CodeTemplateManifestEntry> &outEntries,
                           std::string &error);

  static CodeTemplateScaffoldResult
  Generate(const CodeTemplateScaffoldRequest &request);

  static void DrawImGuiPanel();

private:
  static std::string SubstituteTokens(std::string text,
                                      const CodeTemplateScaffoldRequest &request);
  static bool IsValidClassName(const std::string &name);
};

} // namespace Atrum::Editor

#endif
