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