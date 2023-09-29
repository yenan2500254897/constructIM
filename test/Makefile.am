AUTOMAKE_OPTIONS = foreign

prefix = @prefix@

CXXFLAGS += \
	$(EXTRA_CXXFLAGS) \
	###

AM_CXXFLAGS = \
	-ftls-model=initial-exec \
	###

AM_CPPFLAGS = \
	-I$(top_srcdir)/include \
	$(EXTRA_CPPFLAGS) \
	###

AM_LDFLAGS = \
	-dlopen self \
	-Wl,--warn-execstack \
	-Wl,--warn-common \
	-Wl,--detect-odr-violations \
	-Wl,--unresolved-symbols=report-all \
	-Wl,--allow-shlib-undefined \
	-Wl,--dynamic-list-data \
	-Wl,--dynamic-list-cpp-new\
	-Wl,--dynamic-list-cpp-typeinfo \
	-Wl,--rosegment \
	-Wl,-z,noexecstack \
	-Wl,--wrap=epoll_wait \
	-Wl,--wrap=io_uring_queue_init \
	-L$(top_srcdir)/ircd \
	$(PLATFORM_LDFLAGS) \
	$(EXTRA_LDFLAGS) \
	###

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

if CLANG_PCH
ASIO_UNIT_CPPFLAGS += -include-pch $(top_srcdir)/include/ircd/asio.h.gch
else
ASIO_UNIT_CPPFLAGS += -include ircd/asio.h
endif

bin_PROGRAMS = testAllocator testBuffer testSystemTime testCtx

testAllocator_LDFLAGS = \
	$(AM_LDFLAGS) \
	###

testAllocator_LDADD = \
	-lircd \
	$(EXTRA_LIBS) \
	###

testAllocator_SOURCES = \
	test_allocator.cc   \
	###

testBuffer_LDFLAGS = \
	$(AM_LDFLAGS) \
	###

testBuffer_LDADD = \
	-lircd \
	$(EXTRA_LIBS) \
	###

testBuffer_SOURCES = \
	test_buffer.cc    \
	###

testSystemTime_LDFLAGS = \
	$(AM_LDFLAGS) \
	###

testSystemTime_LDADD = \
	-lircd \
	$(EXTRA_LIBS) \
	###

testSystemTime_SOURCES = \
	test_system_time.cc    \
	###

testCtx_LDFLAGS = \
	$(AM_LDFLAGS) \
	@BOOST_LDFLAGS@ \
	###

testCtx_LDADD = \
	-lircd \
	@BOOST_LIBS@ \
	$(EXTRA_LIBS) \
	###

testCtx_SOURCES = \
	test_ctx.cc    \
	###

test_ctx.o:            AM_CPPFLAGS := ${AM_CPPFLAGS} ${ASIO_UNIT_CPPFLAGS}