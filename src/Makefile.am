lib_LTLIBRARIES = libcnbimobile.la

libcnbimobile_includedir = $(includedir)/cnbimobile
libcnbimobile_include_HEADERS = CmControl.hpp

libcnbimobile_la_SOURCES = CmControl.hpp \
						   CmControl.cpp

libcnbimobile_la_LIBADD = -ldrawtk -lSDL -lcnbicore
libcnbimobile_la_LDFLAGS = $(AM_CFLAGS) \
						-version-info $(CURRENT):$(REVISION):$(AGE)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cnbimobile.pc

