#pragma once
#define HAVE_IRCD_CTX_OLE_H

/// Context OffLoad Engine (OLE).
///
/// This system allows moving a task off the main IRCd thread by passing a function to a
/// worker thread for execution. The context on the main IRCd thread yields until the offload
/// function has returned (or thrown).
///
namespace ircd::ctx::ole
{
	struct init;
	struct opts;
	struct offload;
}

namespace ircd::ctx
{
	using ole::offload;
}

struct ircd::ctx::ole::offload
{
	using function = std::function<void ()>;

	offload(const opts &, const function &);
	offload(const function &);
};

struct ircd::ctx::ole::opts
{
	/// Optionally give this offload task a name for any tasklist.
	string_view name;

	/// The function will be executed on each thread.
	size_t concurrency {1};

	/// Queuing priority; in the form of a nice value.
	int8_t prio {0};
};

struct [[gnu::visibility("hidden")]]
ircd::ctx::ole::init
{
	init();
	~init() noexcept;
};
