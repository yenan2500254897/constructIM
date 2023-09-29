/// Logging facility
// decltype(ircd::ios::log)
// ircd::ios::log
// {
// 	"ios"
// };

/// "main" thread for IRCd; the one the main context landed on.
decltype(ircd::ios::main_thread_id)
ircd::ios::main_thread_id
{
	std::this_thread::get_id()
};

/// True only for the main thread.
decltype(ircd::ios::is_main_thread)
thread_local
ircd::ios::is_main_thread;

/// The embedder/executable's (library user) provided executor handle.
decltype(ircd::ios::user)
ircd::ios::user;

/// Our library's execution strand instance.
decltype(ircd::ios::primary)
ircd::ios::primary;

/// Our library-specific/isolate executor handle.
decltype(ircd::ios::main)
ircd::ios::main;

/// Indicates the user asio::executor is initialized.
decltype(ircd::ios::user_available)
ircd::ios::user_available;

/// Indicates the main asio::executor is initialized.
decltype(ircd::ios::main_available)
ircd::ios::main_available;

// decltype(ircd::boost_version_api)
// ircd::boost_version_api
// {
// 	"boost", info::versions::API, BOOST_VERSION,
// 	{
// 		BOOST_VERSION / 100000,
// 		BOOST_VERSION / 100 % 1000,
// 		BOOST_VERSION % 100,
// 	}
// };

// decltype(ircd::boost_version_abi)
// ircd::boost_version_abi
// {
// 	"boost", info::versions::ABI //TODO: get this
// };

[[gnu::visibility("hidden")]]
void
ircd::ios::init(asio::executor &&user)
{
	// Sample the ID of this thread. Since this is the first transfer of
	// control to libircd after static initialization we have nothing to
	// consider a main thread yet. We need something set for many assertions
	// to pass.
	main_thread_id = std::this_thread::get_id();

	// Set the thread-local bit to true; all other threads will see false.
	is_main_thread = true;

	// Save a reference handle to the user's executor.
	ios::user = user;
	ios::user_available = bool(ios::user);

	// Create our strand instance.
	ios::primary.emplace(static_cast<asio::io_context &>(user.context()));

	// Set the reference handle to our executor.
	ios::main = *ios::primary;
	ios::main_available = bool(ios::main);
}

[[using gnu: cold]]
void
ircd::ios::exiting_quick()
noexcept
{
	if(run::level == run::level::HALT)
		return;

	// if(ircd::debugmode)
	// 	log::logf
	// 	{
	// 		log::star, log::level::DEBUG,
	// 		"IRCd notified of quick exit() in runlevel:%s ctx:%p",
	// 		reflect(run::level),
	// 		(const void *)ctx::current,
	// 	};
}

[[using gnu: cold]]
void
ircd::ios::exiting()
noexcept
{
	if(run::level == run::level::HALT)
		return;

	// if(ircd::debugmode)
	// 	log::logf
	// 	{
	// 		log::star, log::level::DEBUG,
	// 		"IRCd notified of exit() in runlevel:%s ctx:%p",
	// 		reflect(run::level),
	// 		(const void *)ctx::current,
	// 	};
}

[[using gnu: cold]]
void
ircd::ios::continuing()
noexcept
{
	if(run::level == run::level::HALT)
		return;

	// log::logf
	// {
	// 	log::star, log::level::DEBUG,
	// 	"IRCd notified of SIGCONT in runlevel:%s ctx:%p",
	// 	reflect(run::level),
	// 	(const void *)ctx::current,
	// };

	switch(run::level)
	{
		case run::level::RUN:
			break;

		default:
			return;
	}

	// log::notice
	// {
	// 	log::star, "IRCd resuming service in runlevel %s.",
	// 	reflect(run::level),
	// };
}

[[using gnu: cold]]
void
ircd::ios::forking()
{
	#if BOOST_VERSION >= 107000 && BOOST_VERSION < 107400
		get().context().notify_fork(asio::execution_context::fork_prepare);
	#else
		get().notify_fork(asio::execution_context::fork_prepare);
	#endif
}

[[using gnu: cold]]
void
ircd::ios::forked_child()
{
	#if BOOST_VERSION >= 107000 && BOOST_VERSION < 107400
		get().context().notify_fork(asio::execution_context::fork_child);
	#else
		get().notify_fork(asio::execution_context::fork_child);
	#endif
}

