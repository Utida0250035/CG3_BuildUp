#include "Log.h"
#include <filesystem>
#include <chrono>
#include <Windows.h>
#include <fstream>

void Log(const std::string& message) {

	OutputDebugStringA(message.c_str());

}


void LogFile::Initialize() {

	// logsフォルダを作成
	std::filesystem::create_directory("logs");

	// 現在UTC時刻を取得
	std::chrono::system_clock::time_point nowTime = std::chrono::system_clock::now();

	// 1秒未満を切り捨て
	std::chrono::time_point < std::chrono::system_clock, std::chrono::seconds>
		nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(nowTime);

	// 日本時間(PCの設定時間)に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(), nowSeconds };

	// formatを使用して年月日_時分秒の文字列に変換
	std::string dataString = std::format("{:%Y-%m-%d_%H-%M-%S}", localTime);

	// 時刻を使ってファイル名を決定
	filePath_ = std::string("logs/") + dataString + ".log";

}

void LogFile::Log(const std::string& message) const {

	// ファイルを出力用に開く
	std::ofstream logStream(filePath_);

	// ログをファイルに出力
	logStream << message << std::endl;

	// ファイルを閉じる
	logStream.close();

	// 出力ウィンドウにログを出力
	OutputDebugStringA(message.c_str());

}

