#pragma once
#define HAVE_IRCD_UTIL_MASK_H

namespace ircd {
inline namespace util
{
	template<class T> T popmask(const T) noexcept;
	template<class T> T boolmask(const T) noexcept;
}}

/// Convenience template. Extends a bool value where the lsb is 1 or 0 into a
/// mask value like the result of vector comparisons.
template<class T>
inline T
ircd::util::boolmask(const T a)
noexcept
{
	return ~(popmask(a) - 1);
}

/// Convenience template. Vector compare instructions yield 0xff on equal;
/// sometimes one might need an actual value of 1 for accumulators or maybe
/// some bool-type reason...
template<class T>
inline T
ircd::util::popmask(const T a)
noexcept
{
	return a & 1;
}
