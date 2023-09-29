#pragma once
#define HAVE_IRCD_IOS_EMPT_H

/// Emption interface.
///
/// On supporting systems and with the cooperation of libircd's embedder these
/// items can aid with optimizing and/or profiling the boost::asio core event
/// loop. See epoll.h for an epoll_wait(2) use of these items. This is a
/// separate header/namespace so this can remain abstract and applied to
/// different platforms.
///
namespace ircd::ios::empt
{
	// extern conf::item<uint64_t> freq;

	// extern stats::item<uint64_t *> peek;
	// extern stats::item<uint64_t *> skip;
	// extern stats::item<uint64_t *> call;
	// extern stats::item<uint64_t *> none;
	// extern stats::item<uint64_t *> result;
	// extern stats::item<uint64_t *> load_low;
	// extern stats::item<uint64_t *> load_med;
	// extern stats::item<uint64_t *> load_high;
	// extern stats::item<uint64_t *> load_stall;

	extern uint64_t freq;

	extern uint64_t peek;
	extern uint64_t skip;
	extern uint64_t call;
	extern uint64_t none;
	extern uint64_t result;
	extern uint64_t load_low;
	extern uint64_t load_med;
	extern uint64_t load_high;
	extern uint64_t load_stall;
}
