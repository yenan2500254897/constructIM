#ifndef HAVE_IRCD_IRCD_H
#define HAVE_IRCD_IRCD_H

//////////////////////////////////////////////////////////////////////////////>
//
// This is an aggregate header which ties together the general public
// interfaces for IRCd. Include this header to operate and embed the library in
// your application; no other includes from the project should be required.
//
// This header only includes standard library headers; we may forward declare
// third-party symbols, but only if their headers are not required.
//
//////////////////////////////////////////////////////////////////////////////>

//
// Project configuration
//
#include "config.h"
#include "stdinc.h"                    // Standard library includes.
#include "portable.h"
#include "stduse.h"

//
// Project library interfaces
//
#include "util/mask.h"
#include "util/align.h"
#include "string_view.h"
#include "buffer/buffer.h"
#include "allocator/allocator.h"
#include "util/util.h"
#include "exception.h"
#include "panic.h"
#include "terminate.h"
#include "strl.h"
#include "strn.h"
#include "time.h"
#include "fmt.h"

#endif