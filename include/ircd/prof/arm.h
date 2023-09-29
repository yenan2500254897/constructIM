#pragma once
#define HAVE_IRCD_PROF_ARM_H

/// ARM platform related
namespace ircd::prof::arm
{
	unsigned long long read_virtual_counter() noexcept;
}

#if defined(__aarch64__)
extern inline unsigned long long
__attribute__((always_inline, gnu_inline, artificial))
ircd::prof::arm::read_virtual_counter()
noexcept
{
	unsigned long long ret;
	asm volatile
	(
		"mrs %0, cntvct_el0"
		: "=r" (ret)
	);

	return ret;
}
#else
extern inline unsigned long long
__attribute__((always_inline, gnu_inline, artificial))
ircd::prof::arm::read_virtual_counter()
noexcept
{
	return 0;
}
#endif
