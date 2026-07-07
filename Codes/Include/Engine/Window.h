#pragma once
#include "Debug/Log.h"
#include <SDL.h>

struct Window {
	SDL_Window* ptr = nullptr;

	~Window() {

		if (ptr) {

			SDL_DestroyWindow(ptr);

		}

		SDL_Quit();

		LogFile::GetInstance()->Log("SDL2: Quit");

	}

};