#include <RB_INC_SYS_RESOURCE_H
#include <RB_INC_SYS_MMAN_H

// Uncomment or -D this #define to enable our own crude but simple ability to
// profile dynamic memory usage. Global `new` and `delete` will be captured
// here by this definition file into thread_local counters accessible via
// ircd::allocator::profile. This feature allows the developer to find out if
// allocations are occurring during some scope by sampling the counters.
//
// #define RB_PROF_ALLOC

namespace ircd::allocator
{
	static void advise_hugepage(void *const &, const size_t &alignment, const size_t &size);
}

#if defined(MADV_NORMAL) && defined(POSIX_MADV_NORMAL)
	static_assert(MADV_NORMAL == POSIX_MADV_NORMAL);
#endif

#if defined(MADV_SEQUENTIAL) && defined(POSIX_MADV_SEQUENTIAL)
	static_assert(MADV_SEQUENTIAL == POSIX_MADV_SEQUENTIAL);
#endif

#if defined(MADV_RANDOM) && defined(POSIX_MADV_RANDOM)
	static_assert(MADV_RANDOM == POSIX_MADV_RANDOM);
#endif

#if defined(MADV_WILLNEED) && defined(POSIX_MADV_WILLNEED)
	static_assert(MADV_WILLNEED == POSIX_MADV_WILLNEED);
#endif

#if defined(MADV_DONTNEED) && defined(POSIX_MADV_DONTNEED)
	static_assert(MADV_DONTNEED == POSIX_MADV_DONTNEED);
#endif

[[gnu::hot]]
char *
ircd::allocator::allocate(const size_t alignment,
                          const size_t size)
{
	assume(alignment > 0);
	assume(size % alignment == 0);
	assume(alignment % sizeof(void *) == 0);

	void *ret;
	switch(int errc(::posix_memalign(&ret, alignment, size)); errc)
	{
		[[likely]]
		case 0:
			break;

		[[unlikely]]
		case int(std::errc::not_enough_memory):
			throw std::bad_alloc{};

		[[unlikely]]
		default:
			// throw_system_error();
			__builtin_unreachable();
	}

	assert(ret != nullptr);
	assert(uintptr_t(ret) % alignment == 0);

	// if(likely(info::thp_size))
	if(true)
		advise_hugepage(ret, alignment, size);

	#ifdef RB_PROF_ALLOC
	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.alloc_bytes += size;
	this_thread.alloc_count++;
	#endif

	return reinterpret_cast<char *>(ret);
}

void
ircd::allocator::advise_hugepage(void *const &ptr,
                                 const size_t &alignment,
                                 const size_t &size)

#if defined(MADV_HUGEPAGE)
try
{
	// if(likely(alignment < info::thp_size))
	// 	return;

	// if(likely(alignment % size_t(info::thp_size) != 0))
	// 	return;

	// if(!has(info::thp_enable, "[madvise]"))
	// 	return;

	// sys::call(::madvise, ptr, size, MADV_HUGEPAGE);
	madvise(ptr, size, MADV_HUGEPAGE);
}
catch(const std::exception &e)
{
	// log::critical
	// {
	// 	"Failed to madvise(%p, %zu, MADV_HUGEPAGE) :%s",
	// 	ptr,
	// 	size,
	// 	e.what(),
	// };
}
#else
{
}
#endif



//
// control panel
//

bool
__attribute__((weak))
ircd::allocator::trim(const size_t &pad)
noexcept
{
	return false;
}

ircd::string_view
__attribute__((weak))
ircd::allocator::get(const string_view &key,
                     const mutable_buffer &buf)
{
	return {};
}

ircd::string_view
__attribute__((weak))
ircd::allocator::set(const string_view &key,
                     const string_view &val,
                     const mutable_buffer &cur)
{
	return {};
}

//
// allocator::state
//

void
ircd::allocator::state::deallocate(const uint &pos,
                                   const size_type &n)
{
	for(size_t i(0); i < n; ++i)
	{
		assert(test(pos + i));
		btc(pos + i);
	}

	last = pos;
}

uint
ircd::allocator::state::allocate(const size_type &n,
                                 const uint &hint)
{
	const auto ret
	{
		allocate(std::nothrow, n, hint)
	};

	if(unlikely(ret >= size))
		throw std::bad_alloc();

	return ret;
}

uint
ircd::allocator::state::allocate(std::nothrow_t,
                                 const size_type &n,
                                 const uint &hint)
{
	const auto next(this->next(n));
	if(unlikely(next >= size))         // No block of n was found anywhere (next is past-the-end)
		return next;

	for(size_t i(0); i < n; ++i)
	{
		assert(!test(next + i));
		bts(next + i);
	}

	last = next + n;
	return next;
}

