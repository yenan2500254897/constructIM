#pragma once
#define HAVE_IRCD_BUFFER_H

// Forward declarations from boost::asio because it is not included here. IRCd
// buffers are not based directly on the boost ones but are easily converted
// when passing our buffer to an asio function.
namespace boost::asio
{
	struct const_buffer;
	struct mutable_buffer;
}

/// Lightweight buffer interface compatible with boost::asio IO buffers and vectors
///
/// A const_buffer is a pair of iterators like `const char *` meant for sending
/// data; a mutable_buffer is a pair of iterators meant for receiving.
///
/// These templates offer tools for individual buffers as well as tools for
/// iterations of buffers.  An iteration of buffers is an iovector that is
/// passed to our sockets etc. The ircd::iov template can host an iteration of
/// buffers. The `template template` functions are tools for a container of
/// buffers of any permutation.
///
namespace ircd::buffer
{
	template<class it> struct buffer;

	struct const_buffer;
	struct mutable_buffer;
	struct window_buffer;
	struct parse_buffer;
	template<class buffer, size_t SIZE> struct fixed_buffer;
	template<class buffer> struct unique_buffer;
	template<class buffer> struct shared_buffer;

	template<size_t SIZE> using fixed_const_buffer = fixed_buffer<const_buffer, SIZE>;
	template<size_t SIZE> using fixed_mutable_buffer = fixed_buffer<mutable_buffer, SIZE>;
	using unique_const_buffer = unique_buffer<const_buffer>;
	using unique_mutable_buffer = unique_buffer<mutable_buffer>;
	using shared_const_buffer = shared_buffer<const_buffer>;
	using shared_mutable_buffer = shared_buffer<mutable_buffer>;

	// Convenience null buffer
	[[clang::internal_linkage]] extern const mutable_buffer null_buffer;

	// Single buffer iteration of contents
	template<class it> const it &begin(const buffer<it> &buffer);
	template<class it> const it &end(const buffer<it> &buffer);
	template<class it> it &begin(buffer<it> &buffer);
	template<class it> it &end(buffer<it> &buffer);
	template<class it> std::reverse_iterator<it> rbegin(const buffer<it> &buffer);
	template<class it> std::reverse_iterator<it> rend(const buffer<it> &buffer);

    // Single buffer observer utils
	template<class it> bool null(const buffer<it> &buffer);
	template<class it> bool empty(const buffer<it> &buffer);
	template<class it> bool operator!(const buffer<it> &buffer);
	template<class it> size_t size(const buffer<it> &buffer);
	template<class it> const it &data(const buffer<it> &buffer);
	template<class it> bool padded(const buffer<it> &buffer, const size_t &alignment);
	template<class it> bool aligned(const buffer<it> &buffer, const size_t &alignment);
	template<class it> buffer<it> operator+(const buffer<it> &buffer, const size_t &bytes);
	size_t overlap_count(const const_buffer &, const const_buffer &);
	bool overlap(const const_buffer &, const const_buffer &);

	// Single buffer mutator utils
	template<class it> size_t consume(buffer<it> &buffer, const size_t &bytes);
	template<class it> buffer<it> &operator+=(buffer<it> &buffer, const size_t &bytes);

	// other tools
	size_t reverse(const mutable_buffer &dst, const const_buffer &src);
	void reverse(const mutable_buffer &buf);
	size_t zero(const mutable_buffer &buf);

	// Convenience copy to std stream
	template<class it> std::ostream &operator<<(std::ostream &s, const buffer<it> &buffer);
}

#include "buffer_base.h"
#include "mutable_buffer.h"
#include "const_buffer.h"
#include "copy.h"
#include "move.h"
#include "fixed_buffer.h"
#include "unique_buffer.h"
#include "shared_buffer.h"
#include "window_buffer.h"
#include "parse_buffer.h"
#include "buffers.h"

// Export these important aliases down to main ircd namespace
namespace ircd
{
	namespace buffers = buffer::buffers;

	using buffer::const_buffer;
	using buffer::mutable_buffer;
	using buffer::fixed_buffer;
	using buffer::unique_buffer;
	using buffer::shared_buffer;
	using buffer::null_buffer;
	using buffer::window_buffer;
	using buffer::fixed_const_buffer;
	using buffer::fixed_mutable_buffer;
	using buffer::unique_const_buffer;
	using buffer::unique_mutable_buffer;
	using buffer::shared_const_buffer;
	using buffer::shared_mutable_buffer;
	using buffer::null_buffer;

	using buffers::const_buffers;
	using buffers::mutable_buffers;
	using buffers::null_buffers;

	using buffer::size;
	using buffer::data;
	using buffer::copy;
	using buffer::move;
	using buffer::consume;
	using buffer::begin;
	using buffer::end;

	using buffer::aligned;
	using buffer::padded;
	
}

template<class it>
std::ostream &
ircd::buffer::operator<<(std::ostream &s, const buffer<it> &buffer)
{
	assert(!null(buffer) || get<1>(buffer) == nullptr);
	s.write(data(buffer), size(buffer));
	return s;
}