[[using gnu: cold]]
void
ircd::ios::forked_parent()
{
	#if BOOST_VERSION >= 107000 && BOOST_VERSION < 107400
		get().context().notify_fork(asio::execution_context::fork_parent);
	#else
		get().notify_fork(asio::execution_context::fork_parent);
	#endif
}

//
// emption
//

namespace ircd::ios::empt
{
	[[gnu::visibility("internal")]]
	extern const string_view freq_help;

	[[gnu::visibility("internal")]]
	extern uint64_t stats[9];

}

decltype(ircd::ios::empt::freq_help)
ircd::ios::empt::freq_help
{R"(
	Coarse frequency to make non-blocking polls to the kernel for events at the
	beginning of every iteration of the core event loop. boost::asio takes an
	opportunity to first make a non-blocking poll to gather more events from
	the kernel even when one or more tasks are already queued, this setting
	allows more such tasks to first be executed and reduce syscall overhead
	including a large number of unnecessary calls as would be the case by
	default without this.

	When the frequency is set to 1, the above-described default behavior is
	unaltered. When greater than 1, voluntary non-blocking polls are only made
	after N number of tasks. This reduces syscalls to increase overall
	performance, but may cost in responsiveness and cause stalls. For example,
	when set to 2, kernel context-switch is made every other userspace context
	switch. When set to 0, voluntary non-blocking polls are never made.

	This value may be rounded down to nearest base2 so we can avoid invoking
	the FPU in the core event loop's codepath.
)"};

decltype(ircd::ios::empt::stats)
ircd::ios::empt::stats;

uint64_t ircd::ios::empt::freq = 512;
// /// Voluntary kernel poll frequency.
// decltype(ircd::ios::empt::freq)
// ircd::ios::empt::freq
// {
// 	{ "name",      "ircd.ios.empt.freq" },
// 	{ "default",   512                  },
// 	{ "help",      freq_help            },
// };

uint64_t ircd::ios::empt::peek = 0;
// /// Non-blocking call count.
// decltype(ircd::ios::empt::peek)
// ircd::ios::empt::peek
// {
// 	stats + 0,
// 	{
// 		{ "name", "ircd.ios.empt.peek" },
// 	},
// };

uint64_t ircd::ios::empt::skip = 0;
// /// Skipped call count.
// decltype(ircd::ios::empt::skip)
// ircd::ios::empt::skip
// {
// 	stats + 1,
// 	{
// 		{ "name", "ircd.ios.empt.skip" },
// 	},
// };

uint64_t ircd::ios::empt::call = 0;
// /// Non-skipped call count.
// decltype(ircd::ios::empt::call)
// ircd::ios::empt::call
// {
// 	stats + 2,
// 	{
// 		{ "name", "ircd.ios.empt.call" },
// 	},
// };

uint64_t ircd::ios::empt::none = 0;
// /// Count of calls which reported zero ready events.
// decltype(ircd::ios::empt::none)
// ircd::ios::empt::none
// {
// 	stats + 3,
// 	{
// 		{ "name", "ircd.ios.empt.none" },
// 	},
// };

uint64_t ircd::ios::empt::result = 0;
// /// Total number of events reported from all calls.
// decltype(ircd::ios::empt::result)
// ircd::ios::empt::result
// {
// 	stats + 4,
// 	{
// 		{ "name", "ircd.ios.empt.result" },
// 	},
// };

uint64_t ircd::ios::empt::load_low = 0;
// /// Count of calls which reported more events than the low threshold.
// decltype(ircd::ios::empt::load_low)
// ircd::ios::empt::load_low
// {
// 	stats + 5,
// 	{
// 		{ "name", "ircd.ios.empt.load.low" },
// 	},
// };

uint64_t ircd::ios::empt::load_med = 0;
// /// Count of calls which reported more events than the medium threshold.
// decltype(ircd::ios::empt::load_med)
// ircd::ios::empt::load_med
// {
// 	stats + 6,
// 	{
// 		{ "name", "ircd.ios.empt.load.med" },
// 	},
// };

uint64_t ircd::ios::empt::load_high = 0;
// /// Count of calls which reported more events than the high threshold.
// decltype(ircd::ios::empt::load_high)
// ircd::ios::empt::load_high
// {
// 	stats + 7,
// 	{
// 		{ "name", "ircd.ios.empt.load.high" },
// 	},
// };

uint64_t ircd::ios::empt::load_stall = 0;
// /// Count of calls which reported the maximum number of events.
// decltype(ircd::ios::empt::load_stall)
// ircd::ios::empt::load_stall
// {
// 	stats + 8,
// 	{
// 		{ "name", "ircd.ios.empt.load.stall" },
// 	}
// };

