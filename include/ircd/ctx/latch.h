#pragma once
#define HAVE_IRCD_CTX_LATCH_H

namespace ircd::ctx
{
	class latch;
}

class ircd::ctx::latch
{
	mutable dock d;
	size_t count {0};

  public:
	bool is_ready() const noexcept;
	void count_down(const size_t &n = 1);
	void count_down_and_wait();
	void wait() const;

	latch(const size_t &count);
	latch() = default;
	latch(latch &&);
	latch(const latch &) = delete;
	~latch() noexcept;
};

inline
ircd::ctx::latch::latch(const size_t &count)
:count{count}
{}

inline
ircd::ctx::latch::latch(latch &&o)
:d{std::move(o.d)}
,count{std::move(o.count)}
{
	o.count = 0;
}

inline
ircd::ctx::latch::~latch()
noexcept
{
	assert(d.empty());
}

inline void
ircd::ctx::latch::wait()
const
{
	d.wait(std::bind(&latch::is_ready, this));
}

inline void
ircd::ctx::latch::count_down_and_wait()
{
	if(--count == 0)
		d.notify_all();
	else
		wait();
}

inline void
ircd::ctx::latch::count_down(const size_t &n)
{
	count -= n;
	if(is_ready())
		d.notify_all();
}


inline bool
ircd::ctx::latch::is_ready()
const noexcept
{
	return count == 0;
}
