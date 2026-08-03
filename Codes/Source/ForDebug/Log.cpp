#include "ForDebug/Log.h"
#include "String/ConvertString.h"
#include <chrono>
#include <filesystem>
#include <format>
#include <Windows.h>

namespace Atrum::Debug {

	void Log([[maybe_unused]] const std::string& message) {

#ifdef _DEBUG

		OutputDebugStringA(("\n" + message + "\n\n").c_str());

#endif

	}

	void LogFile::Initialize() {

#ifdef _DEBUG

	// logsフォルダを作成
		std::filesystem::create_directory("Logs");

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

		logStream_.open(filePath_, std::ios::app);

#endif

	}

	void LogFile::Log([[maybe_unused]] const std::string& message) {

		// ログをファイルに出力
		logStream_ << "\n" << message << std::endl;
		logStream_.flush();

		// 出力ウィンドウにログを出力
		OutputDebugStringA(("\n" + message + "\n").c_str());

		std::cout << "\n" << message << std::endl;

	}

	void LogFile::Log([[maybe_unused]] const std::wstring& message) {

#ifdef _DEBUG

	// ログをファイルに出力
		logStream_ << std::endl << WStringToString(message) << std::endl;
		logStream_.flush();

		// 出力ウィンドウにログを出力
		OutputDebugStringA(("\n" + WStringToString(message) + "\n\n").c_str());

#endif

	}

	LogFile::~LogFile() {

#ifdef _DEBUG

		logStream_.close();

#endif

	}

}