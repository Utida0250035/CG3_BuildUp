
#include "Engine/Alias/InputAlias.h"

#include "Cast/StaticCast.h"
#include "ForDebug/Log.h"
#include "Input/PlayInput.h"
#include <cassert>

namespace Atrum::Input {

	void PlayInput::EndOfFrame() {

		mouseWheel_ = 0;
		cursorDelta_ = M::Vector2{};

		memcpy(preMouseButtons_, mouseButtons_, sizeof(mouseButtons_));
		memcpy(preKeys_, keys_, sizeof(keys_));

	}

}