//
// descriptor
//

template<>
decltype(ircd::util::instance_list<ircd::ios::descriptor>::allocator)
ircd::util::instance_list<ircd::ios::descriptor>::allocator
{};

template<>
decltype(ircd::util::instance_list<ircd::ios::descriptor>::list)
ircd::util::instance_list<ircd::ios::descriptor>::list
{
	allocator
};

decltype(ircd::ios::descriptor::ids)
ircd::ios::descriptor::ids;

//
// descriptor::descriptor
//

ircd::ios::descriptor::descriptor(const string_view &name,
                                  const decltype(allocator) &allocator,
                                  const decltype(deallocator) &deallocator,
                                  const bool &continuation)
noexcept
:name
{
	name
}
,stats
{
	std::make_unique<struct stats>(*this)
}
,allocator
{
	allocator?: default_allocator
}
,deallocator
{
	deallocator?: default_deallocator
}
,history
(
	256, {0}
)
,history_pos
{
	0
}
,continuation
{
	continuation
}
{
}

ircd::ios::descriptor::~descriptor()
noexcept
{
	// if(likely(stats) && unlikely(stats->queued))
	// 	log::critical
	// 	{
	// 		log, "descriptor(%p) '%s' leaking queued:%zd allocs:%zd bytes:%zd",
	// 		this,
	// 		name,
	// 		ssize_t(stats->queued),
	// 		ssize_t(stats->allocs) - ssize_t(stats->frees),
	// 		ssize_t(stats->alloc_bytes) - ssize_t(stats->free_bytes),
	// 	};

	assert(!stats || stats->queued == 0);
	assert(!stats || stats->allocs == stats->frees);
	assert(!stats || stats->alloc_bytes == stats->free_bytes);
}

//
// descriptor::stats
//

namespace ircd::ios
{
	static thread_local char stats_name_buf[128];
	static string_view stats_name(const descriptor &d, const string_view &key);
}

ircd::string_view
ircd::ios::stats_name(const descriptor &d,
                      const string_view &key)
{
	return string_view
	{
		stats_name_buf, unsigned(::snprintf
		(
			stats_name_buf, sizeof(stats_name_buf),
			"ircd.ios.%s.%s",
			d.name? d.name.c_str(): "",
			key? key.c_str(): ""
		))
	};
}

ircd::ios::descriptor::stats::stats(descriptor &d)
:value{0}
,items{0}
,queued
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "queued") },
	// },
}
,calls
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "calls") },
	// },
}
,faults
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "faults") },
	// },
}
,allocs
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "allocs") },
	// },
}
,alloc_bytes
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "alloc_bytes") },
	// },
}
,frees
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "frees") },
	// },
}
,free_bytes
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "free_bytes") },
	// },
}
,slice_last
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "slice_last") },
	// },
}
,slice_total
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "slice_total") },
	// },
}
,latency_last
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "latency_last") },
	// },
}
,latency_total
{
    0
	// value + items++,
	// {
	// 	{ "name", stats_name(d, "latency_total") },
	// },
}
{
	assert(items <= (sizeof(value) / sizeof(value[0])));
}

ircd::ios::descriptor::stats::~stats()
noexcept
{
}

//
// handler
//

decltype(ircd::ios::handler::current)
thread_local
ircd::ios::handler::current;

decltype(ircd::ios::handler::epoch)
thread_local
ircd::ios::handler::epoch;

[[gnu::cold]]
bool
ircd::ios::handler::fault(handler *const handler)
noexcept
{
	assert(handler && handler->descriptor);
	auto &descriptor(*handler->descriptor);

	assert(descriptor.stats);
	auto &stats(*descriptor.stats);
	++stats.faults;

	bool ret
	{
		false
	};

	// log::logf
	// {
	// 	log, log::level::DERROR,
	// 	"FAULT %5u %-30s [%11lu] faults[%9lu] q:%-4lu :%s",
	// 	descriptor.id,
	// 	trunc(descriptor.name, 30),
	// 	uint64_t(stats.calls),
	// 	uint64_t(stats.faults),
	// 	uint64_t(stats.queued),
	// 	what(std::current_exception()),
	// };

	// Our API sez if this function returns true, caller is responsible for
	// calling leave(), otherwise they must not call leave().
	if(likely(!ret))
		leave(handler);

	return ret;
}

