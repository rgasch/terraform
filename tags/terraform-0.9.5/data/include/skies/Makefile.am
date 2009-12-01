SUBDIRS = include
datadir = @datadir@/terraform/include/skies

EXTRA_DIST = earth_regular_sky.inc \
             sky_system.inc \
             mapped_sky.inc

data_DATA = earth_regular_sky.inc \
            sky_system.inc \
            mapped_sky.inc

install_home: 
	echo "$(HOME)/.terraform"; 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include" ]; then \
	  mkdir "$(HOME)/.terraform/include" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include/skies" ]; then \
	  mkdir "$(HOME)/.terraform/include/skies" ; \
	fi;
	cp -f *.inc "$(HOME)/.terraform/include/skies";
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) install_home); \
	done;

