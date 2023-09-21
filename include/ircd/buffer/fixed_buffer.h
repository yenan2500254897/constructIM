#pragma once
#define HAVE_IRCD_BUFFER_FIXED_BUFFER_H

/// fixed_buffer wraps an std::array with construction and conversions apropos
/// the ircd::buffer suite. fixed_buffer should be punnable. Its only memory
/// footprint is the array itself and
///
template<class buffer_type,
         size_t SIZE>
struct ircd::buffer::fixed_buffer
:std::array<typename std::remove_const<typename buffer_type::value_type>::type, SIZE>
{
	using mutable_type = typename std::remove_const<typename buffer_type::value_type>::type;
	using const_type = typename std::add_const<mutable_type>::type;
	using array_type = std::array<mutable_type, SIZE>;
	using proto_type = void (const mutable_buffer &);
	using args_type = const mutable_buffer &;

	operator buffer_type() const noexcept;
	operator buffer_type() noexcept;

	using array_type::array_type;

	template<class F>
	fixed_buffer(F&&,
	             typename std::enable_if<std::is_invocable<F, args_type>::value, void>::type * = nullptr);

	explicit fixed_buffer(nullptr_t) noexcept;
	fixed_buffer(const buffer_type &b) noexcept;
	fixed_buffer() = default;
};

static_assert
(
	// Assertion over an arbitrary but common template configuration.
	std::is_standard_layout<ircd::buffer::fixed_buffer<ircd::buffer::const_buffer, 32>>::value,
	"ircd::buffer::fixed_buffer must be standard layout"
);

template<class buffer_type,
         size_t SIZE>
template<class F>
inline
ircd::buffer::fixed_buffer<buffer_type, SIZE>::fixed_buffer(F&& closure,
                                                            typename std::enable_if<std::is_invocable<F, args_type>::value, void>::type *)
{
	const mutable_buffer buf
	{
		reinterpret_cast<mutable_buffer::iterator>(this->data()), SIZE
	};

	closure(buf);
}

template<class buffer_type,
         size_t SIZE>
inline
ircd::buffer::fixed_buffer<buffer_type, SIZE>::fixed_buffer(const buffer_type &b)
noexcept
:array_type
{
	std::begin(b), std::end(b)
}
{}

template<class buffer_type,
         size_t SIZE>
inline
ircd::buffer::fixed_buffer<buffer_type, SIZE>::fixed_buffer(nullptr_t)
noexcept
:array_type{{0}}
{}

template<class buffer_type,
         size_t SIZE>
inline
ircd::buffer::fixed_buffer<buffer_type, SIZE>::operator
buffer_type()
noexcept
{
	return buffer_type
	{
		this->data(), SIZE,
	};
}

template<class buffer_type,
         size_t SIZE>
inline
ircd::buffer::fixed_buffer<buffer_type, SIZE>::operator
buffer_type()
const noexcept
{
	return buffer_type
	{
		this->data(), SIZE,
	};
}