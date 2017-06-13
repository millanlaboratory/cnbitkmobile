PACKAGE_NAME = @PACKAGE_NAME@

bin_SCRIPTS = mi_online_python

mi_online_pythondir = $(prefix)/bin

graphicsdir = $(prefix)/share/cnbipython/

graphics_DATA = graphics/blk_ring.gif	\
			    graphics/circle.gif		\
			    graphics/cross.gif		\
			    graphics/icon.gif		\
			    graphics/icon0.gif		\
			    graphics/icon1.gif		\
			    graphics/l_arrow.gif		\
			    graphics/ptr.gif			\
			    graphics/ptr_blue.gif	\
			    graphics/ptr_red.gif		\
			    graphics/r_arrow.gif		\
			    graphics/timeout.gif	

install-exec-local:
	python setup.py install \
		--prefix $(prefix) \
		--install-lib $(prefix)/lib/cnbipython/ \
		--record $(prefix)/lib/cnbipython/install_files.txt \
		--verbose

uninstall-local: 
	cat $(prefix)/lib/cnbipython/install_files.txt | xargs rm -rf 
	rm -rf $(prefix)/lib/cnbipython/
