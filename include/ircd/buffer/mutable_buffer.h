
#pragma once
#define HAVE_IRCD_BUFFER_MUTABLE_BUFFER_H

/// Base for mutable buffers, or buffers which can be written to because they
/// are not const.
///
struct ircd::buffer::mutable_buffer
: buffer<char *>
{
	// The definition for this is somewhere in one of the .cc files.
	/// Conversion offered for the analogous asio buffer.
	operator boost::asio::mutable_buffer() const noexcept;

	template<class R,
	         bool safety = true>
	explicit operator R *() const noexcept(!safety);

	/// Allows boost::spirit to append to the buffer; this means the size() of
	/// this buffer becomes a consumption counter and the real size of the buffer
	/// must be kept separately. This is the lowlevel basis for a stream buffer.
	void insert(char *const &it, const value_type &v);

	using buffer<char *>::buffer;
	template<class T, size_t SIZE> explicit mutable_buffer(T (&)[SIZE]);
	template<size_t SIZE> mutable_buffer(char (&buf)[SIZE]);
	template<size_t SIZE> mutable_buffer(std::array<char, SIZE> &buf);
	mutable_buffer(const std::function<void (const mutable_buffer &)> &);
	explicit mutable_buffer(std::string &buf);
	mutable_buffer(const buffer<char *> &b);
	mutable_buffer() = default;
};

inline
__attribute__((always_inline))
ircd::buffer::mutable_buffer::mutable_buffer(const buffer<char *> &b)
:buffer<char *>{b}
{}

/// lvalue string reference offered to write through to a std::string as
/// the buffer. should be hard to bind by accident...
inline
__attribute__((always_inline))
ircd::buffer::mutable_buffer::mutable_buffer(std::string &buf)
:mutable_buffer{const_cast<char *>(buf.data()), buf.size()}
{}

inline
__attribute__((always_inline))
ircd::buffer::mutable_buffer::mutable_buffer(const std::function<void (const mutable_buffer &)> &closure)
{
	closure(*this);
}

template<size_t SIZE>
inline __attribute__((always_inline))
ircd::buffer::mutable_buffer::mutable_buffer(char (&buf)[SIZE])
:buffer<char *>{buf, SIZE}
{}

template<size_t SIZE>
inline __attribute__((always_inline))
ircd::buffer::mutable_buffer::mutable_buffer(std::array<char, SIZE> &buf)
:buffer<char *>{buf.data(), SIZE}
{}

template<class T,
         size_t SIZE>
inline __attribute__((always_inline))
ircd::buffer::mutable_buffer::mutable_buffer(T (&buf)[SIZE])
:buffer<char *>{reinterpret_cast<char *>(const_cast<char *> (buf)), SIZE * sizeof(T)}
{}

inline void
ircd::buffer::mutable_buffer::insert(char *const &it,
                                     const value_type &v)
{
	assert(it >= this->begin() && it <= this->end());
	memmove(it + 1, it, std::distance(it, this->end()));
	*it = v;
	++std::get<1>(*this);
}

template<class R,
         bool safety>
inline ircd::buffer::mutable_buffer::operator
R *()
const noexcept(!safety)
{
    assert(sizeof(R) <= size(*this));
    if constexpr(safety)
        if(unlikely(sizeof(R) > size(*this)))
            throw std::bad_cast{};

    return reinterpret_cast<const R *>(data(*this));
}