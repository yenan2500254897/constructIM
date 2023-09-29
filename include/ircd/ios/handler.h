#pragma once
#define HAVE_IRCD_IOS_HANDLER_H

namespace ircd::ios
{
	struct handler;

	template<class function>
	struct handle;

	const string_view &name(const handler &);
}

/// Non-template base class for ios::handle; templated derivations of this
/// class comprise the function object we're submitting to boost::asio.
struct ircd::ios::handler
{
	static thread_local handler *current;
	static thread_local uint64_t epoch;

	static void enqueue(handler *) noexcept;
	static void *allocate(handler *, const size_t);
	static void deallocate(handler *, void *, const size_t) noexcept;
	static bool continuation(handler *) noexcept;
	static void enter(handler *) noexcept;
	static void leave(handler *) noexcept;
	static bool fault(handler *) noexcept;

	ios::descriptor *descriptor {nullptr};
	uint64_t ts {0}; // last tsc sample; for profiling each phase
};

/// Our function object type template. These should be rvalue-constructed in a
/// callback argument to a boost::asio call. A reference to an ios::descriptor
/// must be provided.
template<class function>
struct ircd::ios::handle
:handler
{
	function f;

	template<class... args>
	void operator()(args&&... a) const;

	handle(ios::descriptor &, function) noexcept;
};

//
// ircd::ios::handle
//

/// Construction of the handle is considered the enquement. Construct in the
/// callback argument to a boost::asio call; provide a reference to the
/// appropriate callsite descriptor.
template<class function>
inline
ircd::ios::handle<function>::handle(ios::descriptor &d,
                                    function f)
noexcept
:handler{&d, prof::cycles()}
,f(std::move(f))
{
	handler::enqueue(this);
}

template<class function>
template<class... args>
inline void
ircd::ios::handle<function>::operator()(args&&... a)
const
{
	assert(descriptor && descriptor->stats);
	assert(descriptor->stats->queued > 0);
	descriptor->stats->queued--;
	f(std::forward<args>(a)...);
}

//
// ircd::ios::handler
//

[[gnu::hot]]
inline void
ircd::ios::handler::deallocate(handler *const handler,
                               void *const ptr,
                               const size_t size)
noexcept
{
	assert(handler && handler->descriptor);
	auto &descriptor(*handler->descriptor);

	assert(descriptor.deallocator);
	descriptor.deallocator(*handler, ptr, size);

	assert(descriptor.stats);
	auto &stats(*descriptor.stats);
	stats.free_bytes += size;
	++stats.frees;
}

[[gnu::hot]]
inline void *
ircd::ios::handler::allocate(handler *const handler,
                             const size_t size)
{
	assert(handler && handler->descriptor);
	auto &descriptor(*handler->descriptor);

	assert(descriptor.stats);
	auto &stats(*descriptor.stats);
	stats.alloc_bytes += size;
	++stats.allocs;

	assert(descriptor.allocator);
	return descriptor.allocator(*handler, size);
}

[[gnu::hot]]
inline void
ircd::ios::handler::enqueue(handler *const handler)
noexcept
{
	assert(handler && handler->descriptor);
	auto &descriptor(*handler->descriptor);

	assert(descriptor.stats);
	auto &stats(*descriptor.stats);
	++stats.queued;

	// if constexpr(profile::logging)
	// 	log::logf
	// 	{
	// 		log, log::level::DEBUG,
	// 		"QUEUE %5u %-30s [%11lu] ------[%15lu] q:%-4lu",
	// 		descriptor.id,
	// 		trunc(descriptor.name, 30),
	// 		uint64_t(stats.calls),
	// 		0UL,
	// 		uint64_t(stats.queued),
	// 	};
}

[[gnu::hot]]
inline bool
ircd::ios::handler::continuation(handler *const handler)
noexcept
{
	assert(handler && handler->descriptor);
	auto &descriptor(*handler->descriptor);
	return descriptor.continuation;
}

inline const ircd::string_view &
ircd::ios::name(const handler &handler)
{
	assert(handler.descriptor);
	return name(*handler.descriptor);
}
