#include "System/Editor/Entity/EntityEditorTagCatalog.h"

#include "System/Entity/EntityTagRegistry.h"
#include "System/Hash64.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <set>

namespace Atrum::EntityEditor {

namespace {

std::string ToLowerAscii(std::string value) {
  for (char &ch : value) {
    ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
  }
  return value;
}

bool ContainsTag(const std::vector<std::string> &tags, const std::string &name) {
  return std::find(tags.begin(), tags.end(), name) != tags.end();
}

std::string TagDisplayNameFromHash(const uint64_t tagHash,
                                   const std::vector<std::string> &tagCatalog) {
  for (const std::string &name : tagCatalog) {
    if (Hash64(name) == tagHash) {
      return name;
    }
  }
  char buffer[32]{};
  std::snprintf(buffer, sizeof(buffer), "0x%016llX",
                static_cast<unsigned long long>(tagHash));
  return buffer;
}

bool IsHexPlaceholderTagName(const std::string &name) {
  return name.size() == 18 && name.rfind("0x", 0) == 0;
}

bool TryParseHexTagPlaceholder(const std::string &name, uint64_t &outHash) {
  if (!IsHexPlaceholderTagName(name)) {
    return false;
  }
  const std::string digits = name.substr(2);
  try {
    outHash = std::stoull(digits, nullptr, 16);
    return true;
  } catch (...) {
    return false;
  }
}

} // namespace

uint64_t TagHashForDefinitionName(const std::string &tagName) {
  uint64_t parsedHash = 0;
  if (TryParseHexTagPlaceholder(tagName, parsedHash)) {
    return parsedHash;
  }
  return Hash64(tagName);
}

void RebuildEntityDefinitionTagHashes(EntityDefinition &definition) {
  std::set<uint64_t> uniqueHashes{};
  for (const std::string &tagName : definition.tags) {
    if (tagName.empty()) {
      continue;
    }
    uint64_t parsedHash = 0;
    if (TryParseHexTagPlaceholder(tagName, parsedHash)) {
      uniqueHashes.insert(parsedHash);
      continue;
    }
    uniqueHashes.insert(Hash64(tagName));
  }
  definition.tagHashes.assign(uniqueHashes.begin(), uniqueHashes.end());
}

void SyncEntityDefinitionTagsFromJson(EntityDefinition &definition,
                                      const std::vector<std::string> &tagCatalog) {
  const bool hasHashes = !definition.tagHashes.empty();
  const bool hasNames = !definition.tags.empty();

  if (hasNames) {
    RebuildEntityDefinitionTagHashes(definition);
    return;
  }

  if (hasHashes) {
    std::set<uint64_t> uniqueHashes(definition.tagHashes.begin(),
                                    definition.tagHashes.end());
    definition.tagHashes.assign(uniqueHashes.begin(), uniqueHashes.end());
    definition.tags.clear();
    definition.tags.reserve(definition.tagHashes.size());
    for (const uint64_t tagHash : definition.tagHashes) {
      definition.tags.push_back(TagDisplayNameFromHash(tagHash, tagCatalog));
    }
  }
}

void SyncAllEntityDefinitionTags(EditorSceneDocument &document) {
  for (SceneEntityInstance &instance : document.instances) {
    SyncEntityDefinitionTagsFromJson(instance.definition, document.tagCatalog);
  }
}

void MergeTagCatalogFromRegistry(EditorSceneDocument &document) {
  EntityTagRegistry &registry = EntityTagRegistry::Instance();
  registry.EnsureLoaded();
  for (const std::string &tagName : registry.TagCatalog()) {
    AddTagToCatalog(document, tagName);
  }
}

void SyncTagCatalogFromInstances(EditorSceneDocument &document) {
  for (const SceneEntityInstance &instance : document.instances) {
    for (const std::string &tag : instance.definition.tags) {
      if (tag.empty()) {
        continue;
      }
      if (!TagCatalogContains(document, tag)) {
        document.tagCatalog.push_back(tag);
      }
    }
  }
  std::sort(document.tagCatalog.begin(), document.tagCatalog.end());
  document.tagCatalog.erase(
      std::unique(document.tagCatalog.begin(), document.tagCatalog.end()),
      document.tagCatalog.end());
}

bool TagCatalogContains(const EditorSceneDocument &document,
                        const std::string &tagName) {
  if (tagName.empty()) {
    return false;
  }
  return ContainsTag(document.tagCatalog, tagName);
}

bool AddTagToCatalog(EditorSceneDocument &document, const std::string &tagName) {
  if (tagName.empty() || TagCatalogContains(document, tagName)) {
    return false;
  }
  document.tagCatalog.push_back(tagName);
  std::sort(document.tagCatalog.begin(), document.tagCatalog.end());
  return true;
}

bool RenameTagInDocument(EditorSceneDocument &document,
                         const std::string &fromName,
                         const std::string &toName) {
  if (fromName.empty() || toName.empty() || fromName == toName) {
    return false;
  }
  if (!TagCatalogContains(document, fromName)) {
    return false;
  }
  if (TagCatalogContains(document, toName)) {
    return false;
  }

  for (std::string &catalogTag : document.tagCatalog) {
    if (catalogTag == fromName) {
      catalogTag = toName;
      break;
    }
  }

  for (SceneEntityInstance &instance : document.instances) {
    for (std::string &tag : instance.definition.tags) {
      if (tag == fromName) {
        tag = toName;
      }
    }
    RebuildEntityDefinitionTagHashes(instance.definition);
  }

  std::sort(document.tagCatalog.begin(), document.tagCatalog.end());
  return true;
}

void RemoveTagFromDocument(EditorSceneDocument &document,
                           const std::string &tagName) {
  if (tagName.empty()) {
    return;
  }

  document.tagCatalog.erase(
      std::remove(document.tagCatalog.begin(), document.tagCatalog.end(),
                  tagName),
      document.tagCatalog.end());

  for (SceneEntityInstance &instance : document.instances) {
    RemoveTagFromEntityDefinition(instance.definition, tagName);
  }
}

void AddTagToEntityDefinition(EntityDefinition &definition,
                              const std::string &tagName) {
  if (tagName.empty() || ContainsTag(definition.tags, tagName)) {
    return;
  }
  definition.tags.push_back(tagName);
  RebuildEntityDefinitionTagHashes(definition);
}

void RemoveTagFromEntityDefinition(EntityDefinition &definition,
                                   const std::string &tagName) {
  definition.tags.erase(
      std::remove(definition.tags.begin(), definition.tags.end(), tagName),
      definition.tags.end());
  RebuildEntityDefinitionTagHashes(definition);
}

bool EditEntityDefinitionTagAt(EntityDefinition &definition,
                               const size_t tagIndex,
                               const std::string &newTagName) {
  if (newTagName.empty() || tagIndex >= definition.tags.size()) {
    return false;
  }
  if (ContainsTag(definition.tags, newTagName) &&
      definition.tags[tagIndex] != newTagName) {
    return false;
  }
  definition.tags[tagIndex] = newTagName;
  RebuildEntityDefinitionTagHashes(definition);
  return true;
}

std::vector<std::string>
FilterTagCatalog(const EditorSceneDocument &document,
                 const std::string &searchText) {
  if (searchText.empty()) {
    return document.tagCatalog;
  }

  const std::string needle = ToLowerAscii(searchText);
  std::vector<std::string> filtered;
  filtered.reserve(document.tagCatalog.size());

  for (const std::string &tag : document.tagCatalog) {
    if (ToLowerAscii(tag).find(needle) != std::string::npos) {
      filtered.push_back(tag);
    }
  }
  return filtered;
}

} // namespace Atrum::EntityEditor