[[gnu::hot]]
void
ircd::ios::handler::leave(handler *const handler)
noexcept
{
	assert(handler && handler->descriptor);
	auto &descriptor(*handler->descriptor);

	assert(descriptor.stats);
	auto &stats(*descriptor.stats);

	const auto slice_start
	{
		std::exchange(handler->ts, cycles())
	};

	// NOTE: will fail without constant_tsc;
	// NOTE: may fail without nonstop_tsc after OS suspend mode
	assert(handler->ts >= slice_start);
	stats.slice_last = handler->ts - slice_start;
	stats.slice_total += stats.slice_last;

	if constexpr(profile::history)
	{
		assert(descriptor.history_pos < descriptor.history.size());
		descriptor.history[descriptor.history_pos][0] = handler::epoch;
		descriptor.history[descriptor.history_pos][1] = stats.slice_last;
		++descriptor.history_pos;
	}

	// if constexpr(profile::logging)
	// 	log::logf
	// 	{
	// 		log, log::level::DEBUG,
	// 		"LEAVE %5u %-30s [%11lu] cycles[%15lu] q:%-4lu",
	// 		descriptor.id,
	// 		trunc(descriptor.name, 30),
	// 		uint64_t(stats.calls),
	// 		uint64_t(stats.slice_last),
	// 		uint64_t(stats.queued),
	// 	};

	assert(handler::current == handler);

	#if defined(RB_ASSERT_OPTIMISTIC)
	ircd::assertion.point();
	#endif

	handler::current = nullptr;
}

[[gnu::hot]]
void
ircd::ios::handler::enter(handler *const handler)
noexcept
{
	assert(handler && handler->descriptor);
	auto &descriptor(*handler->descriptor);

	assert(descriptor.stats);
	auto &stats(*descriptor.stats);

	const auto last_ts
	{
		std::exchange(handler->ts, cycles())
	};

	stats.latency_last = handler->ts - last_ts;
	stats.latency_total += stats.latency_last;
	++stats.calls;

	assert(!handler::current);
	handler::current = handler;
	++handler::epoch;

	// if constexpr(profile::logging)
	// 	log::logf
	// 	{
	// 		log, log::level::DEBUG,
	// 		"ENTER %5u %-30s [%11lu] latent[%15lu] q:%-4lu",
	// 		descriptor.id,
	// 		trunc(descriptor.name, 30),
	// 		uint64_t(stats.calls),
	// 		uint64_t(stats.latency_last),
	// 		uint64_t(stats.queued),
	// 	};
}

//
// dispatch
//

ircd::ios::dispatch::dispatch(descriptor &descriptor,
                              defer_t,
                              yield_t)
:dispatch
{
	descriptor, defer, yield, []() noexcept
	{
	}
}
{
}

ircd::ios::dispatch::dispatch(descriptor &descriptor,
                              defer_t,
                              yield_t,
                              const std::function<void ()> &function)
{
	const ctx::uninterruptible::nothrow ui;
	ctx::latch latch{1};
	dispatch
	{
		descriptor, defer, [&function, &latch]
		{
			const unwind uw
			{
				[&latch]
				{
					latch.count_down();
				}
			};

			function();
		}
	};

	latch.wait();
}

ircd::ios::dispatch::dispatch(descriptor &descriptor,
                              defer_t,
                              std::function<void ()> function)
{
	boost::asio::post(get(), handle(descriptor, std::move(function)));
}

ircd::ios::dispatch::dispatch(descriptor &descriptor,
                              yield_t,
                              const std::function<void ()> &function)
{
	assert(function);
	assert(ctx::current && handler::current);

	const ctx::uninterruptible ui;
	ctx::continuation
	{
		continuation::false_predicate, continuation::noop_interruptor, [&descriptor, &function]
		(auto &yield)
		{
			assert(!ctx::current && !handler::current);
			boost::asio::dispatch(get(), handle(descriptor, function));

			assert(!ctx::current && !handler::current);
		}
	};
}

ircd::ios::dispatch::dispatch(descriptor &descriptor,
                              std::function<void ()> function)
{
	const auto parent(handler::current); try
	{
		assert(!ctx::current && handler::current);
		ios::handler::leave(parent);

		assert(!ctx::current && !handler::current);
		boost::asio::dispatch(get(), handle(descriptor, std::move(function)));

		assert(!ctx::current && !handler::current);
		ios::handler::enter(parent);
	}
	catch(...)
	{
		assert(!ctx::current && !handler::current);
		ios::handler::enter(parent);

		assert(!ctx::current && handler::current == parent);
		throw;
	}

	assert(!ctx::current && handler::current == parent);
}
