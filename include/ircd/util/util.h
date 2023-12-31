#pragma once
#define HAVE_IRCD_UTIL_H

namespace ircd
{
	/// Utilities for IRCd.
	///
	/// This is an inline namespace: everything declared in it will be
	/// accessible in ircd::. By first opening it here as inline all
	/// subsequent openings of this namespace do not have to use the inline
	/// keyword but will still be inlined to ircd::.
	inline namespace util {}
}

//
// Fundamental macros
//

#define IRCD_EXPCAT(a, b)   a ## b
#define IRCD_CONCAT(a, b)   IRCD_EXPCAT(a, b)
#define IRCD_UNIQUE(a)      IRCD_CONCAT(a, __COUNTER__)

#include "unit_literal.h"
#include "unwind.h"
#include "enum.h"
#include "scope_restore.h"
#include "instance_list.h"
#include "typography.h"
#include "pubsetbuf.h"
#include "va_rtti.h"
#include "scope_count.h"
#include "string.h"
#include "pretty.h"
#include "what.h"
#include "closure.h"
#include "nothrow.h"

// Unsorted section
namespace ircd {
inline namespace util {



} // namespace util
} // namespace ircd
