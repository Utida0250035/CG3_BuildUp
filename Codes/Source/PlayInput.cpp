
#include "Log.h"
#include "PlayInput.h"
#include "StaticCast.h"
#include <cassert>

void PlayInput::EndOfFrame() {

	mouseWheel_ = 0;
	cursorDelta_ = Vector2{};
	
	memcpy(preMouseButtons_, mouseButtons_, sizeof(mouseButtons_));
	memcpy(preKeys_, keys_, sizeof(keys_));

}