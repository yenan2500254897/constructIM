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

bin_PROGRAMS = testEnv

testEnv_LDFLAGS = \
	$(AM_LDFLAGS) \
	###

testEnv_LDADD = \
	-lircd \
	$(EXTRA_LIBS) \
	###

testEnv_SOURCES = \
	test_env.cc    \
	test_allocator.cc   \
	###