AUTOMAKE_OPTIONS = foreign

BUILT_SOURCES =#
CLEANFILES =#

DEFS += -DIRCD_UNIT

AM_CPPFLAGS =#
AM_CPPFLAGS += -I$(top_srcdir)/include

AM_CXXFLAGS =#
AM_CXXFLAGS += -ftls-model=initial-exec

AM_LDFLAGS =#
AM_LDFLAGS += -version-info 3:2:0
AM_LDFLAGS += -Wl,--no-undefined-version
AM_LDFLAGS += -Wl,--icf=all
AM_LDFLAGS += -Wl,--sort-section=name
AM_LDFLAGS += -Wl,--unresolved-symbols=ignore-in-shared-libs
AM_LDFLAGS += -Wl,--wrap=pthread_create
AM_LDFLAGS += -Wl,--wrap=pthread_join
AM_LDFLAGS += -Wl,--wrap=pthread_tryjoin_np
AM_LDFLAGS += -Wl,--wrap=pthread_timedjoin_np
AM_LDFLAGS += -Wl,--wrap=pthread_clockjoin_np
AM_LDFLAGS += -Wl,--wrap=pthread_self
AM_LDFLAGS += -Wl,--wrap=pthread_setname_np
AM_LDFLAGS += -Wl,--wrap=_dl_signal_exception
AM_LDFLAGS += -Wl,-z,nodelete
AM_LDFLAGS += -Wl,-z,nodlopen
AM_LDFLAGS += -Wl,-z,lazy
AM_LDFLAGS += $(PLATFORM_LDFLAGS)

if CLANG_PCH
AM_CPPFLAGS += -include-pch $(top_srcdir)/include/ircd/ircd.pic.h.gch
else
AM_CPPFLAGS += -include ircd/ircd.pic.h
endif

if CLANG
if !CLANG_PCH
AM_CXXFLAGS += -fno-c++-static-destructors
endif
endif

if LTO
if GCC
AM_CXXFLAGS += -flto=auto
endif
endif

if LTO
if CLANG
AM_CXXFLAGS += -flto=thin
AM_CXXFLAGS += -fstrict-vtable-pointers
AM_CXXFLAGS += -fwhole-program-vtables
#AM_LDFLAGS += -Wl,-plugin-opt,-pass-remarks='.*'
endif
endif

if GOLD
AM_LDFLAGS += -Wl,--no-text-reorder
AM_LDFLAGS += -Wl,--weak-unresolved-symbols
AM_LDFLAGS += -Wl,--sort-common=descending
endif

if MINGW
AM_LDFLAGS += -Wl,--enable-runtime-pseudo-reloc
AM_LDFLAGS += -export-symbols-regex '*'
endif

CXXFLAGS += $(EXTRA_CXXFLAGS)
CPPFLAGS += $(EXTRA_CPPFLAGS)
LDFLAGS += $(EXTRA_LDFLAGS)

#
# libircd
#

libircddir = @libdir@
libircd_LTLIBRARIES =#
libircd_LTLIBRARIES += libircd.la

libircd_la_LDFLAGS =#
libircd_la_LDFLAGS += $(AM_LDFLAGS)
libircd_la_LDFLAGS += @ROCKSDB_LDFLAGS@
libircd_la_LDFLAGS += @JS_LDFLAGS@
libircd_la_LDFLAGS += @ICU_LDFLAGS@
libircd_la_LDFLAGS += @BOOST_LDFLAGS@
libircd_la_LDFLAGS += @OPENCL_LDFLAGS@
libircd_la_LDFLAGS += @SSL_LDFLAGS@
libircd_la_LDFLAGS += @PBC_LDFLAGS@
libircd_la_LDFLAGS += @CRYPTO_LDFLAGS@
libircd_la_LDFLAGS += @SODIUM_LDFLAGS@
libircd_la_LDFLAGS += @MAGIC_LDFLAGS@
libircd_la_LDFLAGS += @IMAGEMAGICK_LDFLAGS@
libircd_la_LDFLAGS += @PNG_LDFLAGS@
libircd_la_LDFLAGS += @ZSTD_LDFLAGS@
libircd_la_LDFLAGS += @SNAPPY_LDFLAGS@
libircd_la_LDFLAGS += @LZ4_LDFLAGS@
libircd_la_LDFLAGS += @Z_LDFLAGS@
libircd_la_LDFLAGS += @MALLOC_LDFLAGS@
libircd_la_LDFLAGS += @URING_LDFLAGS@

