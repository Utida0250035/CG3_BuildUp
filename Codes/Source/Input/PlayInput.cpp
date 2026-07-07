
#include "Cast/StaticCast.h"
#include "Debug/Log.h"
#include "Input/PlayInput.h"
#include <cassert>

void PlayInput::EndOfFrame() {

	mouseWheel_ = 0;
	cursorDelta_ = Vector2{};
	
	memcpy(preMouseButtons_, mouseButtons_, sizeof(mouseButtons_));
	memcpy(preKeys_, keys_, sizeof(keys_));

}