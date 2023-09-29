#pragma once
#define HAVE_IRCD_CTX_SHARED_MUTEX_H

namespace ircd::ctx
{
	class shared_mutex;
}

struct ircd::ctx::shared_mutex
{
	dock q;
	ctx *u;
	ssize_t s;

  public:
	bool unique() const;
	bool upgrade() const;
	size_t shares() const;
	size_t waiting() const;

	bool can_lock() const;
	bool can_lock_shared() const;
	bool can_lock_upgrade() const;

	bool try_lock();
	bool try_lock_shared();
	bool try_lock_upgrade();

	template<class time_point> bool try_lock_until(time_point&&);
	template<class time_point> bool try_lock_shared_until(time_point&&);
	template<class time_point> bool try_lock_upgrade_until(time_point&&);

	template<class duration> bool try_lock_for(duration&&);
	template<class duration> bool try_lock_shared_for(duration&&);
	template<class duration> bool try_lock_upgrade_for(duration&&);

	void lock();
	void lock_shared();
	void lock_upgrade();

	bool try_unlock_shared_and_lock();
	bool try_unlock_shared_and_lock_upgrade();
	bool try_unlock_upgrade_and_lock();

	template<class time_point> bool try_unlock_shared_and_lock_until(time_point&&);
	template<class time_point> bool try_unlock_shared_and_lock_upgrade_until(time_point&&);
	template<class time_point> bool try_unlock_upgrade_and_lock_until(time_point&&);

	template<class duration> bool try_unlock_shared_and_lock_for(duration&&);
	template<class duration> bool try_unlock_shared_and_lock_upgrade_for(duration&&);
	template<class duration> bool try_unlock_upgrade_and_lock_for(duration&&);

	void unlock();
	void unlock_shared();
	void unlock_upgrade();
	void unlock_and_lock_shared();
	void unlock_and_lock_upgrade();
	void unlock_upgrade_and_lock();
	void unlock_upgrade_and_lock_shared();

	shared_mutex();
	shared_mutex(shared_mutex &&) noexcept;
	shared_mutex(const shared_mutex &) = delete;
	shared_mutex &operator=(shared_mutex &&) noexcept;
	shared_mutex &operator=(const shared_mutex &) = delete;
	~shared_mutex() noexcept;
};

inline
ircd::ctx::shared_mutex::shared_mutex()
:u{nullptr}
,s{0}
{
}

inline
ircd::ctx::shared_mutex::shared_mutex(shared_mutex &&o)
noexcept
:q{std::move(o.q)}
,u{std::move(o.u)}
,s{std::move(o.s)}
{
	o.s = 0;
	o.u = nullptr;
}

inline
ircd::ctx::shared_mutex &
ircd::ctx::shared_mutex::operator=(shared_mutex &&o)
noexcept
{
	this->~shared_mutex();
	q = std::move(o.q);
	u = std::move(o.u);
	s = std::move(o.s);
	o.s = 0;
	o.u = nullptr;
	return *this;
}

inline
ircd::ctx::shared_mutex::~shared_mutex()
noexcept
{
	assert(!u);
	assert(s == 0);
	assert(q.empty());
}

inline void
ircd::ctx::shared_mutex::unlock_upgrade_and_lock_shared()
{
	assert(u == current);
	assert(s >= 0);

	u = nullptr;
	++s;
	q.notify_one();
}

inline void
ircd::ctx::shared_mutex::unlock_upgrade_and_lock()
{
	assert(current);
	assert(u == current);

	q.wait([this]
	{
		return can_lock();
	});

	s = std::numeric_limits<decltype(s)>::min();
}

inline void
ircd::ctx::shared_mutex::unlock_and_lock_upgrade()
{
	assert(current);
	assert(u == current);
	assert(s == std::numeric_limits<decltype(s)>::min());

	s = 0;
}

inline void
ircd::ctx::shared_mutex::unlock_and_lock_shared()
{
	assert(current);
	assert(u == current);
	assert(s == std::numeric_limits<decltype(s)>::min());

	s = 1;
	q.notify_one();
}

inline void
ircd::ctx::shared_mutex::unlock_upgrade()
{
	assert(current);
	assert(u == current);

	u = nullptr;
	q.notify_one();
}

inline void
ircd::ctx::shared_mutex::unlock_shared()
{
	assert(s > 0);

	--s;
	q.notify_one();
}

inline void
ircd::ctx::shared_mutex::unlock()
{
	assert(current);
	assert(u == current);
	assert(s == std::numeric_limits<decltype(s)>::min());

	s = 0;
	q.notify_all();
}

template<class duration>
inline bool
ircd::ctx::shared_mutex::try_unlock_upgrade_and_lock_for(duration&& d)
{
	return try_unlock_upgrade_and_lock_until(system_clock::now() + d);
}

template<class duration>
inline bool
ircd::ctx::shared_mutex::try_unlock_shared_and_lock_upgrade_for(duration&& d)
{
	return try_unlock_shared_and_lock_upgrade_until(system_clock::now() + d);
}

