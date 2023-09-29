#pragma once
#define HAVE_IRCD_CTX_WAIT_H

namespace ircd::ctx { inline namespace this_ctx
{
	// Returns when context is woken up.
	void wait();

	// Return remaining time if notified; or <= 0 if not, and timeout thrown on throw overloads
	microseconds wait(const microseconds &, const std::nothrow_t &);

	template<class E,
	         class duration>
	nothrow_overload<E, duration> wait(const duration &);

	// timeout thrown if time point reached
	template<class E = timeout,
	         class duration>
	throw_overload<E, duration> wait(const duration &);

	// Returns false if notified; true if time point reached
	bool wait_until(const system_point &tp, const std::nothrow_t &);

	// Returns false if notified; true if time point reached
	template<class E>
	nothrow_overload<E, bool> wait_until(const system_point &tp);

	// timeout thrown if time point reached
	template<class E = timeout>
	throw_overload<E> wait_until(const system_point &tp);
}}

/// Wait for a notification until a point in time. If there is a notification
/// then context continues normally. If there's never a notification then an
/// exception (= timeout) is thrown.
/// interruption point.
template<class E>
inline ircd::throw_overload<E>
ircd::ctx::this_ctx::wait_until(const system_point &tp)
{
	if(unlikely(wait_until<std::nothrow_t>(tp)))
		throw E{};
}

/// Wait for a notification until a point in time. If there is a notification
/// then returns false. If there's never a notification then returns true.
/// interruption point. this is not noexcept.
template<class E>
inline ircd::nothrow_overload<E, bool>
ircd::ctx::this_ctx::wait_until(const system_point &tp)
{
	return wait_until(tp, std::nothrow);
}

/// Wait for a notification for at most some amount of time. If the duration is
/// reached without a notification then E (= timeout) is thrown. Otherwise,
/// returns the time remaining on the duration.
/// interruption point
template<class E,
         class duration>
inline ircd::throw_overload<E, duration>
ircd::ctx::this_ctx::wait(const duration &d)
{
	const auto ret
	{
		wait<std::nothrow_t>(d)
	};

	if(unlikely(ret <= duration(0)))
		throw E{};

	return ret;
}

/// Wait for a notification for some amount of time. This function returns
/// when a context is notified. It always returns the duration remaining which
/// will be <= 0 to indicate a timeout without notification.
/// interruption point. this is not noexcept.
template<class E,
         class duration>
inline ircd::nothrow_overload<E, duration>
ircd::ctx::this_ctx::wait(const duration &d)
{
	const auto ret
	{
		wait(duration_cast<microseconds>(d), std::nothrow)
	};

	return duration_cast<duration>(ret);
}