// We use the sodium_memzero() from libsodium in ircd/sodium.cc if available
// to ensure cross-platform guarantees the zero'ing doesn't get optimized away.
#ifndef HAVE_SODIUM
inline size_t
__attribute__((always_inline))
ircd::buffer::zero(const mutable_buffer &buf)
{
	std::memset(data(buf), 0x0, size(buf));
	return size(buf);
}
#endif

inline void
__attribute__((always_inline))
ircd::buffer::reverse(const mutable_buffer &buf)
{
	std::reverse(data(buf), data(buf) + size(buf));
}

inline size_t
__attribute__((always_inline))
ircd::buffer::reverse(const mutable_buffer &dst,
                      const const_buffer &src)
{
	const size_t ret
	{
		std::min(size(dst), size(src))
	};

	std::reverse_copy(data(src), data(src) + ret, data(dst));
	return ret;
}

template<class it>
inline ircd::buffer::buffer<it> &
__attribute__((always_inline))
ircd::buffer::operator+=(buffer<it> &buffer,
                         const size_t &bytes)
{
	consume(buffer, bytes);
	return buffer;
}

template<class it>
inline size_t
__attribute__((always_inline))
ircd::buffer::consume(buffer<it> &b,
                      const size_t &bytes)
{
	assert(!null(b));
	assert(bytes <= size(b));
	const size_t advance
	{
		std::min(bytes, size(b))
	};

	std::get<0>(b) += advance;
	assert(std::get<0>(b) <= std::get<1>(b));
	return advance;
}

inline bool
__attribute__((always_inline))
ircd::buffer::overlap(const const_buffer &a,
                      const const_buffer &b)
{
	return overlap_count(a, b) > 0UL;
}

inline size_t
__attribute__((always_inline))
ircd::buffer::overlap_count(const const_buffer &a,
                            const const_buffer &b)
{
	const char *const res[2]
	{
		std::max(begin(a), begin(b)),
		std::min(end(a), end(b)),
	};

	return std::max(res[1] - res[0], 0L);
}

template<class it>
inline ircd::buffer::buffer<it>
__attribute__((always_inline))
ircd::buffer::operator+(const buffer<it> &buffer,
                        const size_t &bytes)
{
	auto ret(buffer);
	ret += bytes;
	return ret;
}

template<class it>
inline bool
__attribute__((always_inline))
ircd::buffer::aligned(const buffer<it> &buffer,
                      const size_t &a)
{
	return likely(a)?
		ircd::aligned(data(buffer), a) && ircd::padded(buffer, a):
		true;
}

template<class it>
inline bool
__attribute__((always_inline))
ircd::buffer::padded(const buffer<it> &buffer,
                     const size_t &a)
{
	return likely(a)?
		size(buffer) % a == 0:
		true;
}

template<class it>
inline const it &
__attribute__((always_inline))
ircd::buffer::data(const buffer<it> &buffer)
{
	return get<0>(buffer);
}

template<class it>
inline size_t
__attribute__((always_inline))
ircd::buffer::size(const buffer<it> &buffer)
{
	assert(get<0>(buffer) <= get<1>(buffer));
	assert(!null(buffer) || get<1>(buffer) == nullptr);
	return std::distance(get<0>(buffer), get<1>(buffer));
}

template<class it>
inline bool
__attribute__((always_inline))
ircd::buffer::operator!(const buffer<it> &buffer)
{
	return empty(buffer);
}

template<class it>
inline bool
__attribute__((always_inline))
ircd::buffer::empty(const buffer<it> &buffer)
{
	return null(buffer) || std::distance(get<0>(buffer), get<1>(buffer)) == 0;
}

template<class it>
inline bool
__attribute__((always_inline))
ircd::buffer::null(const buffer<it> &buffer)
{
	return get<0>(buffer) == nullptr;
}

template<class it>
inline std::reverse_iterator<it>
__attribute__((always_inline))
ircd::buffer::rend(const buffer<it> &buffer)
{
	return std::reverse_iterator<it>(get<0>(buffer));
}

template<class it>
inline std::reverse_iterator<it>
__attribute__((always_inline))
ircd::buffer::rbegin(const buffer<it> &buffer)
{
	return std::reverse_iterator<it>(get<0>(buffer) + size(buffer));
}

template<class it>
inline it &
__attribute__((always_inline))
ircd::buffer::end(buffer<it> &buffer)
{
	return get<1>(buffer);
}

template<class it>
inline it &
__attribute__((always_inline))
ircd::buffer::begin(buffer<it> &buffer)
{
	return get<0>(buffer);
}

template<class it>
inline const it &
__attribute__((always_inline))
ircd::buffer::end(const buffer<it> &buffer)
{
	return get<1>(buffer);
}

template<class it>
inline const it &
__attribute__((always_inline))
ircd::buffer::begin(const buffer<it> &buffer)
{
	return get<0>(buffer);
}

[[gnu::visibility("internal")]]
inline decltype(ircd::buffer::null_buffer)
ircd::buffer::null_buffer
{
	nullptr, nullptr
};

[[gnu::visibility("internal")]]
inline decltype(ircd::buffers::null_buffers)
ircd::buffers::null_buffers
{
	{ null_buffer }
};