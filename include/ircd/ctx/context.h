#pragma once
#define HAVE_IRCD_CTX_CONTEXT_H

namespace ircd::ctx
{
	struct context;

	extern const size_t DEFAULT_STACK_SIZE;

	void swap(context &a, context &b) noexcept;
}

/// Principal interface for a context.
///
/// This object creates, holds and destroys a context with an interface
/// similar to that of std::thread.
///
/// The function passed to the constructor is executed on a new stack. By
/// default this execution will start to occur before this object is even
/// fully constructed. To delay child execution pass the `POST` flag to
/// the constructor; the execution will then be posted to the io_service
/// event queue instead. `DISPATCH` is an alternative, see boost::asio docs.
///
/// When this object goes out of scope the context is interrupted and joined
/// if it has not been already; the current context will wait for this to
/// complete. If the child context does not cooperate the destructor will hang.
/// To prevent this behavior `detach()` the ctx from this handler object; the
/// detached context will free its own resources when finished executing.
///
/// To wait for the child context to finish use `join()`. Calling `interrupt()`
/// will cause an `interrupted` exception to be thrown down the child's stack
/// from the next interruption point; a context switch is an interruption point
/// and so the context will wake up in its exception handler.
///
struct ircd::ctx::context
{
	enum flags :uint32_t;
	using function = std::function<void ()>;

  private:
	std::unique_ptr<ctx> c;

  public:
	operator const ctx &() const                 { return *c;                                      }
	operator ctx &()                             { return *c;                                      }

	bool operator!() const                       { return !c;                                      }
	operator bool() const                        { return bool(c);                                 }
	bool joined() const                          { return !c || ircd::ctx::finished(*c);           }

	void terminate()                             { ircd::ctx::terminate(*c);                       }
	void interrupt()                             { ircd::ctx::interrupt(*c);                       }
	void join();                                 // Blocks the current context until this one finishes
	ctx *detach();                               // other calls undefined after this call

	// Note: Constructing with DETACH flag makes any further use of this object undefined.
	context(const string_view &name,
	        const mutable_buffer &stack,
	        const flags &,
	        function);

	context(const string_view &name,
	        const size_t &stack_size,
	        const flags &,
	        function);

	context(const string_view &name,
	        const size_t &stack_size,
	        function,
	        const flags & = (flags)0);

	context(const string_view &name,
	        const flags &,
	        function);

	context(const string_view &name,
	        function,
	        const flags & = (flags)0);

	context(function,
	        const flags & = (flags)0);

	context();
	context(context &&) noexcept;
	context(const context &) = delete;
	context &operator=(context &&) noexcept;
	context &operator=(const context &) = delete;
	~context() noexcept;

	friend void swap(context &a, context &b) noexcept;
};

enum ircd::ctx::context::flags
:uint32_t
{
	POST            = 0x0001,   ///< Defers spawn with an ios.post()
	DISPATCH        = 0x0002,   ///< Defers spawn with an ios.dispatch()
	DEFER           = 0x0004,   ///< Defers spawn with an ios.defer()
	DETACH          = 0x0008,   ///< Context deletes itself; see struct context constructor notes
	NOINTERRUPT     = 0x0010,   ///< Interruption points won't throw while lit.
	SLICE_EXEMPT    = 0x0020,   ///< The watchdog will ignore excessive cpu usage.
	STACK_EXEMPT    = 0x0040,   ///< The watchdog will ignore excessive stack usage.
	WAIT_JOIN       = 0x0080,   ///< Destruction of instance won't terminate ctx.

	INTERRUPTED     = 0x4000,   ///< (INDICATOR) Marked
	TERMINATED      = 0x8000,   ///< (INDICATOR)
};

inline void
ircd::ctx::swap(context &a, context &b)
noexcept
{
	std::swap(a.c, b.c);
}
