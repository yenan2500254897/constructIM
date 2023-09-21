#pragma once
#define HAVE_IRCD_BUFFER_COPY_H

namespace ircd::buffer
{
	// copy single character
	char *&copy(char *&dest, char *const &stop, char src);
	size_t copy(const mutable_buffer &dst, const char src);

	// copy non-overlapping regions
	char *&copy(char *&dest, char *const &stop, const const_buffer &src);
	size_t copy(const mutable_buffer &dst, const const_buffer &src);
	template<size_t SIZE> size_t copy(const mutable_buffer &dst, const char (&)[SIZE]);
}

template<size_t SIZE>
#ifndef __clang__
__attribute__((error
#else
__attribute__((unavailable
#endif
(
	"Copy source is an array. Is this a string literal? Do you want to copy the \\0?"
	" Disambiguate this by typing the source string_view or const_buffer."
)))
inline size_t
ircd::buffer::copy(const mutable_buffer &dst,
                   const char (&buf)[SIZE])
{
	return copy(dst, const_buffer{buf});
}

inline size_t
ircd::buffer::copy(const mutable_buffer &dst,
                   const const_buffer &src)
{
	char *const &s(begin(dst)), *e(s);
	e = copy(e, end(dst), src);
	assert(std::distance(s, e) >= 0);
	return std::distance(s, e);
}

inline size_t
ircd::buffer::copy(const mutable_buffer &dst,
                   const char src)
{
	char *const &s(begin(dst)), *e(s);
	e = copy(e, end(dst), src);
	assert(std::distance(s, e) >= 0);
	return std::distance(s, e);
}

inline char *&
__attribute__((always_inline))
ircd::buffer::copy(char *&__restrict__ dest,
                   char *const &__restrict__ stop,
                   const const_buffer &src)
{
	assert(dest <= stop);
	const auto *const __restrict__ srcp
	{
		data(src)
	};

	const size_t remain
	(
		std::distance(dest, stop)
	);

	const size_t cpsz
	{
		std::min(size(src), remain)
	};

	assert(!overlap(const_buffer(dest, cpsz), src));
	assert(cpsz <= size(src));
	assert(cpsz <= remain);

	// Suppress warnings which present from the __builtin_memcpy() but these
	// are actually caused by the overlap assertion above which should have no
	// side-effects; this is very likely a bug in gcc.
	#pragma GCC diagnostic push
	#ifdef RB_ASSERT
	#pragma GCC diagnostic ignored "-Warray-bounds"
	#pragma GCC diagnostic ignored "-Wstringop-overflow"
	#endif
	__builtin_memcpy(dest, srcp, cpsz);
	#pragma GCC diagnostic pop

	dest += cpsz;
	assert(dest <= stop);
	return dest;
}

inline char *&
ircd::buffer::copy(char *&__restrict__ dest,
                   char *const &__restrict__ stop,
                   char src)
{
	assert(dest <= stop);
	const bool cpsz(dest != stop);
	(cpsz? *dest : src) = src;
	dest += cpsz;
	assert(dest <= stop);
	return dest;
}
