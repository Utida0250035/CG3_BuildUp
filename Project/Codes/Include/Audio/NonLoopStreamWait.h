#pragma once

#include <cstdint>

namespace Atrum::Audio {

	// 空キューで再読込に失敗した回数がこの値に達したら、非ループの Waiting を終了する。
	inline constexpr std::uint8_t kNonLoopEmptyRefillStopMisses = 3;

	// 非ループのストリームが Waiting のとき、即停止せず再読込の失敗回数を数える。
	struct NonLoopStreamWait {

		std::uint8_t emptyRefillMisses = 0;

		void Reset() {
			emptyRefillMisses = 0;
		}

		void NoteBuffersRemain() {
			emptyRefillMisses = 0;
		}

		void NoteReadSucceeded() {
			emptyRefillMisses = 0;
		}

		// 空キューでの失敗を1回足す。閾値に達したら true（終了してよい）。
		bool NoteEmptyReadFailed() {
			if (emptyRefillMisses < kNonLoopEmptyRefillStopMisses) {
				++emptyRefillMisses;
			}
			return emptyRefillMisses >= kNonLoopEmptyRefillStopMisses;
		}

	};

}
