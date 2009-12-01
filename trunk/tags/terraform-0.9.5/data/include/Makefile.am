SUBDIRS = atmospheres earth_textures skies water
datadir = @datadir@/terraform/include

EXTRA_DIST = defaults.inc \
            generic_land.inc \
            generic_river.inc \
            river_texture.inc \
            basic_textures.inc \
            grasstex.inc \
            texture_constructor.inc \
            texture_constructor2.inc \
            trace_objects.inc


data_DATA = defaults.inc \
            generic_land.inc \
            generic_river.inc \
            river_texture.inc \
            basic_textures.inc \
            grasstex.inc \
            texture_constructor.inc \
            texture_constructor2.inc \
            trace_objects.inc

install_home: 
	echo "$(HOME)/.terraform"; 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/include" ]; then \
	  mkdir "$(HOME)/.terraform/include" ; \
	fi;
	cp -f *.inc "$(HOME)/.terraform/include";
	list='$(SUBDIRS)'; \
	for subdir in $$list; do \
	  (cd $$subdir && $(MAKE) $(AM_MAKEFLAGS) install_home); \
	done;

