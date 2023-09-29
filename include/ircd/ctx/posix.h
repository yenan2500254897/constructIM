namespace ircd::ctx::posix
{
	struct enable_pthread;
	struct disable_pthread;

	// extern log::log log;
	extern int enable_hook; // -1 = pthread; 0 = auto; 1 = ircd::ctx.
	extern std::vector<context> ctxs;
}

/// Instances of this object force all pthread_create() to create real
/// pthreads. By default that decision is internally automated. The
/// assertion made by this object takes precedence over instances of
/// disable_pthread.
struct ircd::ctx::posix::enable_pthread
{
	int theirs
	{
		enable_hook
	};

	enable_pthread(const bool &ours = true)
	{
		enable_hook = ours? -1: theirs;
	}

	~enable_pthread() noexcept
	{
		enable_hook = theirs;
	}
};

/// Instances of this object force all pthread_create() to create ircd::context
/// userspace threads rather than real pthreads(). By default this is
/// determined internally, but instances of this object will force that behavior
/// in all cases except when instances of enable_pthread exist, which takes
/// precedence.
struct ircd::ctx::posix::disable_pthread
{
	int theirs
	{
		enable_hook
	};

	disable_pthread(const bool &ours = true)
	{
		enable_hook = ours? 1: theirs;
	}

	~disable_pthread() noexcept
	{
		enable_hook = theirs;
	}
};
