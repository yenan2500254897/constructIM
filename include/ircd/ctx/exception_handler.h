#pragma once
#define HAVE_IRCD_CTX_EXCEPTION_HANDLER_H

namespace ircd::ctx
{
	struct exception_handler;
}

/// An instance of exception_handler must be present to allow a context
/// switch inside a catch block. This is due to ABI limitations that stack
/// exceptions with thread-local assumptions and don't expect catch blocks
/// on the same thread to interleave when we switch the stack.
///
/// We first increment the refcount for the caught exception so it remains
/// intuitively accessible for the rest of the catch block. Then the presence
/// of this object makes the ABI believe the catch block has ended.
///
/// The exception cannot then be rethrown. DO NOT RETHROW THE EXCEPTION.
///
struct ircd::ctx::exception_handler
:std::exception_ptr
{
	static uint uncaught_exceptions(const uint &) noexcept;
	static uint uncaught_exceptions() noexcept;
	static void end_catch() noexcept;

  public:
	exception_handler() noexcept;
	exception_handler(exception_handler &&) = delete;
	exception_handler(const exception_handler &) = delete;
	exception_handler &operator=(exception_handler &&) = delete;
	exception_handler &operator=(const exception_handler &) = delete;
};