uint
ircd::allocator::state::next(const size_t &n)
const
{
	uint ret(last), rem(n);
	for(; ret < size && rem; ++ret)
		if(test(ret))
			rem = n;
		else
			--rem;

	if(likely(!rem))
		return ret - n;

	for(ret = 0, rem = n; ret < last && rem; ++ret)
		if(test(ret))
			rem = n;
		else
			--rem;

	if(unlikely(rem))                  // The allocator should throw std::bad_alloc if !rem
		return size;

	return ret - n;
}

bool
ircd::allocator::state::available(const size_t &n)
const
{
	return this->next(n) < size;
}

//
// allocator::scope
//

decltype(ircd::allocator::scope::current)
ircd::allocator::scope::current;

ircd::allocator::scope::scope(alloc_closure ac,
                              realloc_closure rc,
                              free_closure fc)
:theirs
{
	current
}
,user_alloc
{
	std::move(ac)
}
,user_realloc
{
	std::move(rc)
}
,user_free
{
	std::move(fc)
}
{
	// If an allocator::scope instance already exists somewhere
	// up the stack, *current will already be set. We only install
	// our global hook handlers at the first instance ctor and
	// uninstall it after that first instance dtors.
	if(!current)
		hook_init();

	current = this;
}

ircd::allocator::scope::~scope()
noexcept
{
	assert(current);
	current = theirs;

	// Reinstall the pre-existing hooks after our last scope instance
	// has destructed (the first to have constructed). We know this when
	// current becomes null.
	if(!current)
		hook_fini();
}

void
__attribute__((weak))
ircd::allocator::scope::hook_init()
noexcept
{
}

void
__attribute__((weak))
ircd::allocator::scope::hook_fini()
noexcept
{
}

//
// allocator::profile
//

thread_local ircd::allocator::profile
ircd::allocator::profile::this_thread
{};

ircd::allocator::profile
ircd::allocator::operator-(const profile &a,
                           const profile &b)
{
	profile ret(a);
	ret -= b;
	return ret;
}

ircd::allocator::profile
ircd::allocator::operator+(const profile &a,
                           const profile &b)
{
	profile ret(a);
	ret += b;
	return ret;
}

ircd::allocator::profile &
ircd::allocator::operator-=(profile &a,
                            const profile &b)
{
	a.alloc_count -= b.alloc_count;
	a.free_count -= b.free_count;
	a.alloc_bytes -= b.alloc_bytes;
	a.free_bytes -= b.free_bytes;
	return a;
}

ircd::allocator::profile &
ircd::allocator::operator+=(profile &a,
                            const profile &b)
{
	a.alloc_count += b.alloc_count;
	a.free_count += b.free_count;
	a.alloc_bytes += b.alloc_bytes;
	a.free_bytes += b.free_bytes;
	return a;
}

//
// Developer profiling
//

#ifdef RB_PROF_ALLOC // --------------------------------------------------

void *
__attribute__((alloc_size(1), malloc, returns_nonnull))
operator new(const size_t size)
{
	void *const &ptr(::malloc(size));
	if(unlikely(!ptr))
		throw std::bad_alloc();

	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.alloc_bytes += size;
	this_thread.alloc_count++;

	return ptr;
}

void
operator delete(void *const ptr)
noexcept
{
	::free(ptr);

	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.free_count++;
}

void
operator delete(void *const ptr,
                const size_t size)
noexcept
{
	::free(ptr);

	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.free_bytes += size;
	this_thread.free_count++;
}

#endif // RB_PROF_ALLOC --------------------------------------------------

//
// Linker symbol wrapping hook
//

extern "C" [[gnu::weak]] void *__real_malloc(size_t size);
extern "C" [[gnu::weak]] void *__real_calloc(size_t nmemb, size_t size);
extern "C" [[gnu::weak]] void *__real_realloc(void *ptr, size_t size);
extern "C" [[gnu::weak]] void __real_free(void *ptr);

extern "C" void *
__wrap_malloc(size_t size)
{
	void *const &ptr(::__real_malloc(size));
	if(unlikely(!ptr))
		throw std::bad_alloc();

	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.alloc_bytes += size;
	this_thread.alloc_count++;
	return ptr;
}

extern "C" void *
__wrap_calloc(size_t nmemb, size_t size)
{
	void *const &ptr(::__real_calloc(nmemb, size));
	if(unlikely(!ptr))
		throw std::bad_alloc();

	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.alloc_bytes += nmemb * size;
	this_thread.alloc_count++;
	return __real_calloc(nmemb, size);
}

extern "C" void *
__wrap_realloc(void *ptr, size_t size)
{
	void *const &ret(::__real_realloc(ptr, size));
	if(unlikely(!ret))
		throw std::bad_alloc();

	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.alloc_bytes += size;
	this_thread.alloc_count++;
	return ret;
}

extern "C" void
__wrap_free(void *ptr)
{
	__real_free(ptr);

	auto &this_thread(ircd::allocator::profile::this_thread);
	this_thread.free_bytes += 0UL; //TODO: XXX
	this_thread.free_count++;
}
