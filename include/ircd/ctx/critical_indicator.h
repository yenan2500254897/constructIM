#pragma once
#define HAVE_IRCD_CTX_CRITICAL_INDICATOR_H

namespace ircd::ctx {
inline namespace this_ctx
{
	struct critical_indicator;         // Indicates if yielding happened for a section
}}

/// An instance of critical_indicator reports if context switching happened.
///
/// A critical_indicator remains true after construction until a context switch
/// has occurred. It then becomes false. This is not an assertion and is
/// available in optimized builds for real use. For example, a context may
/// want to recompute some value after a context switch and opportunistically
/// skip this effort when this indicator shows no switch occurred.
///
class ircd::ctx::this_ctx::critical_indicator
{
	uint64_t state;

  public:
	uint64_t count() const             { return epoch(cur()) - state;          }
	operator bool() const              { return epoch(cur()) == state;         }

	critical_indicator()
	:state(epoch(cur()))
	{}
};