libircd_la_LIBADD =#
libircd_la_LIBADD += @ROCKSDB_LIBS@
libircd_la_LIBADD += @JS_LIBS@
libircd_la_LIBADD += @ICU_LIBS@
libircd_la_LIBADD += @BOOST_LIBS@
libircd_la_LIBADD += @PBC_LIBS@
libircd_la_LIBADD += @SSL_LIBS@
libircd_la_LIBADD += @CRYPTO_LIBS@
libircd_la_LIBADD += @SODIUM_LIBS@
libircd_la_LIBADD += @MAGIC_LIBS@
libircd_la_LIBADD += @IMAGEMAGICK_LIBS@
libircd_la_LIBADD += @PNG_LIBS@
libircd_la_LIBADD += @ZSTD_LIBS@
libircd_la_LIBADD += @SNAPPY_LIBS@
libircd_la_LIBADD += @LZ4_LIBS@
libircd_la_LIBADD += @Z_LIBS@
libircd_la_LIBADD += @URING_LIBS@
libircd_la_LIBADD += $(EXTRA_LIBS)

#
# Unit list
#
# Since this is a GNU C++ project we assume the non-standard respect for
# the order of this sources list. Static initialization will occur in
# the same order as this list, and destruction in the reverse order.
# Add the most dependent units at the end and the least dependent units
# at the beginning.
#

libircd_la_SOURCES =#
#libircd_la_SOURCES += ctx_posix.cc
libircd_la_SOURCES += allocator.cc

#
# Specific unit option composition
#

ROCKSDB_SRC_CPPFLAGS =#
ROCKSDB_SRC_CPPFLAGS += -isystem $(top_srcdir)/deps/rocksdb/include
ROCKSDB_SRC_CPPFLAGS += -isystem $(top_srcdir)/deps/rocksdb

GPT_FP_CXXFLAGS =#
#TODO: XXX
if !CLANG_PCH
GPT_FP_CXXFLAGS += -fno-math-errno
GPT_FP_CXXFLAGS += -fno-trapping-math
GPT_FP_CXXFLAGS += -ffinite-math-only
GPT_FP_CXXFLAGS += -fno-signed-zeros
GPT_FP_CXXFLAGS += -fassociative-math
GPT_FP_CXXFLAGS += -freciprocal-math
GPT_FP_CXXFLAGS += -ffp-contract=fast
if CLANG
GPT_FP_CXXFLAGS += -fdenormal-fp-math=positive-zero
GPT_FP_CXXFLAGS += -ffp-model=fast
endif
endif

###############################################################################
#
# Hardware Acceleration / sub-targets
#

if CLANG
if OPENCL

GPU_OPT = opt-${CXX_EPOCH}
GPU_LINK = llvm-link-${CXX_EPOCH}
GPU_SPIRV = llvm-spirv-${CXX_EPOCH}

GPU_CPPFLAGS =#
GPU_CPPFLAGS += -Dcl_clang_storage_class_specifiers
GPU_CPPFLAGS += -DNOFP64
GPU_CPPFLAGS += -I$(top_srcdir)/include
GPU_CPPFLAGS += -include "ircd/config.h"
GPU_CPPFLAGS += $(CPPFLAGS)
GPU_CPPFLAGS += $(CWARNS)
GPU_CPPFLAGS += $(DEFS)

GPU_CFLAGS =#
GPU_CFLAGS += -fident
GPU_CFLAGS += -fno-builtin
GPU_CFLAGS += -fverbose-asm
GPU_CFLAGS += -fno-discard-value-names
GPU_CFLAGS += -mllvm -propagate-attrs=true
GPU_CFLAGS += -O3
GPU_CFLAGS += -fno-trapping-math
GPU_CFLAGS += -ffinite-math-only
GPU_CFLAGS += -fno-signed-zeros
GPU_CFLAGS += -ffp-contract=fast
#GPU_CFLAGS += -include "ircd/config.h"
#GPU_CFLAGS += -include "clc/clc.h"
#GPU_CFLAGS += -Wassume
#GPU_CFLAGS += -Rpass-analysis=".*"
#GPU_CFLAGS += -Rpass-missed=".*"

GPU_LINKFLAGS =#
GPU_LINKFLAGS +=#

GPU_OPTFLAGS =#
GPU_OPTFLAGS += -propagate-attrs=true
GPU_OPTFLAGS += -O3
#GPU_OPTFLAGS += -opt-bisect-limit=-1
#GPU_OPTFLAGS += -debug-pass=Arguments
#GPU_OPTFLAGS += -pass-remarks='.*'

GPU_ASFLAGS =#
GPU_ASFLAGS += -fident
GPU_ASFLAGS += -fno-builtin
GPU_ASFLAGS += -mllvm -propagate-attrs=true
GPU_ASFLAGS += -mllvm -verify-machineinstrs
GPU_ASFLAGS += -O3
GPU_ASFLAGS += -Rpass-analysis=asm-printer
#GPU_ASFLAGS += -Rpass-analysis=".*"
#GPU_ASFLAGS += -mllvm -debug-pass=Arguments
#GPU_ASFLAGS += -mllvm -pass-remarks='.*'

#
#
#

endif # OPENCL
endif # CLANG