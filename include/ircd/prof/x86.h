#pragma once
#define HAVE_IRCD_PROF_X86_H

/// X86 platform related
namespace ircd::prof::x86
{
	unsigned long long rdpmc(const uint &) noexcept;
	unsigned long long rdtscp() noexcept;
	unsigned long long rdtsc() noexcept;
}

#if defined(__x86_64__) || defined(__i386__)
extern inline unsigned long long
__attribute__((always_inline, gnu_inline, artificial))
ircd::prof::x86::rdtsc()
noexcept
{
	return __builtin_ia32_rdtsc();
}
#else
inline unsigned long long
ircd::prof::x86::rdtsc()
noexcept
{
	return 0;
}
#endif

#if defined(__x86_64__) || defined(__i386__)
extern inline unsigned long long
__attribute__((always_inline, gnu_inline, artificial))
ircd::prof::x86::rdtscp()
noexcept
{
	uint32_t ia32_tsc_aux;
	return __builtin_ia32_rdtscp(&ia32_tsc_aux);
}
#else
inline unsigned long long
ircd::prof::x86::rdtscp()
noexcept
{
	return 0;
}
#endif

#if defined(__x86_64__) || defined(__i386__)
extern inline unsigned long long
__attribute__((always_inline, gnu_inline, artificial))
ircd::prof::x86::rdpmc(const uint &c)
noexcept
{
	return __builtin_ia32_rdpmc(c);
}
#else
inline unsigned long long
ircd::prof::x86::rdpmc(const uint &c)
noexcept
{
	return 0;
}
#endif
