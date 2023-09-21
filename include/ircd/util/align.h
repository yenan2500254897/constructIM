#pragma once
#define HAVE_IRCD_UTIL_ALIGN_H

namespace ircd {
inline namespace util
{
	// Alignment constant expressions
	constexpr bool padded(const size_t, size_t alignment);
	constexpr bool aligned(const uintptr_t, size_t alignment);
	constexpr size_t padding(const size_t size, size_t alignment);
	constexpr size_t pad_to(const size_t size, const size_t alignment);
	constexpr uintptr_t align(uintptr_t, size_t alignment);
	constexpr uintptr_t align_up(uintptr_t, size_t alignment);
	constexpr ulong alignment(const uintptr_t);

	// Alignment inline tools
	bool aligned(const void *const &, const size_t &alignment);
	bool aligned(const volatile void *const &, const size_t &alignment);
	template<class T = char> const T *align(const void *const &, const size_t &alignment);
	template<class T = char> T *align(void *const &, const size_t &alignment);
	template<class T = char> const T *align_up(const void *const &, const size_t &alignment);
	template<class T = char> T *align_up(void *const &, const size_t &alignment);
	ulong alignment(const void *const &);
}}

[[gnu::always_inline]]
inline ulong
ircd::util::alignment(const void *const &ptr)
{
	return alignment(uintptr_t(ptr));
}

template<class T>
[[gnu::always_inline]]
inline T *
ircd::util::align(void *const &ptr,
                  const size_t &alignment)
{
	return reinterpret_cast<T *>
	(
		align(uintptr_t(ptr), alignment)
	);
}

template<class T>
[[gnu::always_inline]]
inline const T *
ircd::util::align(const void *const &ptr,
                  const size_t &alignment)
{
	return reinterpret_cast<const T *>
	(
		align(uintptr_t(ptr), alignment)
	);
}

template<class T>
[[gnu::always_inline]]
inline T *
ircd::util::align_up(void *const &ptr,
                     const size_t &alignment)
{
	return reinterpret_cast<T *>
	(
		align_up(uintptr_t(ptr), alignment)
	);
}

template<class T>
[[gnu::always_inline]]
inline const T *
ircd::util::align_up(const void *const &ptr,
                     const size_t &alignment)
{
	return reinterpret_cast<const T *>
	(
		align_up(uintptr_t(ptr), alignment)
	);
}

[[gnu::always_inline]]
inline bool
ircd::util::aligned(const void *const &ptr,
                    const size_t &alignment)
{
	return aligned(uintptr_t(ptr), alignment);
}

[[gnu::always_inline]]
inline bool
ircd::util::aligned(const volatile void *const &ptr,
                    const size_t &alignment)
{
	return aligned(uintptr_t(ptr), alignment);
}

constexpr ulong
ircd::util::alignment(const uintptr_t ptr)
{
	return 1UL << __builtin_ctzl(ptr);
}

constexpr uintptr_t
ircd::util::align_up(uintptr_t ptr,
                     size_t alignment)
{
	alignment = std::max(alignment, 1UL);
	ptr += (alignment - (ptr % alignment)) % alignment;
	return ptr;
}

constexpr uintptr_t
ircd::util::align(uintptr_t ptr,
                  size_t alignment)
{
	alignment = std::max(alignment, 1UL);
	ptr -= (ptr % alignment);
	return ptr;
}

constexpr size_t
ircd::util::pad_to(const size_t size,
                   const size_t alignment)
{
	return size + padding(size, alignment);
}

constexpr size_t
ircd::util::padding(const size_t size,
                    size_t alignment)
{
	alignment = std::max(alignment, 1UL);
	return (alignment - (size % alignment)) % alignment;
}

constexpr bool
ircd::util::aligned(const uintptr_t ptr,
                    size_t alignment)
{
	alignment = std::max(alignment, 1UL);
	return ptr % alignment == 0;
}

constexpr bool
ircd::util::padded(const size_t size,
                   size_t alignment)
{
	alignment = std::max(alignment, 1UL);
	return size % alignment == 0;
}
