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
	-L$(top_srcdir)/ircd \
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
	###