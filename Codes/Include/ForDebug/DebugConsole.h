#pragma once

#ifdef _DEBUG

#include <Windows.h>
#include <iostream>
#include <format>

#endif

namespace Atrum::Debug {

	void OpenDebugConsole()
	{

#ifdef _DEBUG

		AllocConsole();

		FILE* fp;

		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
		freopen_s(&fp, "CONIN$", "r", stdin);

		std::cout.clear();
		std::cerr.clear();
		std::cin.clear();

		std::cout << "Debug Console Opened." << std::endl;

#endif

	}

}