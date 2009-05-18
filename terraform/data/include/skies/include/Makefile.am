datadir = @datadir@/terraform/include/skies/include

EXTRA_DIST = clouds_01.inc \
             clouds_02.inc \
             clouds_03.inc \
             clouds_04.inc \
             clouds_05.inc \
             clouds_06.inc \
             clouds_07.inc \
             lights_standard.inc \
             lights_studio.inc \
             macro_skies.inc \
             skycolor_earth.inc \
             stars_01.inc \
             stars_02.inc \
             stars_03.inc \
             stars_04.inc \
             stars_05.inc \
             stars_06.inc 

data_DATA = clouds_01.inc \
            clouds_02.inc \
            clouds_03.inc \
            clouds_04.inc \
            clouds_05.inc \
            clouds_06.inc \
            clouds_07.inc \
            lights_standard.inc \
            lights_studio.inc \
            macro_skies.inc \
            skycolor_earth.inc \
            stars_01.inc \
            stars_02.inc \
            stars_03.inc \
            stars_04.inc \
            stars_05.inc \
            stars_06.inc 

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
	if [ ! -d "$(HOME)/.terraform/include/skies/include" ]; then \
	  mkdir "$(HOME)/.terraform/include/skies/include" ; \
	fi;
	cp -f *.inc "$(HOME)/.terraform/include/skies/include";

