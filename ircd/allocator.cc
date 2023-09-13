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

//
// allocator::state
//

void ircd::allocator::state::deallocate(const uint &pos,
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

//寻找一块连续的内存区域，恰好能存下n个数据
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