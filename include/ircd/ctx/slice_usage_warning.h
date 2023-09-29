#pragma once
#define HAVE_IRCD_CTX_SLICE_USAGE_WARNING_H

namespace ircd::ctx {
inline namespace this_ctx
{
	struct slice_usage_warning;
}}

#ifdef RB_DEBUG
struct ircd::ctx::this_ctx::slice_usage_warning
{
	string_view fmt;
	va_rtti ap;
	ulong epoch;
	ulong start;

	template<class... args>
	slice_usage_warning(const string_view &fmt, args&&... ap)
	:slice_usage_warning{fmt, va_rtti{ap...}}
	{}

	explicit slice_usage_warning(const string_view &fmt, va_rtti &&ap);
	~slice_usage_warning() noexcept;
};
#else
struct ircd::ctx::this_ctx::slice_usage_warning
{
	#if defined(__clang__)
	template<class... args>
	slice_usage_warning(const string_view &fmt, args&&...)
	{}
	#else
	// In at least gcc 6.3.0 20170519, template param packs are not DCE'ed
	// so legacy varargs are used here.
	slice_usage_warning(const string_view &fmt, ...)
	{}
	#endif
};
#endif
