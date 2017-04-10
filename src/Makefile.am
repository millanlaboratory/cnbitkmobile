AM_CXXFLAGS = -std=c++11

lib_LTLIBRARIES = libcnbimobile.la

libcnbimobile_includedir = $(includedir)/cnbimobile
libcnbimobile_include_HEADERS = CmEngine.hpp

libcnbimobile_la_SOURCES = CmEngine.hpp \
						   CmEngine.cpp

libcnbimobile_la_LIBADD = -ldrawtk -lSDL -lcnbicore
libcnbimobile_la_LDFLAGS = $(AM_CFLAGS) \
						-version-info $(CURRENT):$(REVISION):$(AGE)

noinst_PROGRAMS = test_engine

test_engine_SOURCES = test_engine.cpp
test_engine_LDADD = -lcnbicore -ldrawtk -lcnbimobile
	      

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cnbimobile.pc

