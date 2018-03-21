AM_CXXFLAGS = -std=c++11 


lib_LTLIBRARIES = libcnbimobile.la
#
libcnbimobile_includedir = $(includedir)/cnbimobile
libcnbimobile_include_HEADERS = CmEngine.hpp \
								CmFeedback.hpp 
#								Integrator.hpp \
#								Exponential.hpp

libcnbimobile_la_SOURCES = CmEngine.hpp \
						   CmEngine.cpp \
						   CmFeedback.hpp \
						   CmFeedback.cpp 
#						   Integrator.hpp \
#						   Integrator.cpp \
#						   Exponential.hpp \
#						   Exponential.cpp
#
libcnbimobile_la_LIBADD = -ldrawtk -lSDL -lcnbicore -lcnbiconfig
libcnbimobile_la_LDFLAGS = $(AM_CFLAGS) \
						-version-info $(CURRENT):$(REVISION):$(AGE)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = cnbimobile.pc

bin_PROGRAMS = mi_mobile_offline \
			   smrinc_robotino_run_generator
			   
mi_mobile_offline_SOURCES = mi_mobile_offline.cpp
mi_mobile_offline_LDADD = -lcnbicore -lcnbiloop -ltobicore \
						   -ltobiid -lcnbiconfig -lcnbimobile 

smrinc_robotino_run_generator_SOURCES = smrinc_robotino_run_generator.cpp


#noinst_PROGRAMS = test_engine \
#				  test_control
#
#test_engine_SOURCES = test_engine.cpp
#test_engine_LDADD = -lcnbicore -ldrawtk -lcnbimobile
#
#test_control_SOURCES = test_control.cpp
#test_control_LDADD = -lcnbicore -ldrawtk -lcnbimobile
#
#
#mi_integrator_SOURCES = mi_integrator.cpp
#mi_integrator_LDADD   = -lcnbicore -lcnbiloop -ltobicore \
#						-ltobiid -ltobiic -lcnbiconfig \
#					    -lcnbimobile
#

