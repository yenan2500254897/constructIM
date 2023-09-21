#pragma once
#define HAVE_IRCD_BUFFER_WINDOW_BUFFER_H

/// The window_buffer is just two mutable_buffers. One of the two buffers
/// just spans an underlying space and the other buffer is a window of the
/// remaining space which shrinks toward the end as the space is consumed.
/// The window_buffer object inherits from the latter, so it always has the
/// appearance of a mutable_buffer windowing on the the next place to write.
///
/// The recommended usage of this device is actually through the operator()
/// closure, which will automatically resize the window based on the return
/// value in the closure.
///
struct ircd::buffer::window_buffer
:mutable_buffer
{
	using closure = std::function<size_t (const mutable_buffer &)>;
	using closure_cbuf = std::function<const_buffer (const mutable_buffer &)>;

	mutable_buffer base;

	size_t remaining() const;
	size_t consumed() const;

	const_buffer remains() const;
	const_buffer completed() const;
	mutable_buffer remains();
	mutable_buffer completed();

	explicit operator const_buffer() const;

	const_buffer operator()(const closure &);
	const_buffer operator()(const closure_cbuf &);
	const_buffer rewind(const size_t &n = 1);
	const_buffer shift(const size_t &n);

	window_buffer(const mutable_buffer &base);
	window_buffer() = default;
};

inline
ircd::buffer::window_buffer::window_buffer(const mutable_buffer &base)
:mutable_buffer{base}
,base{base}
{}

inline ircd::buffer::const_buffer
ircd::buffer::window_buffer::shift(const size_t &n)
{
	const size_t nmax{std::min(n, consumed())};
	const const_buffer src
	{
		base.begin() + nmax, this->begin()
	};

	const mutable_buffer dst
	{
		base.begin(), consumed() - nmax
	};

	assert(size(src) == size(dst));
	assert(data(src) >= data(dst));
	assert(size(src) <= consumed());
	assert(size(dst) <= size(base));
	move(dst, src);
	rewind(nmax);
	assert(base.begin() <= begin());
	assert(begin() <= base.end());
	return completed();
}

inline ircd::buffer::const_buffer
ircd::buffer::window_buffer::rewind(const size_t &n)
{
	const size_t nmax{std::min(n, consumed())};
	static_cast<mutable_buffer &>(*this).begin() -= nmax;
	assert(base.begin() <= begin());
	assert(begin() <= base.end());
	return completed();
}

inline ircd::buffer::const_buffer
ircd::buffer::window_buffer::operator()(const closure_cbuf &closure)
{
	return operator()([&closure]
	(const mutable_buffer &buf)
	{
		return size(closure(buf));
	});
}

inline ircd::buffer::const_buffer
ircd::buffer::window_buffer::operator()(const closure &closure)
{
	const size_t addl(closure(*this));
	assert(addl <= remaining());
	consume(*this, addl);
	return completed();
}

/// View the completed portion of the stream
inline ircd::buffer::mutable_buffer
ircd::buffer::window_buffer::completed()
{
	assert(base.begin() <= begin());
	assert(base.begin() + consumed() <= base.end());
	return { base.begin(), base.begin() + consumed() };
}

/// View the remaining portion of the stream
inline ircd::buffer::mutable_buffer
ircd::buffer::window_buffer::remains()
{
	assert(base.begin() <= begin());
	assert(base.begin() + consumed() <= base.end());
	return { base.begin() + consumed(), base.end() };
}

/// Convenience conversion to get the completed portion
inline ircd::buffer::window_buffer::operator
const_buffer()
const
{
	return completed();
}

/// View the completed portion of the stream
inline ircd::buffer::const_buffer
ircd::buffer::window_buffer::completed()
const
{
	assert(base.begin() <= begin());
	assert(base.begin() + consumed() <= base.end());
	return { base.begin(), base.begin() + consumed() };
}

/// View the remaining portion of the stream
inline ircd::buffer::const_buffer
ircd::buffer::window_buffer::remains()
const
{
	assert(base.begin() <= begin());
	assert(base.begin() + consumed() <= base.end());
	return { base.begin() + consumed(), base.end() };
}

/// Bytes used by writes to the stream buffer
inline size_t
ircd::buffer::window_buffer::consumed()
const
{
	assert(begin() >= base.begin());
	assert(begin() <= base.end());
	return std::distance(base.begin(), begin());
}

/// Bytes remaining for writes to the stream buffer (same as size(*this))
inline size_t
ircd::buffer::window_buffer::remaining()
const
{
	assert(begin() <= base.end());
	const size_t ret(std::distance(begin(), base.end()));
	assert(ret == size(*this));
	return ret;
}
