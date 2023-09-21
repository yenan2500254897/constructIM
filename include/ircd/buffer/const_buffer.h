#pragma once
#define HAVE_IRCD_BUFFER_CONST_BUFFER_H

struct ircd::buffer::const_buffer
:buffer<const char *>
{
	// Definition for this is somewhere in the .cc files where boost is incl.
	operator boost::asio::const_buffer() const noexcept;

	template<class R,
	         bool safety = true>
	explicit operator const R *() const noexcept(!safety);

	// For boost::spirit conceptual compliance; illegal/noop
	void insert(const char *const &, const char &);

	using buffer<const char *>::buffer;
	template<class T, size_t SIZE> explicit const_buffer(const T (&)[SIZE]);
	template<size_t SIZE> const_buffer(const char (&buf)[SIZE]);
	template<size_t SIZE> const_buffer(const std::array<char, SIZE> &buf);
	const_buffer(const buffer<const char *> &b);
	const_buffer(const buffer<char *> &b, const size_t &);
	const_buffer(const buffer<char *> &b);
	const_buffer(const mutable_buffer &b);
	const_buffer(const string_view &s);
	const_buffer() = default;
};

inline
__attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const buffer<const char *> &b)
:buffer<const char *>{b}
{}

inline
__attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const buffer<char *> &b)
:buffer<const char *>{data(b), size(b)}
{}

inline
__attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const buffer<char *> &b,
                                         const size_t &max)
:buffer<const char *>{{data(b), size(b)}, max}
{}

inline
__attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const mutable_buffer &b)
:buffer<const char *>{data(b), size(b)}
{}

inline
__attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const string_view &s)
:buffer<const char *>{data(s), size(s)}
{}

template<size_t SIZE>
inline __attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const char (&buf)[SIZE])
:buffer<const char *>{buf, SIZE}
{}

template<size_t SIZE>
inline __attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const std::array<char, SIZE> &buf)
:buffer<const char *>{reinterpret_cast<const char *>(buf.data()), SIZE}
{}

template<class T,
         size_t SIZE>
inline __attribute__((always_inline))
ircd::buffer::const_buffer::const_buffer(const T (&buf)[SIZE])
:buffer<const char *>{reinterpret_cast<const char *>(buf), SIZE * sizeof(T)}
{}

#ifndef _NDEBUG
__attribute__((noreturn))
#endif
inline void
ircd::buffer::const_buffer::insert(const char *const &,
                                   const char &)
{
	assert(0);
	__builtin_unreachable();
}

template<class R,
         bool safety>
inline ircd::buffer::const_buffer::operator
const R *()
const noexcept(!safety)
{
	assert(sizeof(R) <= size(*this));
	if constexpr(safety)
		if(unlikely(sizeof(R) > size(*this)))
			throw std::bad_cast{};

	return reinterpret_cast<const R *>(data(*this));
}