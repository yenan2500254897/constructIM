#pragma once
#define HAVE_IRCD_IOS_DESCRIPTOR_H

namespace ircd::ios
{
	struct handler;
	struct descriptor;

	const string_view &name(const descriptor &);
}

/// Each descriptor classifies and quantifies our operations through asio.
/// Instances are usually static; all callback handlers are wrapped with an
/// ios::handle and associated with an ios::descriptor instance.
struct ircd::ios::descriptor
:instance_list<descriptor>
{
	struct stats;

	static uint64_t ids;

	static void *default_allocator(handler &, const size_t);
	static void default_deallocator(handler &, void *, const size_t) noexcept;

	string_view name;
	uint64_t id {++ids};
	std::unique_ptr<struct stats> stats;
	void *(*allocator)(handler &, const size_t);
	void (*deallocator)(handler &, void *, const size_t);
	std::vector<std::array<uint64_t, 2>> history; // epoch, cycles
	uint8_t history_pos {0};
	bool continuation {false};

	descriptor(const string_view &name,
	           const decltype(allocator) & = default_allocator,
	           const decltype(deallocator) & = default_deallocator,
	           const bool &continuation = false) noexcept;

	descriptor(descriptor &&) = delete;
	descriptor(const descriptor &) = delete;
	~descriptor() noexcept;
};

template<>
decltype(ircd::ios::descriptor::list)
ircd::instance_list<ircd::ios::descriptor>::list;

/// Statistics for the descriptor.
struct ircd::ios::descriptor::stats
{
	// using value_type = uint64_t;
	// using item = ircd::stats::item<value_type *>;

	// value_type value[11];
    uint64_t value[11];
	size_t items;

  public:
	// item queued;
	// item calls;
	// item faults;
	// item allocs;
	// item alloc_bytes;
	// item frees;
	// item free_bytes;
	// item slice_last;
	// item slice_total;
	// item latency_last;
	// item latency_total;
    uint64_t queued;
	uint64_t calls;
	uint64_t faults;
	uint64_t allocs;
	uint64_t alloc_bytes;
	uint64_t frees;
	uint64_t free_bytes;
	uint64_t slice_last;
	uint64_t slice_total;
	uint64_t latency_last;
	uint64_t latency_total;

	stats(descriptor &);
	stats() = delete;
	stats(const stats &) = delete;
	stats &operator=(const stats &) = delete;
	~stats() noexcept;
};

[[gnu::hot]]
inline void
ircd::ios::descriptor::default_deallocator(handler &handler,
                                           void *const ptr,
                                           const size_t size)
noexcept
{
	#ifdef __clang__
		::operator delete(ptr);
	#else
		::operator delete(ptr, size);
	#endif
}

[[gnu::hot]]
inline void *
ircd::ios::descriptor::default_allocator(handler &handler,
                                         const size_t size)
{
	return ::operator new(size);
}

inline const ircd::string_view &
ircd::ios::name(const descriptor &descriptor)
{
	return descriptor.name;
}