template<class duration>
inline bool
ircd::ctx::shared_mutex::try_unlock_shared_and_lock_for(duration&& d)
{
	return try_unlock_shared_and_lock_until(system_clock::now() + d);
}

template<class time_point>
inline bool
ircd::ctx::shared_mutex::try_unlock_upgrade_and_lock_until(time_point&& tp)
{
	assert(0);
	return false;
}

template<class time_point>
inline bool
ircd::ctx::shared_mutex::try_unlock_shared_and_lock_upgrade_until(time_point&& tp)
{
	assert(0);
	return false;
}

template<class time_point>
inline bool
ircd::ctx::shared_mutex::try_unlock_shared_and_lock_until(time_point&& tp)
{
	assert(0);
	return false;
}

inline bool
ircd::ctx::shared_mutex::try_unlock_upgrade_and_lock()
{
	if(!try_lock())
		return false;

	return true;
}

inline bool
ircd::ctx::shared_mutex::try_unlock_shared_and_lock_upgrade()
{
	if(!try_lock_upgrade())
		return false;

	--s;
	return true;
}

inline bool
ircd::ctx::shared_mutex::try_unlock_shared_and_lock()
{
	assert(current);
	if(s != 1)
		return false;

	assert(!u);
	u = current;
	s = std::numeric_limits<decltype(s)>::min();
	return true;
}

inline void
ircd::ctx::shared_mutex::lock_upgrade()
{
	assert(current);
	q.wait([this]
	{
		return can_lock_upgrade();
	});

	u = current;
}

inline void
ircd::ctx::shared_mutex::lock_shared()
{
	q.wait([this]
	{
		return can_lock_shared();
	});

	++s;
}

inline void
ircd::ctx::shared_mutex::lock()
{
	assert(current);
	q.wait([this]
	{
		return can_lock();
	});

	u = current;
	s = std::numeric_limits<decltype(s)>::min();
}

template<class duration>
inline bool
ircd::ctx::shared_mutex::try_lock_upgrade_for(duration&& d)
{
	return try_lock_upgrade_until(system_clock::now() + d);
}

template<class duration>
inline bool
ircd::ctx::shared_mutex::try_lock_shared_for(duration&& d)
{
	return try_lock_shared_until(system_clock::now() + d);
}

template<class duration>
inline bool
ircd::ctx::shared_mutex::try_lock_for(duration&& d)
{
	return try_lock_until(system_clock::now() + d);
}

template<class time_point>
inline bool
ircd::ctx::shared_mutex::try_lock_upgrade_until(time_point&& tp)
{
	assert(current);
	const bool can_lock_upgrade
	{
		q.wait_until(tp, [this]
		{
			return this->can_lock_upgrade();
		})
	};

	if(can_lock_upgrade)
		u = current;

	return can_lock_upgrade;
}

template<class time_point>
inline bool
ircd::ctx::shared_mutex::try_lock_shared_until(time_point&& tp)
{
	assert(current);
	const bool can_lock_shared
	{
		q.wait_until(tp, [this]
		{
			return this->can_lock_shared();
		})
	};

	if(can_lock_shared)
		++s;

	return can_lock_shared;
}

template<class time_point>
inline bool
ircd::ctx::shared_mutex::try_lock_until(time_point&& tp)
{
	assert(current);
	const bool can_lock
	{
		q.wait_until(tp, [this]
		{
			return this->can_lock();
		})
	};

	if(can_lock)
	{
		u = current;
		s = std::numeric_limits<decltype(s)>::min();
	}

	return can_lock;
}

inline bool
ircd::ctx::shared_mutex::try_lock_upgrade()
{
	assert(current);
	if(can_lock_upgrade())
	{
		u = current;
		return true;
	}
	else return false;
}

inline bool
ircd::ctx::shared_mutex::try_lock_shared()
{
	++s;
	return s >= 0;
}

inline bool
ircd::ctx::shared_mutex::try_lock()
{
	assert(current);
	if(can_lock())
	{
		u = current;
		s = std::numeric_limits<decltype(s)>::min();
		return true;
	}
	else return false;
}

inline bool
ircd::ctx::shared_mutex::can_lock_upgrade()
const
{
	assert(u || can_lock_shared());
	return !u;
}

inline bool
ircd::ctx::shared_mutex::can_lock_shared()
const
{
	return s >= 0;
}

inline bool
ircd::ctx::shared_mutex::can_lock()
const
{
	return s == 0 && (!u || u == current);
}

inline size_t
ircd::ctx::shared_mutex::waiting()
const
{
	return q.size();
}

inline size_t
ircd::ctx::shared_mutex::shares()
const
{
	return std::max(s, ssize_t(0));
}

inline bool
ircd::ctx::shared_mutex::upgrade()
const
{
	return u;
}

inline bool
ircd::ctx::shared_mutex::unique()
const
{
	return s == std::numeric_limits<decltype(s)>::min();
}
