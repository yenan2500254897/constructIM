#pragma once
#define HAVE_IRCD_STRN_H

// Vintage
namespace ircd
{
	struct strncpy;
}

/// This is a function. It works the same as the standard strncpy() but it has
/// some useful modernizations and may be informally referred to as strncpy++.
/// See ircd::strlcpy() documentation for reasoning details.
///
struct ircd::strncpy
{
	mutable_buffer ret;

  public:
	operator string_view() const
	{
		return ret;
	}

	operator size_t() const
	{
		return size(ret);
	}

	strncpy(char *const &dst, const string_view &src, const size_t &max)
	:ret{[&]() -> mutable_buffer
	{
		const auto len(std::min(src.size(), max));
		buffer::copy(mutable_buffer(dst, len), src);
		return { dst, len };
	}()}
	{}

	#ifndef HAVE_STRNCPY
	strncpy(char *const &dst, const char *const &src, const size_t &max)
	:strncpy{dst, string_view{src, strnlen(src, max)}, max}
	{}
	#endif

	strncpy(const mutable_buffer &dst, const string_view &src)
	:strncpy{data(dst), src, size(dst)}
	{}
};
