#pragma once

#include <string>

void Log(const std::string& message);

class LogFile {

private:

	// ログ出力先ファイルのパス
	std::string filePath_;

	/// <summary>
	/// コンストラクタ
	/// </summary>
	LogFile() = default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~LogFile() = default;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ログ出力
	/// </summary>
	/// <param name="message"></param>
	void Log(const std::string& message) const;

	/// <summary>
	/// インスタンス取得
	/// </summary>
	/// <returns></returns>
	static LogFile* GetInstance() {

		static LogFile instance;

		return &instance;

	}

	/// <summary>
	/// 代入演算子の削除
	/// </summary>
	/// <param name="source"></param>
	/// <returns></returns>
	LogFile operator=(const LogFile& source) = delete;

	/// <summary>
	/// コピーコンストラクタの削除
	/// </summary>
	/// <param name="source"></param>
	LogFile(const LogFile& source) = delete;

};
