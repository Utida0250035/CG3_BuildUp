#pragma once

#include "System/Editor/Entity/EntityEditorDefinition.h"
#include <string>
#include <vector>

namespace Atrum::EntityEditor {

/// tags から tagHashes を再構築（std::set 相当: 昇順・重複なし）
void RebuildEntityDefinitionTagHashes(EntityDefinition &definition);

/// 定義上のタグ名に対応する Hash64（0x プレースホルダはその値を返す）
uint64_t TagHashForDefinitionName(const std::string &tagName);

/// tagHashes / tags と tagCatalog から定義内のタグ表現を整合させる
void SyncEntityDefinitionTagsFromJson(EntityDefinition &definition,
                                      const std::vector<std::string> &tagCatalog);

/// シーン内の全インスタンス定義のタグを JSON フィールドどおりに整合
void SyncAllEntityDefinitionTags(EditorSceneDocument &document);

/// Assets/Tags/tagRegistry.json の tagCatalog をシーン tagCatalog にマージ
void MergeTagCatalogFromRegistry(EditorSceneDocument &document);

/// シーン内のインスタンス定義から tagCatalog を補完する
void SyncTagCatalogFromInstances(EditorSceneDocument &document);

bool TagCatalogContains(const EditorSceneDocument &document,
                        const std::string &tagName);

/// カタログへ追加（重複名は追加しない）
bool AddTagToCatalog(EditorSceneDocument &document, const std::string &tagName);

/// カタログと全インスタンスのタグ名を一括改名
bool RenameTagInDocument(EditorSceneDocument &document,
                         const std::string &fromName,
                         const std::string &toName);

/// カタログと全インスタンスからタグを削除
void RemoveTagFromDocument(EditorSceneDocument &document,
                           const std::string &tagName);

void AddTagToEntityDefinition(EntityDefinition &definition,
                              const std::string &tagName);
void RemoveTagFromEntityDefinition(EntityDefinition &definition,
                                   const std::string &tagName);

/// エンティティ定義上の 1 タグ名を編集（ハッシュ列も更新）
bool EditEntityDefinitionTagAt(EntityDefinition &definition, size_t tagIndex,
                               const std::string &newTagName);

/// 検索文字列を部分一致（大文字小文字無視）で tagCatalog を絞り込む
std::vector<std::string>
FilterTagCatalog(const EditorSceneDocument &document,
                 const std::string &searchText);

} // namespace Atrum::EntityEditor
