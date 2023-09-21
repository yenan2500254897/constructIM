#pragma once
#define HAVE_IRCD_BUFFER_BASE_H

/// Base for all buffer types
///
template<class it>
struct ircd::buffer::buffer
:std::pair<it, it>
{
	using iterator = it;
	using value_type = typename std::remove_pointer<iterator>::type;

	auto &begin() const                { return std::get<0>(*this);            }
	auto &begin()                      { return std::get<0>(*this);            }
	auto &end() const                  { return std::get<1>(*this);            }
	auto &end()                        { return std::get<1>(*this);            }

	bool null() const;
	bool empty() const;                // For boost::spirit conceptual compliance.

	auto &operator[](const size_t &i) const;
	auto &operator[](const size_t &i);

	explicit operator const it &() const;
	explicit operator std::string_view() const;
	explicit operator std::string() const;
	operator string_view() const;

	buffer(const buffer &start, const size_t &size);
	buffer(const it &start, const it &stop);
	buffer(const it &start, const size_t &size);
	buffer();
};

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::buffer()
:buffer{nullptr, nullptr}
{}

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::buffer(const it &start,
                                 const size_t &size)
:buffer{start, start + size}
{}

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::buffer(const buffer &start,
                                 const size_t &size)
:buffer
{
	data(start), std::min(ircd::buffer::size(start), size)
}
{}

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::buffer(const it &start,
                                 const it &stop)
:std::pair<it, it>{start, stop}
{
	//assert(this->begin() <= this->end());
}

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::operator std::string()
const
{
	return std::string(static_cast<std::string_view>(*this));
}

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::operator std::string_view()
const
{
	return static_cast<ircd::string_view>(*this);
}

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::operator string_view()
const
{
	return string_view
	{
		const_cast<const char *>(data(*this)), size(*this)
	};
}

template<class it>
inline auto &
__attribute__((always_inline))
ircd::buffer::buffer<it>::operator[](const size_t &i)
{
	assert(begin() + i < end());
	return *(begin() + i);
}

template<class it>
inline auto &
__attribute__((always_inline))
ircd::buffer::buffer<it>::operator[](const size_t &i)
const
{
	assert(begin() + i < end());
	return *(begin() + i);
}

template<class it>
inline bool
__attribute__((always_inline))
ircd::buffer::buffer<it>::empty()
const
{
	return null() || std::distance(begin(), end()) == 0;
}

template<class it>
inline bool
__attribute__((always_inline))
ircd::buffer::buffer<it>::null()
const
{
	return !begin();
}

template<class it>
inline __attribute__((always_inline))
ircd::buffer::buffer<it>::operator
const it &()
const
{
	return begin();
}