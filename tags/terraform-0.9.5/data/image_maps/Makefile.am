datadir = @datadir@/terraform/image_maps

EXTRA_DIST = moon_moon.png \
             sky_test_320x180.tga 

data_DATA = moon_moon.png \
            sky_test_320x180.tga 

install_home: 
	if [ ! -d "$(HOME)/.terraform" ]; then \
	  mkdir "$(HOME)/.terraform" ; \
	fi;
	if [ ! -d "$(HOME)/.terraform/image_maps" ]; then \
	  mkdir "$(HOME)/.terraform/image_maps" ; \
	fi;
	cp -f *.png *.tga "$(HOME)/.terraform/image_maps";

