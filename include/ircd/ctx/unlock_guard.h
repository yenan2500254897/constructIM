#pragma once
#define HAVE_IRCD_CTX_UNLOCK_GUARD_H

namespace ircd::ctx
{
	template<class lockable> struct unlock_guard;
}

/// Inverse of std::lock_guard<>
template<class lockable>
struct ircd::ctx::unlock_guard
{
	lockable &l;

	unlock_guard(lockable &l);
	unlock_guard(unlock_guard &&) = delete;
	unlock_guard(const unlock_guard &) = delete;
	~unlock_guard() noexcept;
};

namespace ircd
{
	using ctx::unlock_guard;
}

template<class lockable>
ircd::ctx::unlock_guard<lockable>::unlock_guard(lockable &l)
:l{l}
{
	l.unlock();
}

template<class lockable>
ircd::ctx::unlock_guard<lockable>::~unlock_guard()
noexcept
{
	l.lock();
}
