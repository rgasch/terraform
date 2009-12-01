SUBDIRS = include objects image_maps glade

datadir = @datadir@/terraform

install_home: 
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) install_home); \
	done;

