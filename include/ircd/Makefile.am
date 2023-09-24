AUTOMAKE_OPTIONS = foreign

AM_CPPFLAGS =#
AM_CPPFLAGS += -I$(top_srcdir)/include

AM_CXXFLAGS =#
AM_CXXFLAGS += -x c++-header

if CLANG11
AM_CXXFLAGS += -fpch-instantiate-templates
endif

if GCC
if LOWMEM_COMPILE
LOWMEM_FLAGS= --param ggc-min-expand=1
endif
endif

CPPFLAGS += $(EXTRA_CPPFLAGS)
CXXFLAGS += $(EXTRA_CXXFLAGS)

# includedir = @includedir@/$(PACKAGE_TARNAME)
includedir = @includedir@/ircd
nobase_include_HEADERS = $(DEPS)

if BUILD_PCH # --- Actions when Precompiled headers enabled

noinst_HEADERS = *.gch
BUILT_SOURCES = \
	ircd.h.gch \
	ircd.pic.h.gch \
	asio.h.gch \
	###

ircd.h.gch: ircd.h
	$(CXX) $(AM_CXXFLAGS) $(CXXFLAGS) -o ircd.h.gch $(DEFS) $(AM_CPPFLAGS) $(CPPFLAGS) ircd.h

ircd.pic.h.gch: ircd.h
	cp ircd.h ircd.pic.h
	$(CXX) $(AM_CXXFLAGS) $(CXXFLAGS) -fPIC -o ircd.pic.h.gch $(DEFS) $(AM_CPPFLAGS) $(CPPFLAGS) -DPIC ircd.pic.h

asio.h.gch: ircd.h asio.h
	$(CXX) $(AM_CXXFLAGS) $(CXXFLAGS) -fPIC -o asio.h.gch $(DEFS) $(AM_CPPFLAGS) $(CPPFLAGS) -DPIC asio.h

asio.h.gch: \
	AM_CPPFLAGS += @BOOST_CPPFLAGS@ @SSL_CPPFLAGS@ @CRYPTO_CPPFLAGS@

else # --- Actions without Precompiled Headers enabled

BUILT_SOURCES = \
	ircd.pic.h \
	###

ircd.pic.h: ircd.h
	cp ircd.h ircd.pic.h

endif # --- /BUILD_PCH

#
# Clean / Install
#

clean-local:
	rm -f *.h.gch
	rm -f *.gch.tmp
	rm -f ircd.pic.h
	rm -f matrix.pic.h

install-mkdirs: $(DEPS) $(BUILT_SOURCES)
	-@if test ! -d $(DESTDIR)$(includedir); then \
		echo "mkdir -p $(DESTDIR)$(includedir)"; \
		mkdir -p $(DESTDIR)$(includedir); \
	fi

install: install-mkdirs
	@echo "ircd: installing headers"
	@for i in $(srcdir)/*; do \
		case "$$i" in \
			"./Makefile"*) \
				;; \
			*".h.gch") \
				;; \
			*) \
				echo "installing $$i to $(DESTDIR)$(includedir)"; \
				cp -u -r $$i $(DESTDIR)$(includedir); \
				;; \
		esac \
	done