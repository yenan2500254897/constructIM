#pragma once
#define HAVE_IRCD_STRL_H


// Vintage
namespace ircd
{
	struct strlcpy;
	struct strlcat;
}

/// This is a function. It works the same as the standard strlcpy() but it has
/// some useful modernizations and may be informally referred to as strlcpy++.
///
/// - It optionally works with string_view inputs and ircd::buffer outputs.
/// This allows for implicit size parameters and increases its safety while
/// simplifying its usage (no more sizeof(buf) where buf coderots into char*).
///
/// - Its objectification allows for a configurable return type. The old
/// strlcpy() returned a size integer type. When using string_view's and
/// buffers this would generally lead to the pattern { dst, strlcpy(dst, src) }
/// and this is no longer necessary.
///
struct ircd::strlcpy
{
	mutable_buffer ret;

  public:
	explicit operator const mutable_buffer &() const
	{
		return ret;
	}

	operator string_view() const
	{
		return ret;
	}

	operator size_t() const
	{
		return size(ret);
	}

	strlcpy(char *const &dst, const string_view &src, const size_t &max)
	noexcept
	{
		mutable_buffer buf{dst, max};
		ret = mutable_buffer
		{
			buf, std::min(size(src), std::max(max, 1UL) - 1UL)
		};

		consume(buf, copy(ret, src));
		consume(buf, copy(buf, '\0'));
	}

	#ifndef HAVE_STRLCPY
	strlcpy(char *const &dst, const char *const &src, const size_t &max)
	:strlcpy{dst, string_view{src, strnlen(src, max)}, max}
	{}
	#endif

	strlcpy(const mutable_buffer &dst, const string_view &src)
	:strlcpy{data(dst), src, size(dst)}
	{}
};

/// This is a function. It works the same as the standard strlcat() but it has
/// some useful modernizations and may be informally referred to as strlcat++.
/// see: ircd::strlcpy().
///
struct ircd::strlcat
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

	strlcat(char *const &dst, const string_view &src, const size_t &max)
	noexcept
	{
		const auto pos
		{
			strnlen(dst, max)
		};

		assert(pos <= max);
		const mutable_buffer tgt
		{
			dst + pos, max - pos
		};

		ret = mutable_buffer
		{
			dst, end(mutable_buffer(strlcpy(tgt, src)))
		};
	}

	#ifndef HAVE_STRLCAT
	strlcat(char *const &dst, const char *const &src, const size_t &max)
	:strlcat{dst, string_view{src, strnlen(src, max)}, max}
	{}
	#endif

	strlcat(const mutable_buffer &dst, const string_view &src)
	:strlcat{data(dst), src, size(dst)}
	{}
};
